#pragma once

#include <cmath>
#include <format>
#include <iostream>
#include <memory>
#include <vector>

#include <Eigen/Dense>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/ErrorMetric.h"
#include "HeatTransfer/Core/ResidualMetric.h"
#include "HeatTransfer/Core/SimulationParameters.h"

namespace HeatTransfer::Solvers
{
    class ISolver
    {
    public:
        ISolver() = delete;
        virtual ~ISolver() = default;

        void ComputeSimulation();
        void PrintResidual();

        bool IsConverged();

        const std::vector<std::shared_ptr<Core::ErrorMetric>>& GetErrorMetrics();
        const std::vector<std::shared_ptr<Core::ResidualMetric>>& GetResidualMetrics();

        std::shared_ptr<Eigen::MatrixXd> GetTemperatureMatrix();
        std::shared_ptr<Eigen::MatrixXd> GetResidualMatrix();

    protected:
        Core::SimulationParameters _parameters;
        Core::BoundaryConditions _boundaryCondition;
        std::vector<std::shared_ptr<Core::ErrorMetric>> _errorMetrics;
        std::vector<std::shared_ptr<Core::ResidualMetric>> _residualMetrics;
        std::shared_ptr<Eigen::MatrixXd> _temperatureMatrix;
        std::shared_ptr<Eigen::MatrixXd> _residualMatrix;

        ISolver(const Core::SimulationParameters& parameters,
                const Core::BoundaryConditions& boundaryCondition);

        void IterateTemperature(int maxIterations,
                                double residualTolerance,
                                double errorTolerance,
                                int& totalIterations);
        void ApplyBoundaryConditions();
        void ExpandMatrix();

        bool IsInsulated(Eigen::Index row, Eigen::Index col);
        double InsulatedValue(Eigen::Index row, Eigen::Index col);

        virtual void UpdateTemperatureMatrix(const Eigen::MatrixXd& oldTemperature) = 0;

        void UpdateResidualMatrix();
    };
}