#pragma once

namespace HeatTransfer::Core
{
    enum class Method
    {
        JACOBI,
        GAUSS_SEIDEL,
        SUCCESSIVE_OVER_RELAXATION
    };
}
