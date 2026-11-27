#pragma once

namespace HeatTransfer::Core
{
    struct InitialConditions
    {
        double Temperature; // Uniform temperature at t = 0 [K]

        static InitialConditions Default();
    };
}
