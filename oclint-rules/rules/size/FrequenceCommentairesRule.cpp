#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"
#include "oclint/RuleConfiguration.h"
#include "oclint/util/StdUtil.h"
#include <regex.h>

using namespace std;
using namespace oclint;

class FrequenceCommentairesRule : public AbstractSourceCodeReaderRule
{
private:
	string description = "La fonction ne respecte pas la fréquence de commentaires demandée. ";
	string tooFewComments = "Trop peu de commentaires.";
	string tooManyComments = "Trop de commentaires.";

	bool isInFunction;
	int functionStartLine;
	int openBrackets;
	int nonEmptyLine;
	int commentLinesNumber;

	float upperThreshold;
	float lowerThreshold;

    /*** Implémentation future ***/

    // fonction indiquant si l'accolade trouvée est dans une chaîne de caractères (et devrait donc être ignorée)
	/*bool computeIsInAString(size_t bracketPos, size_t firstQuotePos, size_t secondQuotePos)
	{

		bool isInAString;

		if((int)bracketPos > (int)firstQuotePos && (int)bracketPos < (int)secondQuotePos)
		{
			isInAString = 1;
		}
		else
		{
			isInAString = 0;
		}
		return isInAString;
	}

	// Fonction retournant la position d'une accolade s'il y en a une sur la ligne, et string::npos (= non trouvé) sinon
	size_t getFirstBracketFound( size_t openingBracket, size_t closingBracket)
	{
		size_t firstBracketFound;

		if(openingBracket != string::npos)
		{
			// Si on a une accolade ouvrante, on renvoie sa position
			firstBracketFound = openingBracket;
		}
		else if(closingBracket != string::npos)
		{
			// Sinon, on renvoie la position de l'accolade fermante
			firstBracketFound = closingBracket;
		}
		else
		{
			// Si l'on n'a trouvé ni l'une ni l'autre, on renvoie ce résultat:
			firstBracketFound = string::npos;
		}

		return firstBracketFound;
	}

	bool processBracket(size_t bracketPosition)
	{

		return true;
	}*/

	bool isFunctionStart(string line, int lineNumber)
	{
		const char *pattern = "\\w+\\**\\s\\w+\\(\\s*(\\w+\\**\\s?\\**\\w*,*\\s*)*\\)+";

		int err, match;
		bool functionStart = 0;
		regex_t preg;

		// test de la validité du pattern
		err = regcomp (&preg, pattern, REG_NOSUB | REG_EXTENDED);
		if (err == 0)
		{

			// matching
			// match = 0 si match ok, REG_NOMATCH sinon, et ni l'un ni l'autre en cas d'erreur
			match = regexec (&preg, line.c_str(), 0, NULL, 0);

			regfree (&preg); 	// libération de la mémoire allouée pour la compilation de l'expression régulière

			// Si la ligne ne comporte pas de déclaration de fonction
			if (match == REG_NOMATCH)
			{
				functionStart = 0;
			}
			// S'il y a une déclaration de fonction sur cette ligne
			else if (match == 0)
			{
				functionStart = 1;
			}
			// si erreur
			else
			{
				addViolation(lineNumber, 0, lineNumber, 0, this, "L'expression reguliere n'a pas pu etre comparee");
		         char *text;
		         size_t size;

		         size = regerror (err, &preg, NULL, 0);
		         text = (char*)malloc (sizeof (char) * size);
		         if (text)
		         {
		            regerror (err, &preg, text, size);
		            addViolation(lineNumber, 0, lineNumber, 0, this, text);

		            free (text);
		         }
		         else
		         {
		        	 addViolation(lineNumber, 0, lineNumber, 0, this, "Memoire insuffisante");
		         }
		         functionStart = 0;
			}	// fin du cas d'erreur
		}
		return functionStart;
	}

	void processCommentRatio(int nonEmptyLine, int commentLinesNumber)
	{
		int codeLinesNumber;	// On suppose qu'il sera systématiquement non-nul ...
		float ratio;

		codeLinesNumber = nonEmptyLine - commentLinesNumber;
		ratio = (float)commentLinesNumber/(float)codeLinesNumber;
		if (ratio < lowerThreshold)
		{
			addViolation(functionStartLine, 0, functionStartLine, 0, this, description + tooFewComments);
		}
		if (ratio > upperThreshold)
		{
			addViolation(functionStartLine, 0, functionStartLine, 0, this, description + tooManyComments);
		}
	}
public:
    virtual const string name() const override
    {
        return "frequence commentaires";
    }

    virtual int priority() const override
    {
        return 1;
    }

    virtual const string category() const override
    {
        return "size";
    }

    virtual void setUp() override
    {
        upperThreshold = RuleConfiguration::doubleForKey("MAX_COMMENTS_FREQUENCY", 5);
        lowerThreshold = RuleConfiguration::doubleForKey("MIN_COMMENTS_FREQUENCY", 0.2);
    	isInFunction = 0;
    	openBrackets = 0;
    	functionStartLine = 0;
    	nonEmptyLine = 0;
    	commentLinesNumber = 0;
    }

    virtual void eachLine(int lineNumber, string line) override
    {
    	int anyChar;
    	if (isInFunction)
    	{
    		// Est-on sur une ligne non vide?
    		anyChar = line.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789;()[]/",0);

    		if (anyChar != string::npos)
    		{
    			nonEmptyLine ++;
    		}

    		// Y a-t-il des commentaires sur la ligne
    		if (line.find("//") != string::npos)
    		{
    			commentLinesNumber ++;
    		}

    		// Si on ouvre des accolades
    		if (line.find('{') != string::npos)
    		{
    			openBrackets++;
    		}

    		// Si on en ferme
    		if (line.find('}') != string::npos)
    		{
    			openBrackets--;
    		}
    		// Si la fonction est terminée
    		if (openBrackets == 0)
    		{
    			processCommentRatio(nonEmptyLine, commentLinesNumber);
    			commentLinesNumber = 0;
    			nonEmptyLine = 0;
    			isInFunction = 0;
    		}
    	}

    	if(isFunctionStart(line, lineNumber))
    	{
			isInFunction = 1;
    		functionStartLine = lineNumber;

    		return;
    	}

    }

    virtual void tearDown() override
	{

	}

};

static RuleSet rules(new FrequenceCommentairesRule());
