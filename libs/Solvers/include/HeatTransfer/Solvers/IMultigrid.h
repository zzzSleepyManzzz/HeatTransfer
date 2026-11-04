#pragma once

#include <memory>

#include "HeatTransfer/Solvers/ISolver.h"
#include "HeatTransfer/Solvers/Smoothers/ISmoother.h"

namespace HeatTransfer::Solvers
{
    class IMultigrid : public ISolver
    {
    public:
        IMultigrid() = delete;
        virtual ~IMultigrid() = default;

    protected:
        static constexpr int PRE_SMOOTHING_SWEEPS = 3;
        static constexpr int POST_SMOOTHING_SWEEPS = 3;
        static constexpr int COARSE_SWEEPS = 100;
        static constexpr Eigen::Index MIN_COARSE_SIZE = 8;

        std::vector<Eigen::MatrixXd> _errorStack;
        std::vector<Eigen::MatrixXd> _residualStack;
        std::vector<Eigen::MatrixXd> _insulatedStack;

        IMultigrid(const Core::SimulationParameters& parameters,
                   const Core::MaterialProperties& materialProperties,
                   const Core::BoundaryConditions& boundaryCondition,
                   std::shared_ptr<const Smoothers::ISmoother> smoother);

        void BuildLevels() override;
        virtual void InitialiseConjugateGradientVariables() = 0;

        void VCycle(Eigen::MatrixXd& matrix, const Eigen::MatrixXd& rhs);
        void SolveCoarsest();

        Eigen::MatrixXd ComputeResidual(const Eigen::MatrixXd& matrix,
                                        const Eigen::MatrixXd& rhs,
                                        const Eigen::MatrixXd& insulated);

        Eigen::Index CoarseSize(Eigen::Index n);
        Eigen::MatrixXd CoarsenMask(const Eigen::MatrixXd& mask);
        Eigen::MatrixXd Restrict(const Eigen::MatrixXd& matrix);
        Eigen::MatrixXd
        Prolongate(const Eigen::MatrixXd& matrix, Eigen::Index rows, Eigen::Index cols);

    private:
        std::shared_ptr<const Smoothers::ISmoother> _smoother;
    };
}
