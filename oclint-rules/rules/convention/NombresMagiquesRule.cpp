#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"
#include <regex.h>

using namespace std;
using namespace oclint;

class NombresMagiquesRule : public AbstractSourceCodeReaderRule
{
private:
	string description = "Les nombres magiques ne sont pas autorisés.";

public:
    virtual const string name() const override
    {
        return "nombres magiques";
    }

    virtual int priority() const override
    {
        return 1;
    }

    virtual const string category() const override
    {
        return "convention";
    }

    virtual void setUp() override {}
    virtual void tearDown() override {}

    virtual void eachLine(int lineNumber, string line) override
    {
		// Variables pour le regex
		int err, errException, match, matchException;
		regex_t preg, pregException;
		bool patternHasError, patternExceptionHasError;

		// pattern d'une déclaration de variable + affectation d'une valeur numérique.
		const char *pattern = "\\w+\\s*=\\s*[0-9]+\\s*;";

		// pattern d'une déclaration de variable + affectation d'exactement un 0 ou un 1.
		const char *patternException = "\\w+\\s*=\\s*[01]\\s*;";

		// test de la validité du pattern
		err = regcomp (&preg, pattern, REG_NOSUB | REG_EXTENDED);
		errException = regcomp (&pregException, patternException, REG_NOSUB | REG_EXTENDED);
		if (err == 0 && errException == 0)
		{
			// matching
			// match = 0 si match ok, REG_NOMATCH sinon, et ni l'un ni l'autre en cas d'erreur
			match = regexec (&preg, line.c_str(), 0, NULL, 0);
			matchException = regexec (&pregException, line.c_str(), 0, NULL, 0);

			// libération de la mémoire allouée pour la compilation de l'expression régulière
			regfree (&preg);
			regfree (&pregException);

			patternHasError = (match != REG_NOMATCH && match != 0);
			patternExceptionHasError = (matchException != REG_NOMATCH && matchException != 0);
			// si aucun match n'est fait ...
			if (match == REG_NOMATCH)
			{
				// ... ne rien faire
			}
			// Si un match est trouvé
			else if (match == 0)
			{
				// Et qu'il ne s'agit pas d'une exception (affectation d'un 1 ou 0)
				if(matchException == REG_NOMATCH)
					addViolation(lineNumber, 0, 0, 0, this, description);
			}
			// si erreur pour l'un ou pour l'autre matching
			// Duplication de code. Pas élégant, mais la factorisation n'est qu'à peine mieux,
			// compte tenu du nombre de paramètres exploités par cette portion de code ...
			else if(patternHasError)
			{
				addViolation(lineNumber, 0, 0, 0, this, "L'expression reguliere n'a pas pu etre comparee");
				 char *text;
				 size_t size;

				 size = regerror (err, &preg, NULL, 0);
				 text = (char*)malloc (sizeof (char) * size);
				 if (text)
				 {
					regerror (err, &preg, text, size);
					addViolation(lineNumber, 0, 0, 0, this, text);

					free (text);
				 }
				 else
				 {
					 addViolation(lineNumber, 0, 0, 0, this, "Memoire insuffisante");
				 }


			}
			else if(patternExceptionHasError)
			{
				addViolation(lineNumber, 0, 0, 0, this, "L'expression reguliere n'a pas pu etre comparee");
				 char *textException;
				 size_t sizeException;

				 sizeException = regerror (errException, &pregException, NULL, 0);
				 textException = (char*)malloc (sizeof (char) * sizeException);
				 if (textException)
				 {
					regerror (errException, &pregException, textException, sizeException);
					addViolation(lineNumber, 0, 0, 0, this, textException);

					free (textException);
				 }
				 else
				 {
					 addViolation(lineNumber, 0, 0, 0, this, "Memoire insuffisante");
				 }


			}	// fin du cas d'erreur. Tous les cas ont été traités.
		}
    }
};

static RuleSet rules(new NombresMagiquesRule());
