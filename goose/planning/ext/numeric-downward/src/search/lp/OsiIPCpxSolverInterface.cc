#include "OsiIPCpxSolverInterface.h"
#ifdef USE_LP

using namespace std;

OsiIPCpxSolverInterface::OsiIPCpxSolverInterface(){
    IloEnv env;
    try{
        model = IloModel(env);
        var = IloNumVarArray(env);
        constraints = IloRangeArray(env);
    }catch (IloException& ex) {
        env.out() << "Error: " << ex << std::endl;
    }
}

void OsiIPCpxSolverInterface::loadProblem(const CoinPackedMatrix& matrix,
                                          const double* collb, const double* colub,
                                          const double* obj,
                                          const double* rowlb, const double* rowub){

    int nc=matrix.getNumCols();
    int nr=matrix.getNumRows();
    IloEnv env = model.getEnv();

    IloExpr objective(env);

    for (int ic = 0; ic < nc; ++ic){
        var.add(IloNumVar(env,collb[ic],colub[ic]));
        objective += obj[ic]*var[ic];
    }
    
    for (int ir = 0; ir < nr; ++ir){
        IloRange constraint(env,rowlb[ir],rowub[ir]);
        IloExprArray expression(model.getEnv());
        for (int ic = 0; ic < nc; ++ic){
            //cout << ir << " " << ic << " " << matrix.getCoefficient(ir,ic) << endl;
            double c = matrix.getCoefficient(ir,ic);
            constraint.setLinearCoef(var[ic],c);
        }
        constraints.add(constraint);
    }
    
    model.add(constraints);
    model.add(IloMinimize(env,objective));
    cplex = IloCplex(model);
    cplex.setOut(env.getNullStream());
    //cplex.setError(env.getNullStream());
    cplex.setWarning(env.getNullStream());
    cplex.setParam(IloCplex::Threads, 1);
    cplex.setParam(IloCplex::NumericalEmphasis, true);
}

void OsiIPCpxSolverInterface::setRowLower( int elementIndex, double elementValue ){

    if (constraints[elementIndex].getUB() <= elementValue)
        constraints[elementIndex].setUB(elementValue);
    constraints[elementIndex].setLB(elementValue);
    
}

void OsiIPCpxSolverInterface::setRowUpper( int elementIndex, double elementValue ){
         constraints[elementIndex].setUB(elementValue);
}

void OsiIPCpxSolverInterface::setColLower( int elementIndex, double elementValue ){
        var[elementIndex].setLB(elementValue);
}

void OsiIPCpxSolverInterface::setColUpper( int elementIndex, double elementValue ){
        var[elementIndex].setUB(elementValue);
}

void OsiIPCpxSolverInterface::setObjCoeff( int elementIndex, double elementValue ){
    // TODO fill
}

void OsiIPCpxSolverInterface::writeLp(const char *filename,
                     const char *extension,
                     double epsilon,
                     int numberAcross,
                     int decimals,
                     double objSense,
                     bool useRowNames) const{
    try {
        std::string f(filename);
        std::string e(extension);
        std::string fullname;
        if (e!="") {
            fullname = f + "." + e;
        } else {
            // no extension so no trailing period
            fullname = f;
        }

        cplex.exportModel(fullname.c_str());
    } catch (IloException& e) {
        cout << "Error in writing " << filename << " : " << e << endl;
    }
}

void OsiIPCpxSolverInterface::branchAndBound() {
    try {
        cplex.solve();
    } catch (IloException& e) {
        cout << "Error in solving : " << e << endl;
    }
}

double OsiIPCpxSolverInterface::getObjValue() const {
    return cplex.getObjValue();
}

void OsiIPCpxSolverInterface::setInteger(int index){
    IloEnv env = model.getEnv();
    model.add(IloConversion(env, var[index], ILOINT));
}

void OsiIPCpxSolverInterface::setColName(int colIndex, std::string  name){
    var[colIndex].setName(name.c_str());
}

#endif