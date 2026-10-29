#pragma once

#include "HeatTransfer/Solvers/ISolver.h"

namespace HeatTransfer::Solvers
{
    class IMultigridSolver : public ISolver
    {
    public:
        IMultigridSolver() = delete;
        virtual ~IMultigridSolver() = default;

        IMultigridSolver(const Core::SimulationParameters& parameters,
                         const Core::MaterialProperties& materialProperties,
                         const Core::BoundaryConditions& boundaryCondition);

    protected:
        static constexpr int PreSmoothingSweeps = 2;
        static constexpr int PostSmoothingSweeps = 2;
        static constexpr int CoarseSweeps = 100;
        static constexpr Eigen::Index MinCoarseSize = 8;

        std::vector<Eigen::MatrixXd> _errorStack;
        std::vector<Eigen::MatrixXd> _residualStack;
        std::vector<InsulatedMask> _insulatedStack;

        void BuildLevels() override;
        void VCycle() override;
        void SolveCoarsest();
        Eigen::MatrixXd& LevelMatrix(std::size_t level);

        static Eigen::Index CoarseSize(Eigen::Index n);
        static InsulatedMask CoarsenMask(const InsulatedMask& mask);
        static Eigen::MatrixXd Restrict(const Eigen::MatrixXd& matrix);
        static Eigen::MatrixXd
        Prolongate(const Eigen::MatrixXd& matrix, Eigen::Index rows, Eigen::Index cols);
    };
}