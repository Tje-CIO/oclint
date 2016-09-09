#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"

using namespace std;
using namespace clang;
using namespace oclint;

class UneDeclarationParLigneRule : public AbstractASTVisitorRule<UneDeclarationParLigneRule>
{
private:
	string description = "Une seule déclaration de variable ou fonction par ligne est autorisée.";
	int32_t lastVarDecl;
	int32_t lastFunctionDecl;


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
        return "une declaration par ligne";
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
    	lastVarDecl = 0;
    	lastFunctionDecl = 0;
	}

    virtual void tearDown() override {}

    /* Visit Decl */
    bool VisitFunctionDecl(FunctionDecl* node)
    {
    	int currentLine = getLine(node);

		if(lastFunctionDecl == currentLine)
		{
			addViolation(node, this, description);
		}

		lastFunctionDecl = currentLine;
    	return true;
    }

    /* Visit Decl */
    bool VisitVarDecl(VarDecl* node)
    {
    	int currentLine = getLine(node);
    	// Cast dynamique pour vérifier que l'on n'a pas affaire à un ParmVarDecl. Renvoie nullptr en cas d'erreur.
		if (dyn_cast<ParmVarDecl>(node) == nullptr)
    	{
			if(lastVarDecl == currentLine)
			{
				addViolation(node, this, description);
			}

			lastVarDecl = currentLine;
    	}
		return true;

    }

};

static RuleSet rules(new UneDeclarationParLigneRule());
