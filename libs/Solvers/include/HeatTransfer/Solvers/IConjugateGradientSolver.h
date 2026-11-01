#pragma once

#include "HeatTransfer/Solvers/IMultigridSolver.h"

namespace HeatTransfer::Solvers
{
    class IConjugateGradientSolver : public IMultigridSolver
    {
    public:
        IConjugateGradientSolver() = delete;
        virtual ~IConjugateGradientSolver() = default;

    protected:
        Eigen::MatrixXd _gradient;               // grad f_k
        Eigen::MatrixXd _preconditionedGradient; // d_k = B * grad f_k
        Eigen::MatrixXd _searchDirection;        // s_k
        double _gradientDotPreconditioned = 0.0; // grad f_k^T * d_k

        IConjugateGradientSolver(const Core::SimulationParameters& parameters,
                                 const Core::MaterialProperties& materialProperties,
                                 const Core::BoundaryConditions& boundaryCondition);

        void Iterate() override;

        void InitialiseConjugateGradientVariables() override;
        void PreconditionGradient();
        Eigen::MatrixXd ApplyHessian(const Eigen::MatrixXd& matrix);

        double Dot(const Eigen::MatrixXd& a, const Eigen::MatrixXd& b);
    };
}
