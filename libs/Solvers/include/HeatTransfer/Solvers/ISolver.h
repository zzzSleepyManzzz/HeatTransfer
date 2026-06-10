#pragma once

#include <cmath>
#include <format>
#include <iostream>
#include <vector>

#include <Eigen/dense>

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

        virtual const std::vector<Core::IterationAndError>& GetErrors();

    protected:
        Core::SimulationParameters _parameters;
        Core::BoundaryConditions _boundaryCondition;
        std::vector<Core::IterationAndError> _errors;
        Eigen::MatrixXd T;

        ISolver(const Core::SimulationParameters& parameters,
                const Core::BoundaryConditions& boundaryCondition);
        void ApplyBoundaryConditions();
        void ExpandMatrix();
    };
}