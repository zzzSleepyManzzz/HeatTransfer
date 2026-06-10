#pragma once

namespace HeatTransfer::SimulationRunner
{
    enum class Method
    {
        JACOBI,
        GAUSS_SEIDEL,
        SUCCESSIVE_OVER_RELAXATION
    };
}
