#pragma once

#include <memory>

#include "HeatTransfer/Core/Smoother.h"

#include "HeatTransfer/Solver/Smoothers/ISmoother.h"

namespace HeatTransfer::Solver::Smoothers
{
    class SmootherFactory
    {
    public:
        SmootherFactory(Core::Smoother smoother, double relaxationFactor);

        std::shared_ptr<const ISmoother> Create() const;

    private:
        Core::Smoother _smoother;
        double _relaxationFactor;
    };
}
