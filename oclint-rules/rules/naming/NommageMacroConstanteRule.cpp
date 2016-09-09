#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"
#include <regex.h>
#include "oclint/AbstractASTVisitorRule.h"

using namespace std;
using namespace oclint;

class NommageMacroConstanteRule : public AbstractSourceCodeReaderRule
{
private:
	string description = " : le nom de la constante ne respecte pas la norme de nommage";
	int currentLineSize;
	// Macro : #define suivi de deux mots (un mot : majuscule, minuscule, chiffre, et '_')
	const char* MacroPattern = "^(#define)\\s+\\w+\\s+\\w+";
	// Macro : idem, avec le nom de la macro (1er mot) qui doit commencer par "C_" et être suivi de lettres majuscules et de '_'
	const char* rightNamePattern = "^(#define)\\s+C_[A-Z_]+\\s+\\w+";

	bool isAMacro(string line, int lineNumber)
	{
		int err, match;
		regex_t preg;

		// test de la validité du pattern
		err = regcomp (&preg, MacroPattern, REG_NOSUB | REG_EXTENDED);
		if (err == 0)
		{

			// matching
			// match = 0 si match ok, REG_NOMATCH sinon, et ni l'un ni l'autre en cas d'erreur
			match = regexec (&preg, line.c_str(), 0, NULL, 0);

			regfree (&preg); 	// libération de la mémoire allouée pour la compilation de l'expression régulière

			// si le nom de la constante n'est pas conforme
			if (match == REG_NOMATCH)
			{
				return false;
			}
			// Si la constante est bien nommée ...
			else if (match == 0)
			{
				// ... ne rien faire
				return true;
			}
			// si erreur
			else
			{
				addViolation(lineNumber, 1, lineNumber, currentLineSize, this, "L'expression regulière n'a pas pu être comparée");
		        char *text;
		        size_t size;

		        size = regerror (err, &preg, NULL, 0);
		        text = (char*)malloc (sizeof (char) * size);
		        if (text)
		        {
		        	regerror (err, &preg, text, size);
		            addViolation(lineNumber, 1, lineNumber, currentLineSize, this, text);

		            free (text);
		        }
		        else
		        {
		        	addViolation(lineNumber, 1, lineNumber, currentLineSize, this, "L'expression regulière n'a pas pu etre comparée, problème de mémoire");
		        }
		        return false;
			}	// fin du cas d'erreur

		}
		return false;

	}

	bool isWellNamed(string line, int lineNumber)
	{
		int err, match;
		regex_t preg;

		// test de la validité du pattern
		err = regcomp (&preg, rightNamePattern, REG_NOSUB | REG_EXTENDED);
		if (err == 0)
		{

			// matching
			// match = 0 si match ok, REG_NOMATCH sinon, et ni l'un ni l'autre en cas d'erreur
			match = regexec (&preg, line.c_str(), 0, NULL, 0);

			regfree (&preg); 	// libération de la mémoire allouée pour la compilation de l'expression régulière

			// si le nom de la constante n'est pas conforme
			if (match == REG_NOMATCH)
			{
				return false;
			}
			// si la constante est bien nommée ...
			else if (match == 0)
			{
				// ... ne rien faire
				return true;
			}
			// si erreur
			else
			{
				addViolation(lineNumber, 1, lineNumber, currentLineSize, this, "L'expression regulière n'a pas pu etre comparée");
				char *text;
		         size_t size;

		         size = regerror (err, &preg, NULL, 0);
		         text = (char*)malloc (sizeof (char) * size);
		         if (text)
		         {
		            regerror (err, &preg, text, size);
		            addViolation(lineNumber, 1, lineNumber, currentLineSize, this, text);

		            free (text);
		         }
		         else
		         {
						addViolation(lineNumber, 1, lineNumber, currentLineSize, this, "L'expression regulière n'a pas pu etre comparée, problème de mémoire");
		         }
			}	// fin du cas d'erreur
			return false;

		}
		return false;
	}
public:
    virtual const string name() const override
    {
        return "nommage macro constante";
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
    }
    virtual void tearDown() override
    {
    }

    // Fonction cherchant les macro : "#define texte valeur
    // Se différencie des #define assiociés aux #ifndef par le fait qu'il y ait 2 mots derrière
    virtual void eachLine(int lineNumber, string line) override
    {
    	// Si nous avons un #define à cette ligne
    	if(line.find("#define") != string::npos)
    	{
    		// Et si c'est bien une macro
    		if(isAMacro(line, lineNumber))
    		{
    			// Il faut qu'elle soit bien formatée
    			if (!isWellNamed(line, lineNumber))
    			{
        			currentLineSize = line.size();
        			addViolation(lineNumber, 1, lineNumber, currentLineSize, this, line + description);
    			}
    		}
    	}
    }
};

static RuleSet rules(new NommageMacroConstanteRule());
