#pragma once

#include <atomic>
#include <cmath>
#include <format>
#include <iostream>
#include <memory>
#include <stop_token>
#include <vector>

#include <Eigen/Dense>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/ErrorMetric.h"
#include "HeatTransfer/Core/MaterialProperties.h"
#include "HeatTransfer/Core/ResidualMetric.h"
#include "HeatTransfer/Core/SimulationParameters.h"

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

        std::shared_ptr<Eigen::MatrixXd> GetTemperatureMatrix();
        std::shared_ptr<Eigen::MatrixXd> GetResidualMatrix();

    protected:
        Core::SimulationParameters _parameters;
        Core::MaterialProperties _materialProperties;
        Core::BoundaryConditions _boundaryCondition;
        std::vector<std::shared_ptr<Core::ErrorMetric>> _errorMetrics;
        std::vector<std::shared_ptr<Core::ResidualMetric>> _residualMetrics;
        std::shared_ptr<Eigen::MatrixXd> _temperatureMatrix;
        std::shared_ptr<Eigen::MatrixXd> _residualMatrix;

        Eigen::MatrixXd _insulated;
        Eigen::MatrixXd _heating;

        std::atomic<int> _currentIteration = 0;

        ISolver(const Core::SimulationParameters& parameters,
                const Core::MaterialProperties& materialProperties,
                const Core::BoundaryConditions& boundaryCondition);

        void IterateTemperature(std::stop_token stopToken);
        void ApplyBoundaryConditions();

        bool IsInsulated(Eigen::Index row, Eigen::Index col, Eigen::Index rows, Eigen::Index cols);
        Eigen::MatrixXd BuildInsulatedMask();
        Eigen::MatrixXd BuildHeating();

        void UpdateResidualMatrix();

        virtual void Initialise() = 0;
        virtual void Iterate() = 0;
    };
}
