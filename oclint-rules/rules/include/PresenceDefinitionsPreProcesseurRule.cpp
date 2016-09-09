#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"
#include <libgen.h>
#include <string.h>

using namespace std;
using namespace oclint;

class PresenceDefinitionsPreProcesseurRule : public AbstractSourceCodeReaderRule
{
private:
    string ifndefDescription = "Pas d'instruction preprocesseur du type \"#ifndef\" ";
    string defineDescription = "Pas d'instruction preprocesseur du type \"#define\" ";
    string endifDescription  = "Pas d'instruction preprocesseur du type \"#endif\" ";

    bool isIncludeFile = 0;
    bool hasIfndef = 0;
    bool hasDefine = 0;
    bool hasEndif = 0;

    void computeIsIncludeFile(string filePath)
    {
		/* On cherche si le fichier est un .h */

    	// Récupération du nom du fichier
    	char *tempFileName = strdup(filePath.c_str());
		string fileName = basename(tempFileName);
    	free(tempFileName);
    	// Vérification de l'extension
		if (fileName.find(".h")!= string::npos )
        {
			isIncludeFile = 1;
        }
        else
        {
        	isIncludeFile = 0;
        }
    }
public:
    virtual const string name() const override
    {
        return "presence definitions pre processeur";
    }

    virtual int priority() const override
    {
        return 1;
    }

    virtual const string category() const override
    {
        return "include";
    }

    // Cette fonction vérifie si le fichier traité est un fichier include ou pas, afin de gagner du temps
    virtual void setUp() override
    {
        string filePath = _carrier->getMainFilePath();

        computeIsIncludeFile(filePath);
    }

    virtual void eachLine(int lineNumber, string line) override
    {
		if(isIncludeFile)
        {
        	// Vérification de la présence des 3 instructions préprocesseur requises
        	if (hasIfndef || (line.find("#ifndef") != string::npos) )
        	{
        		hasIfndef = 1;

				if (hasDefine || (line.find("#define") != string::npos) )
				{
					hasDefine = 1;

					if (hasEndif || (line.find("#endif") != string::npos) )
					{
						hasEndif = 1;
					}
				}
        	}
        }
    }

    // Cette fonction indique s'il y a violation de la règle ou non
    virtual void tearDown() override
    {
		if(isIncludeFile)
		{
			if(!hasIfndef)
				addViolation(0, 0, 0, 0, this, ifndefDescription);
			else if (!hasDefine)
				addViolation(0, 0, 0, 0, this, defineDescription);
			else if (!hasEndif)
				addViolation(0, 0, 0, 0, this, endifDescription);
		}
    }

};

static RuleSet rules(new PresenceDefinitionsPreProcesseurRule());
