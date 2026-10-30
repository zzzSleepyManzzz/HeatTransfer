#pragma once

#include "HeatTransfer/Solvers/ISolver.h"

namespace HeatTransfer::Solvers
{
    class IMultigridSolver : public ISolver
    {
    public:
        IMultigridSolver() = delete;
        virtual ~IMultigridSolver() = default;

    protected:
        static constexpr int PRE_SMOOTHING_SWEEPS = 3;
        static constexpr int POST_SMOOTHING_SWEEPS = 3;
        static constexpr int COARSE_SWEEPS = 100;
        static constexpr Eigen::Index MIN_COARSE_SIZE = 8;

        std::vector<Eigen::MatrixXd> _errorStack;
        std::vector<Eigen::MatrixXd> _residualStack;
        std::vector<Eigen::MatrixXd> _insulatedStack;

        IMultigridSolver(const Core::SimulationParameters& parameters,
                         const Core::MaterialProperties& materialProperties,
                         const Core::BoundaryConditions& boundaryCondition);

        void BuildLevels() override;
        void VCycle() override;
        void SolveCoarsest();

        virtual void Smoothen(Eigen::MatrixXd& matrix,
                              const Eigen::MatrixXd& rhs,
                              const Eigen::MatrixXd& insulated) = 0;

        Eigen::MatrixXd ComputeResidual(const Eigen::MatrixXd& matrix,
                                        const Eigen::MatrixXd& rhs,
                                        const Eigen::MatrixXd& insulated);

        Eigen::MatrixXd& LevelMatrix(std::size_t level);

        static Eigen::Index CoarseSize(Eigen::Index n);
        static Eigen::MatrixXd CoarsenMask(const Eigen::MatrixXd& mask);
        static Eigen::MatrixXd Restrict(const Eigen::MatrixXd& matrix);
        static Eigen::MatrixXd
        Prolongate(const Eigen::MatrixXd& matrix, Eigen::Index rows, Eigen::Index cols);
    };
}
