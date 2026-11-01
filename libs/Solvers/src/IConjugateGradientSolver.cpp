#include "HeatTransfer/Solvers/IConjugateGradientSolver.h"

#include <tracy/Tracy.hpp>

namespace HeatTransfer::Solvers
{
    IConjugateGradientSolver::IConjugateGradientSolver(
        const Core::SimulationParameters& parameters,
        const Core::MaterialProperties& materialProperties,
        const Core::BoundaryConditions& boundaryCondition)
        : IMultigridSolver(parameters, materialProperties, boundaryCondition)
    {
    }

    void IConjugateGradientSolver::InitialiseConjugateGradientVariables()
    {
        // We need to compute 4 textbook variables: grad f_0, d_0, s_0, and grad f_0^T * d_0

        // Say we have H * x = b, where H is the Hessian of f(x) = 0.5 * x^T H x - b^T x
        // The residual is given by r = b - H * x
        // The first gradient is given grad f_0 = H * x_0 - b
        // Thus grad f_0 = -r_0, and the first gradient is the negative of the residual
        _gradient = -ComputeResidual(*_temperatureMatrix, _residualStack[0], _insulatedStack[0]);

        // d_0 = B * grad f_0
        PreconditionGradient();

        // s_0 = -d_0
        _searchDirection = -_preconditionedGradient;

        // grad f_0^T * d_0
        _gradientDotPreconditioned = Dot(_gradient, _preconditionedGradient);
    }

    void IConjugateGradientSolver::Iterate()
    {
        ZoneScoped;

        const Eigen::MatrixXd Hs = ApplyHessian(_searchDirection);
        const double sHs = Dot(_searchDirection, Hs);

        // H is symmetric positive definite, so s^T * H * s > 0 for any non-zero vector s
        // If sHs == 0, then the search direction is zero, and we have converged to the solution
        if (sHs <= 0.0)
            return;

        // alpha_k = -grad f_k^T * s_k / (s_k^T * H * s_k)
        const double alpha = -Dot(_gradient, _searchDirection) / sHs;

        // x_k+1 = x_k + alpha_k * s_k
        *_temperatureMatrix += alpha * _searchDirection;

        // grad f_k+1 = grad f_k + alpha_k * H * s_k
        _gradient += alpha * Hs;

        // d_k+1 = B * grad f_k+1
        PreconditionGradient();

        // beta_k = grad f_k+1^T * d_k+1 / (grad f_k^T * d_k)
        const double gradientDotPreconditioned = Dot(_gradient, _preconditionedGradient);
        const double beta = gradientDotPreconditioned / _gradientDotPreconditioned;

        // s_k+1 = -d_k+1 + beta_k * s_k
        _searchDirection = -_preconditionedGradient + beta * _searchDirection;
        _gradientDotPreconditioned = gradientDotPreconditioned;
    }

    void IConjugateGradientSolver::PreconditionGradient()
    {
        // Say we have a linear system H * x = b, where H is the Hessian of f(x) = 0.5 * x^T H x -
        // b^T x. V-Cycle solves H * x = b, to get x = H^-1 * b.
        // Thus H * x = grad f becomes x = H^-1 * grad f, which is the preconditioned gradient d

        _preconditionedGradient = Eigen::MatrixXd::Zero(_gradient.rows(), _gradient.cols());
        VCycle(_preconditionedGradient, _gradient);
    }

    Eigen::MatrixXd IConjugateGradientSolver::ApplyHessian(const Eigen::MatrixXd& matrix)
    {
        // For a linear system H * x = b, compute residual returns r = b - H * x
        // Setting b = 0, gives r = - H * x or H * x = -r
        // We will mainly be applying the Hessian to the search direction s

        return -ComputeResidual(
            matrix, Eigen::MatrixXd::Zero(matrix.rows(), matrix.cols()), _insulatedStack[0]);
    }

    double IConjugateGradientSolver::Dot(const Eigen::MatrixXd& a, const Eigen::MatrixXd& b)
    {
        return (a.array() * b.array()).sum();
    }
}
