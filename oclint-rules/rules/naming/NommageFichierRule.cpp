#include "oclint/AbstractSourceCodeReaderRule.h"
#include "oclint/RuleSet.h"
#include <regex.h>
#include <libgen.h>
#include <string.h>

using namespace std;
using namespace oclint;

//! Symbole used for directory path
#define C_DIR_SEPARATOR '/'

//! Nom du fichier main pour laquelle on force le contrôle positif
#define C_SZ_MAIN_FILENAME "main.c"

class NommageFichierRule : public AbstractSourceCodeReaderRule
{
	private:
		string description = ": le fichier ne respecte pas la norme de nommage.";

		string getCurrentFileName(const string & filePath)
		{
			// Récupération du nom du fichier
			// on utilise strdup car basename peut modifier son argument
			char * pcFilePath = strdup( filePath.c_str() );
			string fileName( basename( pcFilePath ) );
			free (pcFilePath);
			return fileName;
		}

		/* Récupération du nom du dossier courant */
		string extractLastFolder(const string & filePath)
		{
			int sep;
			string lastFolder;
			// on utilise strdup car basename peut modifier son argument
			char * pcFilePath = strdup( filePath.c_str() );

			// Extraction du chemin (dossiers seulement)
			string fileDir = dirname(pcFilePath);

			// Extraction du dernier dossier de l'arborescence
			sep = fileDir.rfind( C_DIR_SEPARATOR );
			lastFolder = fileDir.substr(sep+1, fileDir.size());
			free (pcFilePath);
			return lastFolder;
		}
		
		// Fonction exécutée pour le contrôle
		bool checkFileName(const string & filePath)
		{

			string fileName = getCurrentFileName( filePath );
			string currentDir = extractLastFolder( filePath );
			bool bOK = false ;

			// Cas d'execption au test :
			if( fileName.compare( C_SZ_MAIN_FILENAME ) == 0 )
			{
				return true;
			}

			// Le nom doit être préfixé du répertoire courant
			string s_pattern = "^" + currentDir;

			int err, match;
			regex_t preg;

			// test de la validité du pattern
			err = regcomp (&preg, s_pattern.c_str(), REG_NOSUB | REG_EXTENDED);
			if (err == 0)
			{

				// matching
				// match = 0 si match ok, REG_NOMATCH sinon, et ni l'un ni l'autre en cas d'erreur
				match = regexec (&preg, fileName.c_str(), 0, NULL, 0);
				regfree (&preg); 

				// si le nom du fichier n'est pas conforme
				if (match == REG_NOMATCH)
				{
					bOK = false ;
				}
				// Si le fichier est bien nommé
				else if (match == 0)
				{
					bOK = true;
				}
				// si erreur
				else
				{
					addViolation(0, 0, 0, 0, this, "L'expression reguliere n'a pas pu etre comparee");
					char *text;
					size_t size;

					size = regerror (err, &preg, NULL, 0);
					text = (char*)malloc (sizeof (char) * size);
					if (text)
					{
						regerror (err, &preg, text, size);
						addViolation(0, 0, 0, 0, this, text);
						free (text);
					}
					else
					{
						addViolation(0, 0, 0, 0, this, "L'expression reguliere n'a pas pu etre comparee, probleme de memoire");
					}
				}	// fin du cas d'erreur

			}
			return bOK;
		}

	public:
		virtual const string name() const override
		{
			return "nommage fichier";
		}

		virtual int priority() const override
		{
			return 1;
		}

		virtual const string category() const override
		{
			return "naming";
		}

		virtual void setUp() override
		{
			bool isCorrect = false;
			string filePath;

			filePath = _carrier->getMainFilePath();
			isCorrect = checkFileName(filePath);
			if (isCorrect == false)
			{
				addViolation(0, 0, 0, 0, this, getCurrentFileName(filePath) + description);
			}
		}

		virtual void eachLine(int lineNumber, string line) override
		{
		}

		virtual void tearDown() override
		{

		}
};

static RuleSet rules(new NommageFichierRule());
