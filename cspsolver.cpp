#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

unsigned int difficulty = 0;

template <typename TCSPProblem>
class CSPSolver
{
	public:
		CSPSolver( TCSPProblem p ): mProblem( p ), 
			k(0), mSolution(p), mrv(true), waterfall(true)
		{

		}

		typename TCSPProblem::AssignmentType backtrackSearch( TCSPProblem p )
		{
			if(waterfall) p.waterfall();
//			mProblem.print();
//			std::cout << "---" << std::endl;
			typename TCSPProblem::VariableType& var = p.selectVariable( mrv );
			if( var.null ) return p.mVars;
			else
			{
				k += var.currentDomain.size();
				for( size_t i = 0; i<var.currentDomain.size(); ++i )
				{
					var.value = var.currentDomain[i];
					if( p.checkConstraints() )
					{
						typename TCSPProblem::AssignmentType sol = backtrackSearch( p );
						if( sol.size() != mProblem.failure.size() ) return sol;
					}
					var.value = TCSPProblem::VariableType::unassigned;
				}
			}
			return mProblem.failure;
		}

		TCSPProblem solve()
		{
			k=0;
			difficulty = 0;
			mSolution.mVars = mProblem.mVars;
			mSolution.mVars = backtrackSearch( mSolution );
			return mSolution;
		}

		unsigned int k;
		bool mrv;
		bool waterfall;
	
		TCSPProblem mProblem;
		TCSPProblem mSolution;
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

