#pragma once

#include <memory>

#include "HeatTransfer/Solver/ISolver.h"
#include "HeatTransfer/Solver/MultigridPreconditioner.h"

namespace HeatTransfer::Solver
{
    class MultigridPreconditionedConjugateGradientSolver : public ISolver
    {
    public:
        MultigridPreconditionedConjugateGradientSolver(
            const Core::SimulationParameters& parameters,
            const Core::MaterialProperties& materialProperties,
            const Core::BoundaryConditions& boundaryCondition);

        virtual ~MultigridPreconditionedConjugateGradientSolver() = default;

    private:
        std::shared_ptr<MultigridPreconditioner> _preconditioner;

        Core::Tensor3d _gradient;                // grad f_k
        Core::Tensor3d _preconditionedGradient;  // d_k = B * grad f_k
        Core::Tensor3d _searchDirection;         // s_k
        double _gradientDotPreconditioned = 0.0; // grad f_k^T * d_k

        void Iterate() override;

        void Initialise() override;
        void InitialiseConjugateGradientVariables();
        void PreconditionGradient();
        Core::Tensor3d ApplyHessian(const Core::Tensor3d& matrix);

        double Dot(const Core::Tensor3d& a, const Core::Tensor3d& b);
    };
}
