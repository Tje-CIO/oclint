#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include "oclint/util/StdUtil.h"
#include "oclint/RuleConfiguration.h"

using namespace std;
using namespace clang;
using namespace oclint;

#define MAX_RETCNT 2

class NbreSortiesParFonctionRule : public AbstractASTVisitorRule<NbreSortiesParFonctionRule>
{
	private:
		string description = "Nbre de point de sortie autorisé par fonction dépassé ";

		int iMaxRetCnt = MAX_RETCNT;

		/* Fonction récursive allant chercher les ReturnStmt dans l'arborescence */
		void searchRetStmts(Stmt *S, unsigned int  &iReturnCnt)
		{
			// Perform a bottom up traversal of all children.
			for (Stmt *SubStmt : S->children())
			{
				// Test d'abord de l'expression 
				if (isa<ReturnStmt>(S))
				{
					iReturnCnt++;
					if( iReturnCnt > iMaxRetCnt )
					{
						addViolation(S, this, description);
					}
				}
				// Puis on appelle le traitement pour le code imbriques
				if (SubStmt)
				{
					searchRetStmts(SubStmt, iReturnCnt);
				}
			}
		}

	public:
		virtual const string name() const override
		{
			return "Nbre sorties par fonction";
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
			iMaxRetCnt = RuleConfiguration::intForKey("MAX_SORTIES_PAR_FONCTION", MAX_RETCNT);
		}
		virtual void tearDown() override {}

		/* Visit FunctionDecl */
		bool VisitFunctionDecl(FunctionDecl *decl)
		{
			unsigned int iRetCnt = 0;
			if(decl->hasBody())
			{
				searchRetStmts(decl->getBody(), iRetCnt );
			}
			return true;
		}

};

static RuleSet rules(new NbreSortiesParFonctionRule());
