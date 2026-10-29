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
    // Every solver runs geometric multigrid V-cycles; the subclass chooses the smoother.
    //
    // All grids solve the same scaled equation  x - (sum of 4 neighbours) / 4 = rhs:
    // on the finest grid x is the temperature and rhs the heat source h^2 q / (4 k);
    // on coarser grids x is an error correction and rhs the restricted residual.
    class ISolver
    {
    public:
        ISolver() = delete;
        virtual ~ISolver() = default;

        ISolver(const Core::SimulationParameters& parameters,
                const Core::MaterialProperties& materialProperties,
                const Core::BoundaryConditions& boundaryCondition);

        void ComputeSimulation();
        void PrintResidual();

        bool IsConverged();

        const std::vector<std::shared_ptr<Core::ErrorMetric>>& GetErrorMetrics();
        const std::vector<std::shared_ptr<Core::ResidualMetric>>& GetResidualMetrics();

        std::shared_ptr<Eigen::MatrixXd> GetTemperatureMatrix();
        std::shared_ptr<Eigen::MatrixXd> GetResidualMatrix();

    protected:
        // true on fixed-value nodes: outer edges, inner square outline, centre point
        using InsulatedMask = Eigen::Array<bool, Eigen::Dynamic, Eigen::Dynamic>;

        static constexpr int PreSmoothingSweeps = 2;
        static constexpr int PostSmoothingSweeps = 2;
        static constexpr int CoarseSweeps = 100;
        static constexpr Eigen::Index MinCoarseSize = 8;

        Core::SimulationParameters _parameters;
        Core::MaterialProperties _materialProperties;
        Core::BoundaryConditions _boundaryCondition;
        std::vector<std::shared_ptr<Core::ErrorMetric>> _errorMetrics;
        std::vector<std::shared_ptr<Core::ResidualMetric>> _residualMetrics;
        std::shared_ptr<Eigen::MatrixXd> _temperatureMatrix;
        std::shared_ptr<Eigen::MatrixXd> _residualMatrix;

        // One entry per grid, finest first. _errorStack[0] is unused: level 0 works on
        // the temperature matrix itself. _residualStack[0] is the heat source.
        std::vector<Eigen::MatrixXd> _errorStack;
        std::vector<Eigen::MatrixXd> _residualStack;
        std::vector<InsulatedMask> _insulatedStack;

        ISolver(const Core::SimulationParameters& parameters,
                const Core::BoundaryConditions& boundaryCondition);

        void IterateTemperature(int maxIterations,
                                double residualTolerance,
                                double errorTolerance,
                                int& totalIterations);
        void ApplyBoundaryConditions();

        static bool
        IsInsulated(Eigen::Index row, Eigen::Index col, Eigen::Index rows, Eigen::Index cols);

        // One relaxation sweep over the free nodes; insulated nodes are left untouched
        virtual void Smoothen(Eigen::MatrixXd& matrix,
                              const Eigen::MatrixXd& rhs,
                              const InsulatedMask& insulated) = 0;

        void UpdateResidualMatrix();

        // Multigrid
        void BuildLevels();
        void VCycle();
        void SolveCoarsest();
        Eigen::MatrixXd& LevelMatrix(std::size_t level);

        static Eigen::MatrixXd ComputeResidual(const Eigen::MatrixXd& matrix,
                                               const Eigen::MatrixXd& rhs,
                                               const InsulatedMask& insulated);
        static Eigen::Index CoarseSize(Eigen::Index n);
        static InsulatedMask CoarsenMask(const InsulatedMask& mask);
        static Eigen::MatrixXd Restrict(const Eigen::MatrixXd& matrix);
        static Eigen::MatrixXd
        Prolongate(const Eigen::MatrixXd& matrix, Eigen::Index rows, Eigen::Index cols);
    };
}
