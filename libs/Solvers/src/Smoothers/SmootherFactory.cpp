#include "HeatTransfer/Solvers/Smoothers/SmootherFactory.h"

#include <stdexcept>

#include "HeatTransfer/Solvers/Smoothers/GaussSeidel.h"
#include "HeatTransfer/Solvers/Smoothers/Jacobi.h"
#include "HeatTransfer/Solvers/Smoothers/RedBlackSOR.h"
#include "HeatTransfer/Solvers/Smoothers/SuccessiveOverRelaxation.h"

namespace HeatTransfer::Solvers::Smoothers
{
    SmootherFactory::SmootherFactory(Core::Smoother smoother, double relaxationFactor)
        : _smoother(smoother), _relaxationFactor(relaxationFactor)
    {
    }

    std::shared_ptr<const ISmoother> SmootherFactory::Create() const
    {
        using enum Core::Smoother;

        switch (_smoother)
        {
            case JACOBI:
                return std::make_shared<Jacobi>();
            case GAUSS_SEIDEL:
                return std::make_shared<GaussSeidel>();
            case SUCCESSIVE_OVER_RELAXATION:
                return std::make_shared<SuccessiveOverRelaxation>(_relaxationFactor);
            case RED_BLACK_SOR:
                return std::make_shared<RedBlackSOR>(_relaxationFactor);
            default:
                throw std::runtime_error("Provided non-existant smoother");
        }
    }
}
