#include "oclint/AbstractASTMatcherRule.h"
#include "oclint/RuleSet.h"
#include "oclint/RuleConfiguration.h"

using namespace std;
using namespace clang;
using namespace clang::ast_matchers;
using namespace oclint;

// Max instruction per condition (default)
#define MAX_COND 4

class InstructionParConditionRule : public AbstractASTMatcherRule
{
	private:
		string description = "L'instruction comporte trop de conditions";
		int iMaxInsCond = MAX_COND;
	public:
		virtual const string name() const override
		{
			return "instruction par condition";
		}

		virtual int priority() const override
		{
			return 1;
		}

		virtual const string category() const override
		{
			return "convention";
		}

		virtual void callback(const MatchFinder::MatchResult &result) override
		{
			const IfStmt *ifStmt = result.Nodes.getNodeAs<IfStmt>("ifStmt");
			if (ifStmt)
			{
				addViolation(ifStmt->getCond(), this, description);
			}

			const WhileStmt *whileStmt = result.Nodes.getNodeAs<WhileStmt>("whileStmt");
			if (whileStmt)
			{
				addViolation(whileStmt->getCond(), this, description);
			}

			const DoStmt *doStmt = result.Nodes.getNodeAs<DoStmt>("doStmt");
			if (doStmt)
			{
				addViolation(doStmt->getCond(), this, description);
			}

			const ConditionalOperator *conditionalOperator =
				result.Nodes.getNodeAs<ConditionalOperator>("conditionalOperator");
			if (conditionalOperator)
			{
				addViolation(conditionalOperator->getCond(), this, description);
			}

		}

		virtual void setUpMatcher() override
		{
			iMaxInsCond = RuleConfiguration::intForKey("MAX_INSTR_PAR_CONDITION", MAX_COND);
			StatementMatcher biOpMatcher = binaryOperator(
					anyOf(hasOperatorName("&&"), hasOperatorName("||")));
			int i = 0; 
			for( i = 0 ; i < iMaxInsCond ; i++)
			{
				biOpMatcher = hasDescendant(biOpMatcher) ;
			}
			addMatcher ( ifStmt( hasCondition( biOpMatcher ) ).bind("ifStmt") );
			addMatcher ( whileStmt( hasCondition( biOpMatcher ) ).bind("whileStmt") );
			addMatcher ( doStmt( hasCondition( biOpMatcher ) ).bind("doStmt") );
			addMatcher ( conditionalOperator( hasCondition( biOpMatcher ) ).bind("conditionalOperator") );
		}

};

static RuleSet rules(new InstructionParConditionRule());
