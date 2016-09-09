#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include <regex.h>

using namespace std;
using namespace clang;
using namespace oclint;

class NommageVariableGlobaleRule : public AbstractASTVisitorRule<NommageVariableGlobaleRule>
{
private:
	string description = " : le nom de la variable globale ne respecte pas la norme de nommage";
	bool isInFunction;

public:
    virtual const string name() const override
    {
        return "nommage variable globale";
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

    /* Visit VarDecl */
    bool VisitVarDecl(VarDecl *node)
    {
		if(node->hasGlobalStorage())
		{
			// préfixe "l_"
			const char *pattern = "g_\\w{1,}";

			int err, match;
			regex_t preg;

			string variableName = node->getName();

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

static RuleSet rules(new NommageVariableGlobaleRule());
