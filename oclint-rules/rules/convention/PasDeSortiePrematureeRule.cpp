#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"
#include "oclint/util/StdUtil.h"

using namespace std;
using namespace oclint;

class PasDeSortiePrematuree : public AbstractSourceCodeReaderRule
{
private:
	string description = "Les instructions du type break ou exit ne sont pas autorisées.";
	bool isInCase = 0;
	bool isInWhile = 0;
	//bool expectedBreak = 0;
	bool functionHasReturn = 0;

	size_t bracketPositionInLine;

	/* Fonction déterminant si le prochain "break" rencontré est celui d'un switch ou d'un while, et donc, autorisé */
	void isBreakExpected(string line, int lineNumber)
	{
		// Si l'on trouve le début d'un case
		if( (line.find("case") != string::npos) || (line.find("default") != string::npos) )
		{
			isInCase = 1;
		}

    	// Si l'on voit un break qui termine un case ou un default
    	if (isInCase)
    	{
    		if ((line.find("break;") != string::npos) || (line.find("break ;") != string::npos))
			{
    			//expectedBreak = 1;
			}
    	}

    	// Si l'on trouve le début d'un while
    	if( (line.find("while(") != string::npos) || (line.find("while (") != string::npos) )
    	{
			isInWhile = 1;
			bracketPositionInLine = line.find("while");
    		//addViolation(lineNumber, lineNumber, lineNumber, lineNumber, this, "while ouvert: "+ toString<int>(bracketPositionInLine));

    	}
    	// Détection de la fin du while (si une accolade fermante est au même niveau que le while, c'est celle qui le ferme
    	if(isInWhile && (line.find("}") == bracketPositionInLine))
    	{
    		isInWhile = 0;
    		//addViolation(lineNumber, lineNumber, lineNumber, lineNumber, this, "while fermé");
    	}
	}

	/* Fonction indiquant si nous avons changé de fonction ou pas */


public:
    virtual const string name() const override
    {
        return "pas de sortie prematuree";
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

    }

    virtual void eachLine(int lineNumber, string line) override
    {
    	int column;
    	// Détection des breaks "autorisés" (switch cases et boucles while). Doit être placé avant la détection des breaks !
    	isBreakExpected(line, lineNumber);

    	if ((line.find("break;") != string::npos) || (line.find("break ;") != string::npos))
    	{
			// Si le break rencontré n'est pas autorisé
    		//if (!expectedBreak)
    		if(!isInCase && !isInWhile)
    		{
				column = line.find("break");
    			addViolation(lineNumber, column, lineNumber,column,  this, description);
    		}
    		// Sinon, s'il l'est
			else
			{
				isInCase= 0;
				//expectedBreak = 0;
			}

    	}

    	if (line.find("exit(") != string::npos || line.find("exit (") != string::npos)
    	{
			column = line.find("exit");
    		addViolation(lineNumber, column, lineNumber, column,  this, description);
    	}


    }

    virtual void tearDown() override
    {

    }

};

static RuleSet rules(new PasDeSortiePrematuree());
