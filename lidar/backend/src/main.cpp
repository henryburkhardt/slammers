#include "httplib.h"
#include <iostream>
#include <sstream>
#include <memory>

#include <g2o/core/sparse_optimizer.h>
#include <g2o/core/block_solver.h>
#include <g2o/core/optimization_algorithm_gauss_newton.h>
#include <g2o/solvers/dense/linear_solver_dense.h>
#include <g2o/types/slam2d/types_slam2d.h>


// intialize the optimimzer
std::unique_ptr<g2o::SparseOptimizer> setupOptimizer() {
    auto optimizer = std::make_unique<g2o::SparseOptimizer>();
    optimizer->setVerbose(true);

    using BlockSolver = g2o::BlockSolver<g2o::BlockSolverTraits<-1, -1>>;
    using LinearSolver = g2o::LinearSolverDense<BlockSolver::PoseMatrixType>;

    auto solver = std::make_unique<BlockSolver>(
        std::make_unique<LinearSolver>());

    optimizer->setAlgorithm(
        new g2o::OptimizationAlgorithmGaussNewton(std::move(solver)));

    return optimizer;
}


// http handler
void handleOptimize(const httplib::Request& req, httplib::Response& res) {
    // req.body contains the g2o file content
    std::stringstream inputStream(req.body);
    std::stringstream outputStream;

    auto optimizer = setupOptimizer();

    // Load from stringstream
    if (!optimizer->load(inputStream)) {
        res.status = 400;
        res.set_content("Failed to load g2o content", "text/plain");
        return;
    }
    
    optimizer->initializeOptimization();
    optimizer->optimize(20);

    // Save to stringstream
    optimizer->save(outputStream);

    // Return optimized content
    res.status = 200;
    res.set_content(outputStream.str(), "text/plain");
}


// main server
int main() {
    httplib::Server svr;

    svr.Post("/optimize", handleOptimize);

    std::cout << "Listening on port 8080..." << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
