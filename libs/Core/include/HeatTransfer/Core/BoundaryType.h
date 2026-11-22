#pragma once

namespace HeatTransfer::Core
{
    enum class BoundaryType
    {
        FIXED_TEMPERATURE, // Dirichlet:  T = T_fixed
        HEAT_FLUX,         // Neumann:    -k dT/dn = q  (q = 0 is insulated)
        CONVECTION         // Robin:      -k dT/dn = h (T - T_ambient)
    };
}
