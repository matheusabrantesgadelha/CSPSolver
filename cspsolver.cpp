#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

template <typename TCSPProblem>
class CSPSolver
{
};

class Sudoku
{
	typedef int DomainType;
	struct VariableType
	{
		VariableType( std::vector<DomainType> c, std::vector<DomainType> o)
			: currentDomain(c), orgDomain(o){}
		std::vector<DomainType> currentDomain;
		std::vector<DomainType> orgDomain;
	};

	std::vector<std::vector<VariableType> > mVars;

	public:

		Sudoku( const char* filename )
		{
			std::ifstream file( filename );
			std::vector<DomainType> domain = {1,2,3,4,5,6,7,8,9};
			mVars = std::vector<std::vector<VariableType> >
				(9, std::vector<VariableType>(9, VariableType( domain, domain )));
			char cval;
			int val;

			file >> val;
			std::cout << val << std::endl;
			for( int i=0; i<9; ++i )
			{
				for( int j=0; j<9; ++j )
				{
					file >> cval;
					val = cval - '0';
					if( val > 0 )
					{
						mVars[i][j] = VariableType({val},{val});
					}
				}
			}

//			for( int i=0; i<9; ++i )
//			{
//				for( int j=0; j<9; ++j )
//				{
//					for( size_t k = 0; k<mVars[i][j].currentDomain.size(); ++k )
//					{
//						std::cout << mVars[i][j].currentDomain[k] << " ";
//					}
//					std::cout << std::endl;
//				}
//			}
		}
};

int main( int argc, char* argv[] )
{
	Sudoku sudoku( argv[1] );
	return 0;
}
