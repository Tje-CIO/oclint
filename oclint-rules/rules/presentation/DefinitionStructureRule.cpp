#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"

using namespace std;
using namespace clang;
using namespace oclint;

class DefinitionStructureRule : public AbstractASTVisitorRule<DefinitionStructureRule>
{
private:
	string description = "Dans une définition de structure, les champs doivent apparaître à raison d'un seul par ligne.";
	int32_t lastDeclarationLine;

    int32_t getLine(clang::Decl *decl)
    {
    	int32_t lineNumber;

		clang::SourceManager *sourceManager = &_carrier->getSourceManager();
		lineNumber = sourceManager->getPresumedLineNumber(decl->getLocStart());

    	return lineNumber;
    }

public:
    virtual const string name() const override
    {
        return "definition structure";
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


    /* Visit FieldDecl */
    /* On enregistre la ligne de chaque déclaration de champ de structure,
     * et si on a deux champs sur la même ligne, on signale la violation
     */
    bool VisitFieldDecl(FieldDecl *node)
    {
    	int currentLine = getLine(node);

		if(lastDeclarationLine == currentLine)
		{
			addViolation(node, this, description);
		}

		lastDeclarationLine = currentLine;
		return true;
    }

};

static RuleSet rules(new DefinitionStructureRule());
