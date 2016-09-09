#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include <vector>
#include <algorithm>

using namespace std;
using namespace clang;
using namespace oclint;

//! Code d'erreur retournée par la fonction de comptage d'occurence d'utilisation
typedef enum
{
	ERR_SUCCESS=0, //!< Aucune erreur
	ERR_OVERRUN, //!< la variable est utilisée avec excès

} T_ERR_CODE;

class g_szCurrFunction : public AbstractASTVisitorRule<g_szCurrFunction>
{
private:

	// Memorise la fonction dans lequel on exécute le décompte, afin de repérer les changements ou non de fonction courante
	string g_szCurrFunction = "";

	vector<string> vectVarCntLocal;
	vector<string> vectVarCntGlobal;

	string description = "Le nom de variable est déjà utilisé : ";

	/* Fonction ajoutant une occurrence à une variable déjà existante,
	 * ou une nouvelle structure variable si ce n'est pas le cas.
	 */
	T_ERR_CODE compteOccVariable(string currentVarName, bool bIsCurrentVarGlobal)
	{
		T_ERR_CODE err = ERR_SUCCESS;

		// Pointeur sur la bonne liste. Sert à déterminer si.
		vector<string> * vectCurrentVectVarCnt = 0;

		if( bIsCurrentVarGlobal )
		{
			vectCurrentVectVarCnt = &vectVarCntGlobal;
		}
		else
		{
			vectCurrentVectVarCnt = &vectVarCntLocal;
		}

		// On cherche si le nom de variable est présent dans l'un ou l'autre vecteur
		if ( std::find(vectVarCntGlobal.begin(), vectVarCntGlobal.end(), currentVarName) < vectVarCntGlobal.end() ||
			( std::find(vectVarCntLocal.begin(), vectVarCntLocal.end(), currentVarName) < vectVarCntLocal.end() ))
		{
			// Variable déjà présente. Violation !
			err = ERR_OVERRUN;
		}
		else
		{
			// Variable inconnue au bataillon. On l'ajoute à la liste.
			err = ERR_SUCCESS;

			(*vectCurrentVectVarCnt).push_back(currentVarName);
		}

		return err;
	}

public:
    virtual const string name() const override
    {
        return "conflit nom variable";
    }

    virtual int priority() const override
    {
        return 1;
    }

	virtual const string category() const override
	{
		return "naming";
	}

    virtual void setUp() override {}
    virtual void tearDown() override {}

	/*	On demande a être appelé pour toutes les expressions, l'analyse distingue l'appel de fonction
		d'une affectation de variable.
		Puis on appelle le resultat du décompte. */
	bool VisitVarDecl(VarDecl* node)
	{
		T_ERR_CODE err;
		string currentVarName = node->getNameAsString();
		bool bIsCurrentVarGlobal = ( node->hasGlobalStorage() || node->hasExternalStorage() );

		err = compteOccVariable(currentVarName, bIsCurrentVarGlobal);
		if (err == ERR_OVERRUN )
		{
			addViolation(node, this, description + ( currentVarName ) );
		}

		return true;
	}

	// Detection de nouveau corps de fonction pour réinitialiser la liste
	bool VisitFunctionDecl(FunctionDecl *decl)
	{
		string szFuncName = decl->getNameInfo().getAsString();

		// Si l'on débute l'analyse d'une nouvelle fonction, réinitialiser la liste
		// (il n'est pas interdit d'avoir des variables avec le même nom si elles ne sont pas locales à la même fonction)
		if( decl->hasBody() && ( szFuncName.compare( g_szCurrFunction ) != 0)  )
		{
			vectVarCntLocal.clear();

			// Mémorisation de la fonction en cours, sert à déterminer si l'on a changé de fonction ou pas
			g_szCurrFunction = szFuncName;
		}
		return true;
	}

};

static RuleSet rules(new g_szCurrFunction());
