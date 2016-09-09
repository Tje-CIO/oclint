#include "TestHeaders.h"
#include "rules/naming/ConflitNomVariableRule.cpp"

TEST(ConflitNomVariableRuleTest, PropertyTest)
{
    ConflitNomVariableRule rule;
    EXPECT_EQ(3, rule.priority());
    EXPECT_EQ("conflit nom variable", rule.name());
}

TEST(ConflitNomVariableRuleTest, FirstFailingTest)
{
    EXPECT_FALSE("Start writing a new test");
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
