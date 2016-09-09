#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include "oclint/RuleConfiguration.h"
#include "oclint/util/StdUtil.h"

using namespace std;
using namespace clang;
using namespace oclint;

class FonctionParFichierRule : public AbstractASTVisitorRule<FonctionParFichierRule>
{
private:
	string description = "Ce fichier comporte un trop grand nombre de fonctions";
	int functionCount;
	int threshold;
	FunctionDecl *firstFaultyFunction;
public:
    virtual const string name() const override
    {
        return "fonction par fichier";
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
    	threshold = RuleConfiguration::intForKey("FONCTION_PAR_FICHIER", 10);
	}

    /* Visit FunctionDecl */
    bool VisitFunctionDecl(FunctionDecl *node)
    {

    	//On ne compte pas les prototypes et fonctions vides
        if(node->hasBody() && node->isThisDeclarationADefinition())
    	{
    		functionCount++;
			// Ceci permet d'indiquer le numéro de ligne de la première fonction en infraction,
			// puisqu'il faut indiquer un objet Decl ou Stmt à addViolation
			if (functionCount==threshold+1)
				firstFaultyFunction = node;
    	}
        return true;
    }

    virtual void tearDown() override
	{

		if (functionCount>threshold)
			addViolation( firstFaultyFunction, this, description + " (" + toString<int>(functionCount) + ").");
		// Réinitialisation nécessaire du compteur
		functionCount = 0;
	}
};

static RuleSet rules(new FonctionParFichierRule());
