#pragma once

#include <array>

#include "HeatTransfer/Core/Faces.h"
#include "HeatTransfer/Core/Tensor3d.h"

namespace HeatTransfer::Solver
{
    class SystemMatrix
    {
    public:
        // The matrix H of the linear system H * x = b. Mainly used to store the diagonal of H

        using BiotNumbers = std::array<double, Core::Faces::ALL.size()>;

        // The inverse Fourier number is zero for steady state, i.e. an infinite time step
        SystemMatrix(Core::Tensor3d fixedTemperatureMask,
                     const BiotNumbers& biotNumbers,
                     double inverseFourierNumber = 0.0);

        const Core::Tensor3d& GetFixedTemperatureMask() const;
        const Core::Tensor3d& GetDiagonal() const;
        const BiotNumbers& GetBiotNumbers() const;
        double GetInverseFourierNumber() const;

    private:
        Core::Tensor3d _fixedTemperatureMask;
        Core::Tensor3d _diagonal;
        BiotNumbers _biotNumbers;
        double _inverseFourierNumber;

        void BuildDiagonal();
    };
}
