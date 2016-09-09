#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include <regex.h>
#include "oclint/RuleConfiguration.h"

using namespace std;
using namespace clang;
using namespace oclint;

#define SZ_VARLIST_ALLWD "i,j,k,l"

class NommageVariableLocaleFonctionRule : public AbstractASTVisitorRule<NommageVariableLocaleFonctionRule>
{
	private:
		string description = " : le nom de la variable ne respecte pas la norme de nommage";
		bool isInFunction;
		// La liste des noms de variables autorisées, avec un séparateur quelconque
		string szVarListAllwd = SZ_VARLIST_ALLWD;

	public:
		virtual const string name() const override
		{
			return "nommage variable locale fonction";
		}

		virtual int priority() const override
		{
			return 1;
		}

		virtual const string category() const override
		{
			return "naming";
		}

		virtual void setUp() override
		{
			szVarListAllwd = RuleConfiguration::stringForKey("NOMMAGE_VARLOC_EXCEPTION", SZ_VARLIST_ALLWD);
		}
		virtual void tearDown() override {}


		/* Visit FunctionDecl */
		/* Algorithme de parcourt :
		 * 1) Dans tous les éléments d'un FunctionDecl, chercher les DeclStmt
		 * 2) Une fois trouvés, les parcourir (VisitDeclStmt). Cela aura pour effet de
		 * parcourir aussi VisitVarDecl et d'effectuer l'analyse.
		 */
		bool VisitFunctionDecl(FunctionDecl *node)
		{
			isInFunction = 1;

			if(node->hasBody())
			{
				// Perform a bottom up traversal of all children.
				for (Stmt *SubStmt : node->getBody()->children())
				{
					if (SubStmt)
					{
						DeclStmt *DC = dyn_cast_or_null<DeclStmt >(SubStmt);
						if (DC != nullptr)
						{
							// Une fois trouvés les DeclStmt, VisitVarDecl va être parcourue automatiquement
							TraverseDeclStmt(DC);
						}
					}
				}
			}
			isInFunction = 0;
			return true;
		}

		/* Visit VarDecl */
		bool VisitVarDecl(VarDecl *node)
		{

			if(isInFunction)
			{
				// préfixe "l_"
				const char *pattern = "l_\\w{1,}";

				int err, match;
				regex_t preg;

				string variableName = node->getName();

				// Vérification de la correspondance avec un nom de variable autorisé
				if( szVarListAllwd.find( variableName ) != string::npos )
				{
					return true;
				}
				
				// test de la validité du pattern
				err = regcomp (&preg, pattern, REG_NOSUB | REG_EXTENDED);
				if (err == 0)
				{

					// matching
					// match = 0 si match ok, REG_NOMATCH sinon, et ni l'un ni l'autre en cas d'erreur
					match = regexec (&preg, variableName.c_str(), 0, NULL, 0);

					regfree (&preg); 	// libération de la mémoire allouée pour la compilation de l'expression régulière

					// si le nom de la variable n'est pas conforme
					if (match == REG_NOMATCH)
					{
						addViolation(node, this, variableName + description);
					}
					// Si la variable est bien nommée ...
					else if (match == 0)
					{
						// ... ne rien faire
					}
					// si erreur
					else
					{
						addViolation(node, this, "L'expression reguliere n'a pas pu etre comparee");
						char *text;
						size_t size;

						size = regerror (err, &preg, NULL, 0);
						text = (char*)malloc (sizeof (char) * size);
						if (text)
						{
							regerror (err, &preg, text, size);
							addViolation(node, this, text);

							free (text);
						}
						else
						{
							addViolation(node, this, "Memoire insuffisante");
						}

					}	// fin du cas d'erreur
				}
			}
			return true;
		}
};

static RuleSet rules(new NommageVariableLocaleFonctionRule());
