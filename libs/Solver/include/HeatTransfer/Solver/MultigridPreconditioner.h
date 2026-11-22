#pragma once

#include <memory>
#include <vector>

#include "HeatTransfer/Core/Tensor3d.h"

#include "HeatTransfer/Solver/Discretisation/SystemMatrix.h"
#include "HeatTransfer/Solver/Smoothers/ISmoother.h"

namespace HeatTransfer::Solver
{
    class MultigridPreconditioner
    {
    public:
        explicit MultigridPreconditioner(std::shared_ptr<const Smoothers::ISmoother> smoother);

        void BuildLevels(const SystemMatrix& system);
        void Precondition(Core::Tensor3d& result, const Core::Tensor3d& rhs);

    private:
        static constexpr int PRE_SMOOTHING_SWEEPS = 3;
        static constexpr int POST_SMOOTHING_SWEEPS = 3;
        static constexpr int COARSE_SWEEPS = 100;
        static constexpr Eigen::Index MIN_COARSE_SIZE = 4;

        std::shared_ptr<const Smoothers::ISmoother> _smoother;

        std::vector<Core::Tensor3d> _errorStack;
        std::vector<Core::Tensor3d> _residualStack;
        std::vector<SystemMatrix> _systemStack;

        bool CanCoarsen(const Core::Tensor3d& matrix) const;

        void VCycle(Core::Tensor3d& matrix, const Core::Tensor3d& rhs);
        void SolveCoarsest();
    };
}
