#pragma once

#include "HeatTransfer/Solvers/IMultigridSolver.h"

namespace HeatTransfer::Solvers
{
    class GaussSeidelMethod : public IMultigridSolver
    {
    public:
        GaussSeidelMethod(const Core::SimulationParameters& parameters,
                          const Core::MaterialProperties& materialProperties,
                          const Core::BoundaryConditions& boundaryCondition);

    private:
        void Smoothen(Eigen::MatrixXd& matrix,
                      const Eigen::MatrixXd& rhs,
                      const InsulatedMask& insulated) override;
    };
}