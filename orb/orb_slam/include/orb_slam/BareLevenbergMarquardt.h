#ifndef ORB_SLAM_LEVENBERG_MARQUARDT_H
#define ORB_SLAM_LEVENBERG_MARQUARDT_H

#include "Thirdparty/g2o/g2o/core/optimization_algorithm_with_hessian.h"

namespace ORB_SLAM
{

class BareLevenbergMarquardt : public g2o::OptimizationAlgorithmWithHessian
{
public:
    explicit BareLevenbergMarquardt(g2o::Solver* solver);
    

    virtual ~BareLevenbergMarquart();

    virtual SolverResult solve(int iteration, bool online = false);

protected:
    double _lambda;
};

}

#endif