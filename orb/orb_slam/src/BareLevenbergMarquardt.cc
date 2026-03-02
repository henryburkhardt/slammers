#include "BareLevenbergMarquart.h"
#include "Thirdparty/g2o/g2o/core/solver.h"
#include "Thirdparty/g2o/g2o/core/sparse_optimizer.h"
#include "Thirdparty/g2o/g2o/core/optimization_algorithm_with_hessian.h"
#include "Thirdparty/g2o/g2o/core/optimizable_graph.h"

namespace ORB_SLAM
{
BareLevenbergMarquardt::BareLevenbergMarquardt(g2o::Solver* solver)
: g2o::OptimizationAlgorithmWithHessian(solver)
{
    _lambda = 1e-5;
}
BareLevenbergMarquardt::~BareLevenbergMarquardt() {}


g2o::OptimizationAlgorithm::SolverResult BareLevenbergMarquardt::solve(int iteration, bool online)
{
    //compute the errors between points to build matrix
    _optimizer->computeActiveErrors();

    //calculate sum of all errors
    double Chi = _optimizer->activeRobustChi2();

    // creates hessian matrix
    _solver->buildStructure();

    //computes/fills jacobian 
    _solver -> buildSystem();

    //add damping to matrix
    _solver->setLambda(_lambda, true);

    bool system = _solver->solve();

    if(!system){
        return Fail;
    }

    _optimizer->push();

    //update the matrix with our current lambda
    _optimizer->update(_solver->x());


    _optimizer->computeActiveErrors();

    //restore back to original matrix
    _solver -> restoreDiagonal();

    //calculate new sum of all errors with herber kernal
    double newChi = _optimizer->activeChi2();

    //did error decrease
    if(newChi < Chi){
        _lambda *= 0.1;
        _optimizer -> discardTop();
        return OK;
    }
    else{
        _lambda *= 10.0;
        _optimizer->pop();
        return Fail;
    }
}

}
