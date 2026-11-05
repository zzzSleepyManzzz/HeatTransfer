#pragma once

#include <memory>
#include <vector>

#include <Eigen/Dense>

#include "HeatTransfer/Solvers/Smoothers/ISmoother.h"

namespace HeatTransfer::Solvers
{
    class MultigridPreconditioner
    {
    public:
        explicit MultigridPreconditioner(std::shared_ptr<const Smoothers::ISmoother> smoother);

        void BuildLevels(const Eigen::MatrixXd& insulated);
        void Precondition(Eigen::MatrixXd& result, const Eigen::MatrixXd& rhs);

    private:
        static constexpr int PRE_SMOOTHING_SWEEPS = 3;
        static constexpr int POST_SMOOTHING_SWEEPS = 3;
        static constexpr int COARSE_SWEEPS = 100;
        static constexpr Eigen::Index MIN_COARSE_SIZE = 8;

        std::shared_ptr<const Smoothers::ISmoother> _smoother;

        std::vector<Eigen::MatrixXd> _errorStack;
        std::vector<Eigen::MatrixXd> _residualStack;
        std::vector<Eigen::MatrixXd> _insulatedStack;

        void VCycle(Eigen::MatrixXd& matrix, const Eigen::MatrixXd& rhs);
        void SolveCoarsest();
    };
}
