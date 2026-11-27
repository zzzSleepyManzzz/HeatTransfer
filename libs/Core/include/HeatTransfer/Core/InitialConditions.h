#pragma once

namespace HeatTransfer::Core
{
    struct InitialConditions
    {
        double Temperature;

        static InitialConditions Default();
    };
}
