#pragma once

#include <string>

namespace HeatTransfer::Visualisation
{
    struct SettingsState
    {
        bool lightModeOn = false;
        int iterativeMethod = 2; // SOR by default
    };
}