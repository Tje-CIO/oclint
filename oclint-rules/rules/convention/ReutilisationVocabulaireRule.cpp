#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include <clang/AST/Decl.h>
//#include <vector>
#include "oclint/util/StdUtil.h"
#include "oclint/RuleConfiguration.h"

using namespace std;
using namespace clang;
using namespace oclint;


#define MAX_VAR_CNT 10

//! Code d'erreur retournée par la fonction de comptage d'occurence d'utilisation
typedef enum
{
	ERR_SUCCESS=0, //!< Aucune erreur
	ERR_OVERRUN, //!< la variable est utilisée avec excès

} T_ERR_CODE;

class ReutilisationVocabulaire: public AbstractASTVisitorRule<ReutilisationVocabulaire> {
	// TYPES
	private:
		typedef map<string, unsigned int> T_MAP_VARCNT;
		typedef T_MAP_VARCNT::value_type T_VARCNT_PAIR;
		typedef T_MAP_VARCNT::iterator T_VARCNT_IT;
	private:

		string description = "La variable est utilisée trop de fois : ";


		/* Compte les occurences d'une variable fichier */
		T_MAP_VARCNT  mapVarCnt;

		// Memorise la fonction dans lequel on exécute le décompte
		string szCurrFunction = "";

		// Seuil maximal de comptage
		int iMaxVarCnt = MAX_VAR_CNT;

		/* Fonction ajoutant une occurrence à une variable déjà existante,
		 * ou une nouvelle structure variable si ce n'est pas le cas.
		 */
		T_ERR_CODE compteOccVariable(string currentVarName, DeclRefExpr *node)
		{
			T_ERR_CODE err = ERR_SUCCESS;
			T_VARCNT_IT it = mapVarCnt.find(currentVarName);
			int cnt = 1;
			if( it != mapVarCnt.end() )
			{
				it->second++;
				cnt = it->second;
			}
			else // Nouvelle Variable
			{
				mapVarCnt[ currentVarName ] = 1;
			}
			/*
				On ne va déclarer que la première occurrence en trop car sinon ça pollue trop la lecture
				La feinte : tester à MAX+1
			*/
			if(cnt == (iMaxVarCnt + 1) )
			{
				err = ERR_OVERRUN;
			}
			return err;
		}
	public:
		virtual const string name() const override
		{
			return "reutilisation vocabulaire";
		}

		virtual int priority() const override
		{
			return 1;
		}

		virtual const string category() const override
		{
			return "convention";
		}

		virtual void setUp() override
		{
			iMaxVarCnt = RuleConfiguration::intForKey("MAX_REUTILISATION_VOCABULAIRE", MAX_VAR_CNT);
		}

		/*
			On demande a être appelé pour toutes les expressions, l'analyse distingue l'appel de fonction
			d'une affectation de variable.
			Puis on appelle le resultat du décompte.
		*/
		bool VisitDeclRefExpr(DeclRefExpr *node)
		{
			T_ERR_CODE err;
			string currentVarName = node->getFoundDecl()->getName();

			// Si le DeclRefExpr ne comporte pas de type "function", c'est une variable
			if(!node->getFoundDecl()->getAsFunction())
			{
				err = compteOccVariable(currentVarName, node);
				if (err == ERR_OVERRUN )
				{
					addViolation(node, this, description + " (" + ( currentVarName ) + ")" );
				}
			}
			return true;
		}

		// Detection de nouveau corps de fonction pour réinitialiser les compteurs
		bool VisitFunctionDecl(FunctionDecl *decl)
		{
			string szFuncName = decl->getNameInfo().getAsString();
			// Si l'on débute l'implémentation d'une nouvelle fonction réinitialiser les compteurs
			if( decl->hasBody() && ( szFuncName.compare( szCurrFunction ) != 0)  )
			{
				mapVarCnt.clear();
				szCurrFunction = szFuncName;
			}
			return true;
		}

		virtual void tearDown() override
		{
			mapVarCnt.clear();
		}
};

static RuleSet rules(new ReutilisationVocabulaire());
