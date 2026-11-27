#include "HeatTransfer/Core/InitialConditions.h"

namespace HeatTransfer::Core
{
    InitialConditions InitialConditions::Default()
    {
        return {.Temperature = 300.0};
    }
}
