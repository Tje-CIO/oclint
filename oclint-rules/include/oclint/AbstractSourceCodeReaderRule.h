#ifndef OCLINT_ABSTRACTSOURCECODEREADERRULE_H
#define OCLINT_ABSTRACTSOURCECODEREADERRULE_H

#include "oclint/RuleBase.h"

namespace oclint
{

class AbstractSourceCodeReaderRule : public RuleBase
{
protected:
    virtual void apply() override;

    void addViolation(int startLine, int startColumn,
        int endLine, int endColumn, RuleBase *rule, const std::string& message = "");

public:
    virtual ~AbstractSourceCodeReaderRule();
    virtual void setUp() = 0;
    virtual void eachLine(int lineNumber, std::string line) = 0;
    virtual void tearDown() = 0;
};

} // end namespace oclint

#endif
