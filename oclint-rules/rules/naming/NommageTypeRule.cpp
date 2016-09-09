#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include <regex.h>

using namespace std;
using namespace clang;
using namespace oclint;

class NommageTypeRule : public AbstractASTVisitorRule<NommageTypeRule>
{
private:
	string description = " : le nom du type ne respecte pas la norme de nommage";
public:
    virtual const string name() const override
    {
        return "nommage type";
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

    /* Visit TypedefDecl */
    bool VisitTypedefDecl(TypedefDecl *node)
    {
		// commence par "T_" suivi de lettres majuscules
		const char *pattern = "^(T_)[A-Z]+$";

		int err, match;
		regex_t preg;

		string typeName = node->getName();

		// test de la validité du pattern
		err = regcomp (&preg, pattern, REG_NOSUB | REG_EXTENDED);
		if (err == 0)
		{

			// matching
			// match = 0 si match ok, REG_NOMATCH sinon, et ni l'un ni l'autre en cas d'erreur
			match = regexec (&preg, typeName.c_str(), 0, NULL, 0);

			regfree (&preg); 	// libération de la mémoire allouée pour la compilation de l'expression régulière

			// si le nom ddu type n'est pas conforme
			if (match == REG_NOMATCH)
			{
				addViolation(node, this, typeName + description);
			}
			// Si le type est bien nommé ...
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
    	return true;
    }

};

static RuleSet rules(new NommageTypeRule());
