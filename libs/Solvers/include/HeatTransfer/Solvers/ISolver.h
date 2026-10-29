#pragma once

#include <cmath>
#include <format>
#include <iostream>
#include <memory>
#include <vector>

#include <Eigen/Dense>

#include "HeatTransfer/Core/BoundaryConditions.h"
#include "HeatTransfer/Core/ErrorMetric.h"
#include "HeatTransfer/Core/MaterialProperties.h"
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
        using InsulatedMask = Eigen::Array<bool, Eigen::Dynamic, Eigen::Dynamic>;

        ISolver(const Core::SimulationParameters& parameters,
                const Core::MaterialProperties& materialProperties,
                const Core::BoundaryConditions& boundaryCondition);

        Core::SimulationParameters _parameters;
        Core::MaterialProperties _materialProperties;
        Core::BoundaryConditions _boundaryCondition;
        std::vector<std::shared_ptr<Core::ErrorMetric>> _errorMetrics;
        std::vector<std::shared_ptr<Core::ResidualMetric>> _residualMetrics;
        std::shared_ptr<Eigen::MatrixXd> _temperatureMatrix;
        std::shared_ptr<Eigen::MatrixXd> _residualMatrix;

        void IterateTemperature();
        void ApplyBoundaryConditions();

        static bool
        IsInsulated(Eigen::Index row, Eigen::Index col, Eigen::Index rows, Eigen::Index cols);

        virtual void Smoothen(Eigen::MatrixXd& matrix,
                              const Eigen::MatrixXd& rhs,
                              const InsulatedMask& insulated) = 0;

        void UpdateResidualMatrix();

        virtual void BuildLevels() = 0;
        virtual void VCycle() = 0;

        static Eigen::MatrixXd ComputeResidual(const Eigen::MatrixXd& matrix,
                                               const Eigen::MatrixXd& rhs,
                                               const InsulatedMask& insulated);
    };
}
