#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleConfiguration.h"
#include "oclint/RuleSet.h"
#include "oclint/util/StdUtil.h"

using namespace std;
using namespace oclint;

class TailleFichierRule : public AbstractSourceCodeReaderRule
{
private:
	string description = "Le fichier dépasse le nombre de lignes maximal autorisé ";
	int64_t threshold;
	int64_t lineCount = 0 ;
public:
    virtual const string name() const override
    {
        return "taille fichier";
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
    	threshold = RuleConfiguration::intForKey("LONG_FILE", 2000);
	}

    virtual void eachLine(int lineNumber, string line) override
    {
	lineCount++;
    }

    virtual void tearDown() override
	{
    		if(lineCount > threshold)
    			addViolation(0, 0, lineCount, 0, this, description + "(" + toString<int64_t>(lineCount) + ")" );
		// Réinitialisation nécessaire du compteur !!
		lineCount = 0;
	}
};

static RuleSet rules(new TailleFichierRule());
