#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"

using namespace std;
using namespace oclint;

class BlocDoxygenFichierRule : public AbstractSourceCodeReaderRule
{
private:
	string descriptionDoxygenTag = "Pas de bloc doxygen dans ce fichier ou bloc incomplet";
	string descriptionStart = "Pas de balise doxygen \" ";
	string descriptionEnd = " \" dans ce fichier";

	const string fileTag = "\\file";
	const string briefTag = "\\brief";
	const string authorTag = "\\author";
	const string versionTag = "\\version";
	const string dateTag = "\\date";
	const string copyrightTag = "\\copyright";

	bool hasDoxygenTag = 0;
	bool hasFileTag = 0;
	bool hasBriefTag = 0;
	bool hasAuthorTag = 0;
	bool hasVersionTag = 0;
	bool hasDateTag = 0;
	bool hasCopyrightTag = 0;

public:
    virtual const string name() const override
    {
        return "bloc doxygen fichier";
    }

    virtual int priority() const override
    {
        return 1;
    }

    virtual const string category() const override
    {
        return "presentation";
    }

    virtual void setUp() override
    {

    }

    virtual void eachLine(int lineNumber, string line) override
    {
    	/*** Vérification de la présence des balises requises ***/
    	// On cherche l'ouverture d'un bloc doxygen au format /*! dans les deux premières lignes.
    	if (hasDoxygenTag || ((line.find("/*!") != string::npos) && lineNumber <= 2))
    	{
    		hasDoxygenTag = 1;
    	}
    	if (hasDoxygenTag)
    	{
			if (hasFileTag || (line.find(fileTag) != string::npos) )
			{
				hasFileTag = 1;
			}
			if (hasBriefTag || (line.find(briefTag) != string::npos) )
			{
				hasBriefTag = 1;
			}
			if (hasAuthorTag || (line.find(authorTag) != string::npos) )
			{
				hasAuthorTag = 1;
			}
			if (hasVersionTag || (line.find(versionTag) != string::npos) )
			{
				hasVersionTag = 1;
			}
			if (hasDateTag || (line.find(dateTag) != string::npos) )
			{
				hasDateTag = 1;
			}
			if (hasCopyrightTag || (line.find(copyrightTag) != string::npos) )
			{
				hasCopyrightTag = 1;
			}
    	}
    }

    virtual void tearDown() override
    {
		if(!hasDoxygenTag)
			addViolation(0, 0, 0, 0, this, descriptionDoxygenTag);
		else if (!hasFileTag)
			addViolation(0, 0, 0, 0, this, descriptionStart + fileTag + descriptionEnd);
		else if (!hasBriefTag)
			addViolation(0, 0, 0, 0, this, descriptionStart + briefTag + descriptionEnd);
		else if (!hasAuthorTag)
			addViolation(0, 0, 0, 0, this, descriptionStart + authorTag + descriptionEnd);
		else if (!hasVersionTag)
			addViolation(0, 0, 0, 0, this, descriptionStart + versionTag + descriptionEnd);
		else if (!hasDateTag)
			addViolation(0, 0, 0, 0, this, descriptionStart + dateTag + descriptionEnd);
		else if (!hasCopyrightTag)
			addViolation(0, 0, 0, 0, this, descriptionStart + copyrightTag + descriptionEnd);

    }

};

static RuleSet rules(new BlocDoxygenFichierRule());
