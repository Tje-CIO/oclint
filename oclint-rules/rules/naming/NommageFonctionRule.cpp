#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include <regex.h>
#include <libgen.h>

using namespace std;
using namespace clang;
using namespace oclint;

class NommageFonctionRule : public AbstractASTVisitorRule<NommageFonctionRule>
{

private:
	string description = " : le nom de la fonction ne respecte pas la norme de nommage";
	string filePath;

	string extractLastFolder(string filePath)
	{
        int sep;
        string lastFolder;

        // Extraction du chemin (dossiers seulement)
		char *cFilePath = strdup(filePath.c_str());
        string fileDir = dirname(cFilePath);

        // Extraction du dernier dossier de l'arborescence
        sep = fileDir.rfind('/');
        lastFolder = fileDir.substr(sep+1, fileDir.size());

        free(cFilePath);

		return lastFolder;
	}
public:
    virtual const string name() const override
    {
        return "nommage fonction";
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
        filePath = _carrier->getMainFilePath();
	}
    virtual void tearDown() override {}

    /* Visit FunctionDecl */
    bool VisitFunctionDecl(FunctionDecl *node)
    {
    	/*** Construction du pattern ***/
    	string currentDir = extractLastFolder(filePath);

    	// Le nom doit commencer par le nom du répertoire courant, suivi d'un '_', et finalement du nom réel de la fonction
    	string s_pattern = "^" + currentDir + "_\\w+";
    	const char *pattern = s_pattern.c_str();
		int err, match;
		regex_t preg;

		string functionName = node->getName();

		if (functionName != "main")
		{
			// test de la validité du pattern
			err = regcomp (&preg, pattern, REG_NOSUB | REG_EXTENDED);
			if (err == 0)
			{

				// matching
				// match = 0 si match ok, REG_NOMATCH sinon, et ni l'un ni l'autre en cas d'erreur
				match = regexec (&preg, functionName.c_str(), 0, NULL, 0);

				regfree (&preg); 	// libération de la mémoire allouée pour la compilation de l'expression régulière

				// si le nom de la fonction n'est pas conforme
				if (match == REG_NOMATCH)
				{
					addViolation(node, this, functionName + description);
				}
				// Si la fonction est bien nommée ...
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

static RuleSet rules(new NommageFonctionRule());
