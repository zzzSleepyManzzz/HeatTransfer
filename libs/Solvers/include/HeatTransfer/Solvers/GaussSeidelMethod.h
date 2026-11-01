#pragma once

#include "HeatTransfer/Solvers/IConjugateGradientSolver.h"

namespace HeatTransfer::Solvers
{
    class GaussSeidelMethod : public IConjugateGradientSolver
    {
    public:
        GaussSeidelMethod(const Core::SimulationParameters& parameters,
                          const Core::MaterialProperties& materialProperties,
                          const Core::BoundaryConditions& boundaryCondition);

    private:
        void Smoothen(Eigen::MatrixXd& matrix,
                      const Eigen::MatrixXd& rhs,
                      const Eigen::MatrixXd& insulated,
                      SweepDirection direction) override;
    };
}
