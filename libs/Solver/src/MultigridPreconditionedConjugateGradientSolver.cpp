#include "HeatTransfer/Solver/MultigridPreconditionedConjugateGradientSolver.h"

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Core/Tensor3dMapper.h"

#include "HeatTransfer/Solver/Discretisation/Stencil.h"
#include "HeatTransfer/Solver/Smoothers/SmootherFactory.h"

namespace HeatTransfer::Solver
{
    MultigridPreconditionedConjugateGradientSolver::MultigridPreconditionedConjugateGradientSolver(
        const Core::SimulationParameters& parameters,
        const Core::MaterialProperties& materialProperties,
        const Core::BoundaryConditions& boundaryCondition)
        : ISolver(parameters, materialProperties, boundaryCondition)
        , _preconditioner(std::make_shared<MultigridPreconditioner>(
              Smoothers::SmootherFactory(parameters.Smoother, parameters.RelaxationFactor)
                  .Create()))
    {
    }

    void MultigridPreconditionedConjugateGradientSolver::Initialise()
    {
        ZoneScoped;

        _preconditioner->BuildLevels(*_system);

        InitialiseConjugateGradientVariables();
    }

    void MultigridPreconditionedConjugateGradientSolver::InitialiseConjugateGradientVariables()
    {
        // We need to compute 4 textbook variables: grad f_0, d_0, s_0, and grad f_0^T * d_0

        // Say we have H * x = b, where H is the Hessian of f(x) = 0.5 * x^T H x - b^T x
        // The residual is given by r = b - H * x
        // The first gradient is given grad f_0 = H * x_0 - b
        // Thus grad f_0 = -r_0, and the first gradient is the negative of the residual
        _gradient = Stencil::ComputeResidual(*_temperatureTensor, _rhs, *_system);
        Core::Tensor3dMapper::AsVector(_gradient) *= -1.0;

        // d_0 = B * grad f_0
        PreconditionGradient();

        // s_0 = -d_0
        _searchDirection = _preconditionedGradient;
        Core::Tensor3dMapper::AsVector(_searchDirection) *= -1.0;

        // grad f_0^T * d_0
        _gradientDotPreconditioned = Dot(_gradient, _preconditionedGradient);
    }

    void MultigridPreconditionedConjugateGradientSolver::Iterate()
    {
        ZoneScoped;

        const Core::Tensor3d Hs = ApplyHessian(_searchDirection);
        const double sHs = Dot(_searchDirection, Hs);

        // H is symmetric positive definite, so s^T * H * s > 0 for any non-zero vector s
        // If sHs == 0, then the search direction is zero, and we have converged to the solution
        if (sHs <= 0.0)
            return;

        // alpha_k = -grad f_k^T * s_k / (s_k^T * H * s_k)
        const double alpha = -Dot(_gradient, _searchDirection) / sHs;

        // x_k+1 = x_k + alpha_k * s_k
        Core::Tensor3dMapper::AsVector(*_temperatureTensor) +=
            alpha * Core::Tensor3dMapper::AsVector(_searchDirection);

        // grad f_k+1 = grad f_k + alpha_k * H * s_k
        Core::Tensor3dMapper::AsVector(_gradient) += alpha * Core::Tensor3dMapper::AsVector(Hs);

        // d_k+1 = B * grad f_k+1
        PreconditionGradient();

        // beta_k = grad f_k+1^T * d_k+1 / (grad f_k^T * d_k)
        const double gradientDotPreconditioned = Dot(_gradient, _preconditionedGradient);
        const double beta = gradientDotPreconditioned / _gradientDotPreconditioned;

        // s_k+1 = -d_k+1 + beta_k * s_k
        auto S = Core::Tensor3dMapper::AsVector(_searchDirection);
        S = -Core::Tensor3dMapper::AsVector(_preconditionedGradient) + beta * S;
        _gradientDotPreconditioned = gradientDotPreconditioned;
    }

    void MultigridPreconditionedConjugateGradientSolver::PreconditionGradient()
    {
        // Say we have a linear system H * x = b, where H is the Hessian of f(x) = 0.5 * x^T H x -
        // b^T x. A V-Cycle approximately solves H * x = b, to get x = H^-1 * b.
        // Thus H * x = grad f becomes x = H^-1 * grad f, which is the preconditioned gradient d

        _preconditioner->Precondition(_preconditionedGradient, _gradient);
    }

    Core::Tensor3d
    MultigridPreconditionedConjugateGradientSolver::ApplyHessian(const Core::Tensor3d& matrix)
    {
        // For a linear system H * x = b, compute residual returns r = b - H * x
        // Setting b = 0, gives r = - H * x or H * x = -r
        // We will mainly be applying the Hessian to the search direction s

        Core::Tensor3d zero(matrix.dimensions());
        zero.setZero();

        Core::Tensor3d Hx = Stencil::ComputeResidual(matrix, zero, *_system);
        Core::Tensor3dMapper::AsVector(Hx) *= -1.0;

        return Hx;
    }

    double MultigridPreconditionedConjugateGradientSolver::Dot(const Core::Tensor3d& a,
                                                               const Core::Tensor3d& b)
    {
        return Core::Tensor3dMapper::AsVector(a).dot(Core::Tensor3dMapper::AsVector(b));
    }
}
