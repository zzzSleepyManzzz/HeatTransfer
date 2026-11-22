#pragma once

#include <atomic>
#include <cmath>
#include <format>
#include <iostream>
#include <memory>
#include <stop_token>
#include <vector>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/ErrorMetric.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/ResidualMetric.h"
#include "HeatTransfer/Core/SimulationParameters.h"
#include "HeatTransfer/Core/Tensor3d.h"

#include "HeatTransfer/Solver/Discretisation/SystemMatrix.h"

namespace HeatTransfer::Solver
{
    class ISolver
    {
    public:
        ISolver() = delete;
        virtual ~ISolver() = default;

        void ComputeSimulation(std::stop_token stopToken = {});
        void PrintResidual();

        bool IsConverged();

        int GetCurrentIteration() const;

        const std::vector<std::shared_ptr<Core::ErrorMetric>>& GetErrorMetrics();
        const std::vector<std::shared_ptr<Core::ResidualMetric>>& GetResidualMetrics();

        std::shared_ptr<Core::Tensor3d> GetTemperatureTensor();
        std::shared_ptr<Core::Tensor3d> GetResidualTensor();

    protected:
        Core::SimulationParameters _parameters;
        Core::MaterialProperties _materialProperties;
        Core::BoundaryConditions _boundaryCondition;
        std::vector<std::shared_ptr<Core::ErrorMetric>> _errorMetrics;
        std::vector<std::shared_ptr<Core::ResidualMetric>> _residualMetrics;
        std::shared_ptr<Core::Tensor3d> _temperatureTensor;
        std::shared_ptr<Core::Tensor3d> _residualTensor;

        std::shared_ptr<SystemMatrix> _system;
        Core::Tensor3d _rhs;

        std::atomic<int> _currentIteration = 0;

        ISolver(const Core::SimulationParameters& parameters,
                const Core::MaterialProperties& materialProperties,
                const Core::BoundaryConditions& boundaryCondition);

        void IterateTemperature(std::stop_token stopToken);

        void UpdateResidualTensor();

        virtual void Initialise() = 0;
        virtual void Iterate() = 0;
    };
}
