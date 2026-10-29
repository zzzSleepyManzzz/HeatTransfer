#pragma once

#include <Eigen/Dense>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/SimulationParameters.h"

#include "HeatTransfer/Solvers/ISolver.h"

namespace HeatTransfer::Solvers
{
    class SuccessiveOverRelaxation : public ISolver
    {
    public:
        SuccessiveOverRelaxation(const Core::SimulationParameters& parameters,
                                 const Core::MaterialProperties& materialProperties,
                                 const Core::BoundaryConditions& boundaryCondition);

    private:
        void Smoothen(Eigen::MatrixXd& matrix,
                      const Eigen::MatrixXd& rhs,
                      const InsulatedMask& insulated) override;
    };
}