		VariableType& selectVariable( bool isMrv )
		{
			size_t fi, fj;
			unsigned int smallestDomain = 10;
			for(size_t i=0; i<mVars.size(); ++i )
			{
				for( size_t j=0; j<mVars[i].size(); ++j )
				{
					if(mVars[i][j].isUnassigned())
					{
						if( !isMrv ) return mVars[i][j];

						if( mVars[i][j].currentDomain.size() < 10 && 
								mVars[i][j].currentDomain.size() > 0 )
						{
							smallestDomain = mVars[i][j].currentDomain.size();
							fi = i;
							fj = j;
						}
					}
				}
			}

			if( smallestDomain < 10 && isMrv)
				return mVars[fi][fj];
			
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

		void inference()
		{
			std::vector<int> values(9,0);

			//Rows
			for( size_t i=0; i< mVars.size(); ++i )
			{
				for( size_t j=0; j<mVars.size(); ++j )
				{
					if(mVars[i][j].isUnassigned()) continue;
					values[mVars[i][j].value-1]++;
				}

				for( size_t j=0; j<mVars.size(); ++j )
				{
					for( size_t k=0; k<values.size(); ++k )
					{
						if( values[k] > 0 )
						{
							mVars[i][j].currentDomain.erase(
								std::remove( mVars[i][j].currentDomain.begin(), 
									mVars[i][j].currentDomain.end(), k+1), 
								mVars[i][j].currentDomain.end());
						}
					}
				}

				for( size_t j=0; j<values.size(); ++j )
					values[j] = 0;
			}

			//Cols
			for( size_t i=0; i< mVars.size(); ++i )
			{
				for( size_t j=0; j<mVars.size(); ++j )
				{
					if(mVars[j][i].isUnassigned()) continue;
					values[mVars[j][i].value-1]++;
				}

				for( size_t j=0; j<mVars.size(); ++j )
				{
					for( size_t k=0; k<values.size(); ++k )
					{
						if( values[k] > 0 )
						{
							mVars[j][i].currentDomain.erase(
								std::remove( mVars[j][i].currentDomain.begin(), 
									mVars[j][i].currentDomain.end(), k+1), 
								mVars[j][i].currentDomain.end());
						}
					}
				}

				for( size_t j=0; j<values.size(); ++j )
					values[j] = 0;
			}

			for( int i=0; i<3; ++i )
			{
				for( int j=0; j<3; ++j )
				{
					inferenceOnBlock(i,j);
					hiddenSingleBlock(i,j);
				}
			}

			pairInferenceRow();
			pairInferenceCol();
		}

		bool assignVariables()
		{
			bool assigned = false;
			for( size_t i=0; i<mVars.size(); ++i )
			{
				for( size_t j=0; j<mVars.size(); ++j )
				{
					if( mVars[i][j].isUnassigned() && 
						mVars[i][j].currentDomain.size()==1 )
					{
						assigned = true;
						mVars[i][j].value = mVars[i][j].currentDomain[0];
					}
				}
			}
			return assigned;
		}

		void waterfall()
		{
			int i=0;
			inference();
			while( assignVariables() )
				inference();
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

		void pairInferenceRow()
		{
			for( size_t i=0; i<mVars.size(); ++i )
			{
				for( size_t j=0; j<mVars.size(); ++j )
				{
					for( size_t k=j+1; k<mVars.size(); ++k )
					{
						if( mVars[i][j].currentDomain == mVars[i][k].currentDomain
								&& mVars[i][k].currentDomain.size() == 2 
								&& mVars[i][j].isUnassigned()
								&& mVars[i][k].isUnassigned() )
						{

							for( size_t m=0; m<mVars.size(); ++m )
							{
								if( m != k && m!= j && mVars[i][m].isUnassigned() )
								{
									mVars[i][m].currentDomain.erase(
										std::remove( mVars[i][m].currentDomain.begin(), 
											mVars[i][m].currentDomain.end(), mVars[i][j].currentDomain[0]), 
										mVars[i][m].currentDomain.end());

									mVars[i][m].currentDomain.erase(
										std::remove( mVars[i][m].currentDomain.begin(), 
											mVars[i][m].currentDomain.end(), mVars[i][j].currentDomain[1]), 
										mVars[i][m].currentDomain.end());

									difficulty++;
								}
							}
						}
					}
				}
			}
		}

		void pairInferenceCol()
		{
			for( size_t i=0; i<mVars.size(); ++i )
			{
				for( size_t j=0; j<mVars.size(); ++j )
				{
					for( size_t k=j+1; k<mVars.size(); ++k )
					{
						if( mVars[j][i].currentDomain == mVars[k][i].currentDomain
								&& mVars[k][i].currentDomain.size() == 2 
								&& mVars[j][i].isUnassigned()
								&& mVars[k][i].isUnassigned() )
						{

							for( size_t m=0; m<mVars.size(); ++m )
							{
								if( m != k && m!= j && mVars[m][i].isUnassigned() )
								{
									mVars[m][i].currentDomain.erase(
										std::remove( mVars[m][i].currentDomain.begin(), 
											mVars[m][i].currentDomain.end(), mVars[j][i].currentDomain[0]), 
										mVars[m][i].currentDomain.end());

									mVars[m][i].currentDomain.erase(
										std::remove( mVars[m][i].currentDomain.begin(), 
											mVars[m][i].currentDomain.end(), mVars[j][i].currentDomain[1]), 
										mVars[m][i].currentDomain.end());

									difficulty++;
								}
							}
						}
					}
				}
			}
		}

		void inferenceOnBlock( int p, int q )
		{
			std::vector<int> values (9, 0);
			for( int i=0; i<3; ++i )
			{
				for( int j=0; j<3; ++j )
				{
					if( mVars[i+p*3][j+q*3].isUnassigned() ) continue;
					values[mVars[i+p*3][j+q*3].value-1]++;
				}
			}
			for( int i=0; i<3; ++i )
			{
				for( int j=0; j<3; ++j )
				{
					for( size_t k=0; k<values.size(); ++k )
					{
						if( values[k] > 0 )
						{
							mVars[i+p*3][j+q*3].currentDomain.erase(
								std::remove( mVars[i+p*3][j+q*3].currentDomain.begin(), 
									mVars[i+p*3][j+q*3].currentDomain.end(), k+1), 
								mVars[i+p*3][j+q*3].currentDomain.end());

							difficulty++;
						}
					}
				}
			}
		}

		void hiddenSingleBlock( int p, int q )
		{
			std::vector<int> values(9, 0);
			for( int i=0; i<3; ++i )
			{
				for( int j=0; j<3; ++j )
				{
					if( mVars[i+p*3][j+q*3].isUnassigned() ) 
					{
						for( size_t k =0; k<mVars[i+p*3][j+q*3].currentDomain.size(); ++k )
						{
							values[mVars[i+p*3][j+q*3].currentDomain[k]-1]++;
						}
					}
				}
			}

			for( int v=0; v<values.size(); ++v )
			{
				if( values[v] == 1 )
				{
					for( int i=0; i<3; ++i )
					{
						for( int j=0; j<3; ++j )
						{
							if( std::find( mVars[i+p*3][j+q*3].currentDomain.begin(),
								mVars[i+p*3][j+q*3].currentDomain.end(), v+1) !=
								mVars[i+p*3][j+q*3].currentDomain.end() )
							{
								mVars[i+p*3][j+q*3].currentDomain.clear();
								mVars[i+p*3][j+q*3].currentDomain.push_back(v+1);

								difficulty++;
							}
						}
					}
				}
			}
		}
		VariableType nullFlag;
};

int main( int argc, char* argv[] )
{
	Sudoku sudoku( argv[1] );
	CSPSolver<Sudoku> solver( sudoku );
//	std::cout << sudoku.checkConstraints();
//	sudoku.mVars = solver.backtrackSearch( sudoku );

	std::cout << "============================" << std::endl;
	std::cout << argv[1] << std::endl;
	std::cout << "============================" << std::endl;

//	std::cout << "Using MRV w/ waterfall infereces" << std::endl;
//	solver.mrv = true;
//	solver.waterfall = true;
//	solver.solve().print();
//	std::cout << "Number of guesses: " << solver.k << std::endl;
//	std::cout << "---" << std::endl;
//
	std::cout << "Using MRV" << std::endl;
	solver.mrv = true;
	solver.waterfall = false;
	solver.solve().print();
	std::cout << "Number of guesses: " << solver.k << std::endl;
	std::cout << "---" << std::endl;

//	std::cout << "Without MRV" << std::endl;
//	solver.mrv = false;
//	solver.waterfall = false;
//	solver.solve().print();
//	std::cout << "Number of guesses: " << solver.k << std::endl;
//	std::cout << "---" << std::endl;

	std::cout << "Difficulty: " << difficulty << std::endl;

	return 0;
}
