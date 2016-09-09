#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"

using namespace std;
using namespace oclint;

class UneInstructionParLigneRule : public AbstractSourceCodeReaderRule
{
private:
	string description = "Une seule instruction par ligne est autorisée";

	bool isNotAComment(string line, size_t startIndex)
	{
		bool result = 0;

		// Si l'on ne trouve ni '//', ni '/*' sur la ligne à partir de l'index indiqué,
		// c'est qu'il n'y avait pas encore de commentaire
		if (line.find("//", startIndex) == string::npos && line.find("/*", startIndex) == string::npos)
		{
			result = 1;
		}

		return result;
	}
public:
    virtual const string name() const override
    {
        return "une instruction par ligne";
    }

    virtual int priority() const override
    {
        return 1;
    }

    virtual const string category() const override
    {
        return "presentation";
    }

    virtual void setUp() override {}
    virtual void tearDown() override {}

    virtual void eachLine(int lineNumber, string line) override
    {
    	size_t firstSemiColon, secondSemiColon, thirdSemiColon;

    	// Si un premier ';' est trouvé sur la ligne ...
    	firstSemiColon = line.find(';');
    	if(firstSemiColon != string::npos && isNotAComment(line, firstSemiColon))
    	{
    		// Si la ligne n'est pas une declaration de boucle for
    		if(line.find("for")==string::npos)
    		{
				// ... on en cherche un second après le premier
				secondSemiColon = line.find(';', firstSemiColon+1);
				if(secondSemiColon != string::npos && isNotAComment(line, secondSemiColon))
				{
					addViolation(lineNumber, secondSemiColon, lineNumber, secondSemiColon, this, description);
				}
    		}
    		// Si l'on a affaire à une boucle for, 2 ';' sont acceptés
    		else
    		{
				secondSemiColon = line.find(';', firstSemiColon+1);
				if(secondSemiColon != string::npos && isNotAComment(line, secondSemiColon))
				{
					thirdSemiColon = line.find(';', secondSemiColon+1);
					if(thirdSemiColon != string::npos && isNotAComment(line, thirdSemiColon))
					{
						addViolation(lineNumber, thirdSemiColon, lineNumber, thirdSemiColon, this, description);
					}
				}
    		}
    	}
    }
};

static RuleSet rules(new UneInstructionParLigneRule());
