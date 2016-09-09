#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"
#include <regex.h>

using namespace std;
using namespace oclint;

class BlocDoxygenFonctionRule : public AbstractSourceCodeReaderRule
{
private:
	string description = "La fonction n'a pas de bloc Doxygen complet.";

	// Valeur pour être sûr de provoquer la violation s'il n'y a pas de blocs avant la toute première fonction.
	//	Dû au nombre de lignes max accepté entre le bloc et la fonction
	int endDoxygenBlock = -3;

	/* recherche de bloc doxygen */
	bool doxygenBlockOpened = 0;
	bool doxygenBlockClosed = 0;
	bool hasFcn = 0;
	bool hasBrief = 0;
	bool hasParam = 0;
	bool hasReturn = 0;

	void vResetTemoins()
	{
		doxygenBlockOpened = 0;
		doxygenBlockClosed = 0;
		hasFcn = 0;
		hasBrief = 0;
		hasParam = 0;
		hasReturn = 0;

	}

	bool hasFunctionDefinition(string line, int lineNumber)
	{
		// expression régulière recherchant les déclarations de fonction
		const char *pattern = "\\w+\\**\\s\\w+\\(\\s*(\\w+\\**\\s?\\**\\w*,*\\s*)*\\)+";
		int err, match;
		regex_t preg;
		short sPosition;

		// Exclusion des fonction définies par des macros préprocesseur
		if (line.find("#define") != string::npos)
		{
			return false;
		}

		// Exclusion des prototypes (le regex n'a pas l'air de le supporter, on le fait donc ici)
		sPosition = line.find(";");
		if( sPosition != string::npos )
		{
			if( sPosition == line.size() || sPosition == line.size() - 1 )
			{
				return false;
			}
		}

		// test de la validité du pattern
		err = regcomp (&preg, pattern, REG_NOSUB | REG_EXTENDED);
		if (err == 0)
		{
			// matching
			// match = 0 si match ok, REG_NOMATCH sinon, et ni l'un ni l'autre en cas d'erreur
			match = regexec (&preg, line.c_str(), 0, NULL, 0);

			regfree (&preg); 	// libération de la mémoire allouée pour la compilation de l'expression régulière

			// s'il n'y a pas de définition de fonction à cette ligne, l'indiquer
			if (match == REG_NOMATCH)
			{
				return false;
			}
			// si la ligne comporte bien une déclaration de fonction, l'indiquer aussi
			else if (match == 0)
			{
				return true;
			}
			// si erreur
			else
			{
				addViolation(lineNumber, 0, lineNumber, 0, this, line + "\nL'expression régulière n'a pas pu être comparée");
		         char *text;
		         size_t size;

		         size = regerror (err, &preg, NULL, 0);
		         text = (char*)malloc (sizeof (char) * size);
		         if (text)
		         {
		            regerror (err, &preg, text, size);
		            addViolation(lineNumber, 0, lineNumber, 0, this, text);

		            free (text);
		         }
		         else
		         {
		        	 addViolation(lineNumber, 0, lineNumber, 0, this, "Mémoire insuffisante");
		         }
		         return false;
			}	// fin du cas d'erreur

		}

		return false;
	}

	// Fonction appelée quand un bloc doxygen vient d'être fermé.
	// Elle indique si tous les paramètres d'un bloc Doxygen de fonction sont présents
	bool isFullDoxygenBlock()
	{
		// Calcul de la complétion du bloc courant
		bool isBlockFull = doxygenBlockOpened && doxygenBlockClosed && hasFcn && hasBrief&& hasParam && hasReturn;

		// Reset des indicateurs de présence
		vResetTemoins();

		return isBlockFull;
	}

	// Fonction de recherche d'un bloc Doxygen complet
	bool hasFullDoxygenBlock(string line)
	{
		// On recherche l'ouverture d'un bloc doxygen
		if (line.find("/*!") != string::npos ||
			line.find("/**") != string::npos)
		{
			// Ouverture d'un nouveau bloc, on remet à zéro les indicateurs de présence des différentes balises
			vResetTemoins();

			// Et on indique l'ouverture que l'on vient de trouver
			doxygenBlockOpened = 1;
		}

		// si un bloc est ouvert, on cherche les balises \fcn, \brief, \param, et \return ou \retval sur les lignes suivantes
		if (doxygenBlockOpened)
		{
			if (line.find("\\fcn") != string::npos)
			{
				hasFcn = 1;
			}

			if (line.find("\\brief") != string::npos)
			{
				hasBrief = 1;
			}

			if (line.find("\\param") != string::npos)
			{
				hasParam = 1;
			}

			if (line.find("\\return") != string::npos ||
			    line.find("\\retval") != string::npos )
			{
				hasReturn = 1;
			}
		}

		// detection d'une fin de bloc doxygen
		if (line.find("*/") != string::npos)
		{
			if (doxygenBlockOpened)
			{
				doxygenBlockClosed = 1;
			}
		}

		// Si un bloc doxygen est ferme ...
		if (doxygenBlockOpened && doxygenBlockClosed)
		{
			// ... on verifie qu'il etait complet
			if(isFullDoxygenBlock())
			{
				return true;
			}
		}
		return false;
	}

public:
    virtual const string name() const override
    {
        return "bloc doxygen fonction";
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
    	if (hasFullDoxygenBlock(line))
    	{
			endDoxygenBlock = lineNumber;
    	}

    	// On vérifie que la déclaration de fonction est bien placée juste après la
    	// fin du bloc Doxygen (0 ou 1 ligne d'écart)
    	if (hasFunctionDefinition(line, lineNumber))
    	{
			if ((lineNumber - endDoxygenBlock) > 2)
			{
    			addViolation(lineNumber, 0, lineNumber, 0, this, description );
			}
    	}
    }

    virtual void tearDown() override
    {

    }
};

static RuleSet rules(new BlocDoxygenFonctionRule());
