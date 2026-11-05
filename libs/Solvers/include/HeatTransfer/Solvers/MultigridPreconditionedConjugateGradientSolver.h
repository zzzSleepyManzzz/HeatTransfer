#pragma once

#include <memory>

#include "HeatTransfer/Solvers/ISolver.h"
#include "HeatTransfer/Solvers/MultigridPreconditioner.h"

namespace HeatTransfer::Solvers
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

        Eigen::MatrixXd _gradient;               // grad f_k
        Eigen::MatrixXd _preconditionedGradient; // d_k = B * grad f_k
        Eigen::MatrixXd _searchDirection;        // s_k
        double _gradientDotPreconditioned = 0.0; // grad f_k^T * d_k

        void Iterate() override;

        void Initialise() override;
        void InitialiseConjugateGradientVariables();
        void PreconditionGradient();
        Eigen::MatrixXd ApplyHessian(const Eigen::MatrixXd& matrix);

        double Dot(const Eigen::MatrixXd& a, const Eigen::MatrixXd& b);
    };
}
