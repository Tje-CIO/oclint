#include "oclint/AbstractASTVisitorRule.h"
#include "oclint/RuleSet.h"
#include "oclint/util/StdUtil.h"
#include "oclint/RuleConfiguration.h"


using namespace std;
using namespace clang;
using namespace oclint;

#define MAX_CALLCNT 10

class TooManyProcedureCallsRule : public AbstractASTVisitorRule<TooManyProcedureCallsRule>
{
	private:
		string description = "Fonction trop longue dépassant la limite du nombres d'appels de fonction: ";
		int functionLength;
		bool isInFunction;
		std::vector<int> callExprLines;

		// Seuil maximal
		int threshold = MAX_CALLCNT;

		int getLine(Stmt* node)
		{
			int line;
			clang::SourceLocation fileLoc;
			clang::SourceManager *sourceManager = &_carrier->getSourceManager();

			fileLoc = node->getLocStart();
			line = sourceManager->getPresumedLineNumber(fileLoc);

			return line;
		}

		bool hasAlreadyBeenCounted (int line,Stmt* node)
		{
			int index = 0;
			bool hasAlreadyBeenCountedValue;

			// On sort avant de checker callExprLines.at(0) à la première itération
			while(index < callExprLines.size() && callExprLines.at(index) != line)
			{
				index ++;
			}

			// Si la ligne n'était pas dans la liste
			if(index == callExprLines.size())
			{
				hasAlreadyBeenCountedValue = 0;
			}
			else
			{
				hasAlreadyBeenCountedValue = 1;
			}

			return hasAlreadyBeenCountedValue;
		}


		// Fonction ajoutant une ligne donnée à la liste des lignes comptabilisées
		int addLineToDB(int line)
		{
			int err = 0;

			try
			{
				callExprLines.push_back(line);
			}
			catch(const std::bad_alloc &) {
				// Récupération de l'erreur la plus courante
				err=1;
			}
			catch(const std::exception &) {
				// Si une autre erreur identifiée se produit
				err=2;
			}
			catch(...) {
				// Cas d'une erreur mal identifiée
				err=3;
			}

			return err;

		}

		// Désallocation totale du Vecteur
		void cleanVector()
		{
			// swap avec un vecteur vide. On ne peut faire vraiment mieux avant le C++ 11...
			std::vector<int>().swap(callExprLines);
		}

	public:
		virtual const string name() const override
		{
			return "trop d'appels fonction";
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
			threshold = RuleConfiguration::intForKey("MAX_NBRES_PROCEDURES", MAX_CALLCNT );
		}
		virtual void tearDown() override
		{
			cleanVector();
		}

		/* Visit FunctionDecl */
		bool VisitFunctionDecl(FunctionDecl *node)
		{

			isInFunction = 1;
			functionLength = 0;
			string funcName = node->getNameAsString();

			// Cast dynamique pour récupérer le corps de la fonction et s'assurer du bon format. Renvoie nullptr en cas d'erreur.
			CompoundStmt *CS = dyn_cast_or_null<CompoundStmt>(node->getBody());
			if (CS != nullptr)
			{
				// Parcours de tous les CompoundStmt enfants
				TraverseCompoundStmt(CS);
				if (functionLength > threshold)
				{
					addViolation(node, this, description + funcName + " avec " + toString<int>(functionLength) +  " appels de fonction.");
				}

			}
			isInFunction = 0;

			return true;
		}

		/* Visit CallExpr */
		bool VisitCallExpr(CallExpr *node)
		{
			int err;
			int line = getLine(node);

			if(isInFunction == 1)
			{
				/* On doit empêcher la ligne courante d'être comptée deux fois. Bug se produisant quand
				 * une fonction est appelée au début d'un CompoundStmt
				 */
				if(!hasAlreadyBeenCounted(line, node))
				{
					functionLength ++;
					err = addLineToDB(line);
					if(err)
					{
						addViolation(node, this, "Problème de gestion des appels de fonction à la ligne: " + toString<int>(line));
					}
				}
			}
			return true;
		}

		/* Visit CompoundStmt */
		bool VisitCompoundStmt(CompoundStmt *node)
		{
			if(isInFunction == 1)
			{
				// Cast dynamique pour récupérer le corps de la fonction et s'assurer du bon format. Renvoie nullptr en cas d'erreur.
				CallExpr *CE = dyn_cast_or_null<CallExpr>(node->body_front());
				if (CE != nullptr)
				{
					// Parcours de tous les CallExpr enfants
					TraverseCallExpr(CE);
				}
			}
			return true;
		}

};

static RuleSet rules(new TooManyProcedureCallsRule());
