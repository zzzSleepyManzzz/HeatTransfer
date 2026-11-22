#pragma once

#include "HeatTransfer/Core/Face.h"
#include "HeatTransfer/Core/Letter.h"

namespace HeatTransfer::Core
{
    struct LetterPatch
    {
        Core::Letter Letter = Core::Letter::H;
        Face Surface = Face::TOP;

        // Using u-v plane between normalized [0, 1]
        double UMin = 0.0;
        double UMax = 1.0;
        double VMin = 0.0;
        double VMax = 1.0;

        double Temperature = 300.0; // [K]
        bool Enabled = true;
    };
}
