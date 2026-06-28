#pragma once

#include <cmath>
#include <format>
#include <iostream>
#include <memory>
#include <vector>

#include <Eigen/Dense>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/IterationAndError.h"
#include "HeatTransfer/Core/SimulationParameters.h"

namespace HeatTransfer::Solvers
{
    class ISolver
    {
    public:
        ISolver() = delete;
        virtual ~ISolver() = default;
        virtual void ComputeSimulation() = 0;
        virtual void PrintError();

        virtual const std::vector<std::shared_ptr<Core::IterationAndError>>& GetErrors();
        std::shared_ptr<Eigen::MatrixXd> GetTemperatureMatrix();

    protected:
        Core::SimulationParameters _parameters;
        Core::BoundaryConditions _boundaryCondition;
        std::vector<std::shared_ptr<Core::IterationAndError>> _errors;
        std::shared_ptr<Eigen::MatrixXd> _temperatureMatrix;

        ISolver(const Core::SimulationParameters& parameters,
                const Core::BoundaryConditions& boundaryCondition);
        void ApplyBoundaryConditions();
        void ExpandMatrix();

        bool IsInsulated(Eigen::Index row, Eigen::Index col);
        double InsulatedValue(Eigen::Index row, Eigen::Index col);
    };
}