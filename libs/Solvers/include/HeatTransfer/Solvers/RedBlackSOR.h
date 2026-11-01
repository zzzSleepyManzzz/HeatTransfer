#pragma once

#include "HeatTransfer/Solvers/IConjugateGradientSolver.h"

namespace HeatTransfer::Solvers
{
    class RedBlackSOR : public IConjugateGradientSolver
    {
    public:
        RedBlackSOR(const Core::SimulationParameters& parameters,
                    const Core::MaterialProperties& materialProperties,
                    const Core::BoundaryConditions& boundaryCondition);

    private:
        void Smoothen(Eigen::MatrixXd& matrix,
                      const Eigen::MatrixXd& rhs,
                      const Eigen::MatrixXd& insulated,
                      SweepDirection direction) override;
    };
}
