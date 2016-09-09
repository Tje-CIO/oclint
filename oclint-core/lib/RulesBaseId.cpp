/**
  \brief Rule base ID management, implementation file
  The implementation is based on the STL library
 */


#include "oclint/RulesBaseId.h"

typedef MapBaseId::iterator MapBaseIdIt;

// Static initialization of the map, easy to read using C++11 initialization
/*
	Give an ID for all Rules
*/
MapBaseId RulesBaseId::m_eBaseId =
{
	
	{"bitwise operator in conditional", 1},
	{"broken null check", 2},
	{"broken oddness check", 3},
	{"collapsible if statements", 4},
	{"constant conditional operator", 5},
	{"constant if expression", 6},
	{"dead code", 7},
	{"double negative", 8},
	{"goto statement", 9},
	{"jumbled incrementer", 10},
	{"misplaced null check", 11},
	{"multiple unary operator", 12},
	{"avoid branching statement as last in loop", 13},
	{"switch statements don't need default when fully covered", 14 },
	{"default label not last in switch statement", 15},
	{"missing break in switch statement", 16},
	{"inverted logic", 17},
	{"instruction par condition", 18},
	{"non case label in switch statement", 19},
	{"parameter reassignment", 20},
	{"switch statements should have default", 21},
	{"too few branches in switch statement", 22},
	{"pas de sortie prematuree",23 },
	{"reutilisation vocabulaire", 24},
	{"empty do/while statement",25}, 
	{"empty else block",26}, 
	{"empty for statement", 27},
	{"empty if statement",28}, 
	{"empty switch statement", 29},
	{"empty while statement", 30}, 
	{"long variable name", 31},
	{"short variable name", 32},
	{"redundant conditional operator", 33},
	{"redundant local variable", 34},
	{"unnecessary else statement", 35},
	{"useless parentheses",36}, 
	{"high cyclomatic complexity", 37},
	{"long line",38}, 
	{"long method",39}, 
	{"frequence commentaires", 40},
	{"trop d'appels fonction",41}, 
	{"high ncss method",42}, 
	{"deep nested block",43}, 
	{"high npath complexity",44}, 
	{"too many parameters", 45},
	{"unused local variable",46}, 
	{"unused method parameter", 47},
	{"nommage fichier",48}, 
	{"nommage fonction", 49}, 
	{"nommage macro constante",50}, 
	{"nommage macro fonction",51}, 
	{"nommage variable locale fonction", 52}, 
	{"nommage variable constante", 53},
	{"nommage variable globale", 54},
	{"nommage variable locale fichier", 55},
	{"nommage parametre", 56},
	{"types variable", 57},
	{"nombres magiques", 58},
	{"longueur fichier", 59},
	{"fonction par fichier", 60},
	{"bloc doxygen fichier", 61},
	{"une instruction par ligne", 62},
	{"une declaration par ligne", 63},
	{"definition structure", 64},
	{"presence definitions pre processeur", 65}
};

RulesBaseId & RulesBaseId::getInstance(void)
{
	// Instanciate member in the accesso to garantee correct initialization
	static RulesBaseId myBase;
	return myBase;
}


int RulesBaseId::getIdForRule( const std::string & szName)
{
	// DEBUG
	int id = -1;
	MapBaseIdIt it = m_eBaseId.find(szName);
	if( it != m_eBaseId.end() )
	{
		id = it->second;
	}

	return id;
}

std::ostream & operator<<(std::ostream &a, const BaseIdPair &b)
{
	a << "Clef : '" << b.first << "' Valeur : " << b.second << "\n";
	return a;
}

void RulesBaseId::printContent( void )
{
	MapBaseIdIt it;
	for(it = m_eBaseId.begin() ; it != m_eBaseId.end() ; ++it)
	{
		std::cout << *it;
	}
} 

