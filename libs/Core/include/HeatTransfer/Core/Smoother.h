#pragma once

namespace HeatTransfer::Core
{
    enum class Smoother
    {
        JACOBI,
        GAUSS_SEIDEL,
        SUCCESSIVE_OVER_RELAXATION,
        RED_BLACK_SOR
    };
}
