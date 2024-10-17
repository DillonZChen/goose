#ifndef OsiIPCpxSolverInterface_h
#define OsiIPCpxSolverInterface_h

#ifdef USE_LP

#include "OsiSolverInterface.hpp"
#include "OsiCpxSolverInterface.hpp"
#include "CoinWarmStartBasis.hpp"
#include "OsiColCut.hpp"
#include "OsiRowCut.hpp"

#define IL_STD 1

#include <ilcplex/ilocplex.h>

class OsiIPCpxSolverInterface : virtual public OsiCpxSolverInterface {
    friend void OsiIPCpxSolverInterfaceUnitTest(const std::string & mpsDir, const std::string & netlibDir);
public:

    OsiIPCpxSolverInterface();
    
    ~OsiIPCpxSolverInterface(){
        
    }
    
    virtual void loadProblem(const CoinPackedMatrix& matrix,
                             const double* collb, const double* colub,
                             const double* obj,
                             const double* rowlb, const double* rowub);
    
    virtual void setRowLower( int elementIndex, double elementValue );
    virtual void setRowUpper( int elementIndex, double elementValue ) ;
    virtual void setColLower( int elementIndex, double elementValue );
    virtual void setColUpper( int elementIndex, double elementValue );
    virtual void writeLp(const char *filename,
                         const char *extension = ".lp",
                         double epsilon = 1e-5,
                         int numberAcross = 10,
                         int decimals = 5,
                         double objSense = 0.0,
                         bool useRowNames = true) const;
    
    virtual int getNumRows() const {
        return constraints.getSize();
    }
    
    virtual int getNumCols() const {
        return var.getSize();
    }
    
    virtual void setObjCoeff( int elementIndex, double elementValue );
    virtual void branchAndBound();
    virtual void solve() {branchAndBound(); };
    virtual void initialSolve() {branchAndBound(); };
    virtual void resolve() {branchAndBound(); };
    virtual bool isAbandoned() const{
        return false;
    }
    virtual bool isProvenOptimal() const{
        return true;
    }
    /// Is primal infeasiblity proven?
    virtual bool isProvenPrimalInfeasible() const{
        return false;
    };
    /// Is dual infeasiblity proven?
    virtual bool isProvenDualInfeasible() const{
        return false;
    }
    virtual double getObjValue() const;
    
    virtual void setInteger(int index);
    virtual void setColName(int colIndex, std::string  name) ;

private:
    IloCplex cplex;
    IloModel model;
    IloNumVarArray var;
    IloRangeArray constraints;
    IloObjective objective;
};

//#############################################################################
/** A function that tests the methods in the OsiCpxSolverInterface class. */
void OsiCpxSolverInterfaceUnitTest(const std::string & mpsDir, const std::string & netlibDir);

#endif

#endif /* OsiIPCpxSolverInterface_h */
