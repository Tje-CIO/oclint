#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include <array>

using namespace std;
using namespace clang;
using namespace oclint;

class TypeVariableRule : public AbstractASTVisitorRule<TypeVariableRule>
{
private:
	string description = " : le type de cette variable n'est pas autorisé.";

	// Liste des types proscrits
	std::array<string, 7> bannedTypeList = { "short", "int", "long", "unsigned char", "unsigned short", "unsigned int", "unsigned long long"};
	std::array<string, 4> qualTypeList = {"const", "volatile", "restrict", "_Atomic"};
	bool isVarTypeForbidden(string currentVarType, Decl* decl )
	{
		bool isForbidden = 0;
		size_t position;

		// "Nettoyage" des types reçus : on enlève les "Type qualifiers"
		for(string* qualType = qualTypeList.begin(); qualType != qualTypeList.end(); ++qualType)
		{
			// On cherche si l'un des type qualifiers est présent dans le type de la variable ...
			position = currentVarType.find(*qualType);
			if (position != string::npos)
			{
				// ... Si c'est le cas, on l'enlève avant de procéder à l'analyse
				currentVarType.erase(position, (*qualType).size()+1);	// +1 pour supprimer l'espace parasite restant
			}
		}

		if (std::find(std::begin(bannedTypeList), std::end(bannedTypeList), currentVarType) != std::end(bannedTypeList))
		{
			isForbidden= 1;
		}
		return isForbidden;
	}
public:
    virtual const string name() const override
    {
        return "types variable";
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

    /* Visit VarDecl */
    bool VisitVarDecl(VarDecl *decl)
    {
    	string varType = decl->getType().getAsString();
    	string varName = decl->getNameAsString();
    	if(isVarTypeForbidden(varType, decl))
    	{
    		addViolation(decl, this, varType + " " + varName + description);
    	}

    	return true;
    }
    
    /* Visit FieldDecl */
    bool VisitFieldDecl(FieldDecl *decl)
    {
    	string varType = decl->getType().getAsString();
    	string varName = decl->getNameAsString();
    	if(isVarTypeForbidden(varType, decl))
    	{
    		addViolation(decl, this, varType + " " + varName + description);
    	}

    	return true;
    }

    /* Visit ParmVarDecl */
    bool VisitParmVarDecl(ParmVarDecl *decl)
    {
    	string varType = decl->getType().getAsString();
    	string varName = decl->getNameAsString();
    	if(isVarTypeForbidden(varType, decl))
    	{
    		addViolation(decl, this, varType + " " + varName + description);
    	}

    	return true;
    }

};

static RuleSet rules(new TypeVariableRule());
