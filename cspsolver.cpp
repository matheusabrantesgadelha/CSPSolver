#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

template <typename TCSPProblem>
class CSPSolver
{
	public:
		CSPSolver( TCSPProblem p ): mProblem( p ), k(0){}

		typename TCSPProblem::AssignmentType backtrackSearch()
		{
//			mProblem.print();
//			std::cout << "---" << std::endl;
			typename TCSPProblem::VariableType& var = mProblem.selectVariable();
			if( var.null ) return mProblem.mVars;
			else
			{
				k += var.currentDomain.size();
				for( size_t i = 0; i<var.currentDomain.size(); ++i )
				{
					var.value = var.currentDomain[i];
					if( mProblem.checkConstraints() )
					{
						typename TCSPProblem::AssignmentType sol = backtrackSearch();
						if( sol.size() != mProblem.failure.size() ) return sol;
					}
					var.value = TCSPProblem::VariableType::unassigned;
				}
			}
			return mProblem.failure;
		}
		unsigned int k;
	
		TCSPProblem mProblem;
};

class Sudoku
{
	public:

		typedef int DomainType;
		struct VariableType
		{
			VariableType( std::vector<DomainType> c, std::vector<DomainType> o)
				: currentDomain(c), orgDomain(o)
			{
				value = unassigned;
				null = false;
			}

			bool isUnassigned(){ return value == VariableType::unassigned; }
			std::vector<DomainType> currentDomain;
			std::vector<DomainType> orgDomain;
			DomainType value;
			bool null;
			static const DomainType unassigned = -1;
		};

		typedef std::vector<std::vector<VariableType> > AssignmentType;

		AssignmentType mVars;
		AssignmentType failure;

		Sudoku( const char* filename ) : nullFlag({},{})
		{
			nullFlag.null = true;

			std::ifstream file( filename );
			std::vector<DomainType> domain = {1,2,3,4,5,6,7,8,9};
			mVars = std::vector<std::vector<VariableType> >
				(9, std::vector<VariableType>(9, VariableType( domain, domain )));
			char cval;
			int val;

			for( int i=0; i<9; ++i )
			{
				for( int j=0; j<9; ++j )
				{
					file >> cval;
					val = cval - '0';
					if( val > 0 )
					{
						mVars[i][j] = VariableType({val},{val});
						mVars[i][j].value = val;
					}
					else
					{
						mVars[i][j].value = VariableType::unassigned;
					}
				}
			}

		}

		void print()
		{
			for( int i=0; i<mVars.size(); ++i )
			{
				for( int j=0; j<mVars.size(); ++j )
				{
					if( mVars[i][j].isUnassigned() ) std::cout << "-";
					else std::cout << mVars[i][j].value;
					std::cout << " ";
				}
				std::cout << std::endl;
			}
		}

		VariableType& selectVariable()
		{
			for(size_t i=0; i<mVars.size(); ++i )
			{
				for( size_t j=0; j<mVars[i].size(); ++j )
				{
					if(mVars[i][j].isUnassigned())
					{
						return mVars[i][j];
					}
				}
			}
			
			return nullFlag;
		}

		bool checkConstraints()
		{
			std::vector<int> values (9, 0);
			for( size_t i=0; i<mVars.size(); ++i )
			{
				for( size_t j=0; j<mVars.size(); ++j )
				{
					if( mVars[i][j].isUnassigned() ) continue;
					if( values[mVars[i][j].value-1] != 0 )
					{
//						std::cout << "LINE CHECKING FAILED " << i << ", " << j << std::endl;
						return false;
					}
//					std::cout << mVars[i][j].value << std::endl;
					values[mVars[i][j].value-1]++;
				}

				for( size_t j=0; j<values.size(); ++j )
					values[j] = 0;
			}

			for( size_t i=0; i<mVars.size(); ++i )
			{
				for( size_t j=0; j<mVars.size(); ++j )
				{
					if( mVars[j][i].isUnassigned() ) continue;
//					std::cout << mVars[j][i].value << std::endl;
					if( values[mVars[j][i].value-1] != 0 )
					{
//						std::cout << "COL CHECKING FAILED " << i << ", " << j << std::endl;
						return false;
					}
					values[mVars[j][i].value-1]++;
				}

				for( size_t j=0; j<values.size(); ++j )
					values[j] = 0;
			}
			
			for( int i=0; i<3; ++i )
			{
				for( int j=0; j<3; ++j )
				{
					if(!checkBlock(i,j)) return false;
				}
			}

			return true;
		}
		
	private:
		bool checkBlock(int p, int q)
		{
			std::vector<int> values (9, 0);
			for( int i=0; i<3; ++i )
			{
				for( int j=0; j<3; ++j )
				{
					if( mVars[i+p*3][j+q*3].isUnassigned() ) continue;
//					std::cout << mVars[i+p*3][j+q*3].value << std::endl;
					if( values[mVars[i+p*3][j+q*3].value-1] != 0 )
					{
//						std::cout << "BLOCK CHECKING FAILED " << p << ", " << q << std::endl;
						return false;
					}
					values[mVars[i+p*3][j+q*3].value-1]++;
				}
			}
			return true;
		}
		VariableType nullFlag;
};

int main( int argc, char* argv[] )
{
	Sudoku sudoku( argv[1] );
	CSPSolver<Sudoku> solver( sudoku );
//	std::cout << sudoku.checkConstraints();
	solver.backtrackSearch();
	solver.mProblem.print();

	std::cout << "Number of guesses: " << solver.k << std::endl;

	return 0;
}
