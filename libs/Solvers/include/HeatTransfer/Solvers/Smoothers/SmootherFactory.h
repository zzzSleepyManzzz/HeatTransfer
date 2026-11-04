#pragma once

#include <memory>

#include "HeatTransfer/Core/Smoother.h"

#include "HeatTransfer/Solvers/Smoothers/ISmoother.h"

namespace HeatTransfer::Solvers::Smoothers
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
