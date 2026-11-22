#include "HeatTransfer/Solver/Discretisation/SystemMatrix.h"

#include <utility>

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Solver/Discretisation/Stencil.h"

namespace HeatTransfer::Solver
{
    SystemMatrix::SystemMatrix(Core::Tensor3d fixedTemperatureMask, const BiotNumbers& biotNumbers)
        : _fixedTemperatureMask(std::move(fixedTemperatureMask)), _biotNumbers(biotNumbers)
    {
        BuildDiagonal();
    }

    void SystemMatrix::BuildDiagonal()
    {
        // Diagonal = sum of neighbour coefficients + convection terms, zero on fixed points
        // Convection term = Bi * area of face, for each convective face

        ZoneScoped;

        const Eigen::Index rows = _fixedTemperatureMask.dimension(0);
        const Eigen::Index cols = _fixedTemperatureMask.dimension(1);
        const Eigen::Index layers = _fixedTemperatureMask.dimension(2);

        const auto& dimensions = _fixedTemperatureMask.dimensions();

        _diagonal = Core::Tensor3d(dimensions);
        _diagonal.setConstant(1.0);

#pragma omp parallel for collapse(2) schedule(static)
        for (Eigen::Index k = 0; k < layers; k++)
        {
            for (Eigen::Index j = 0; j < cols; j++)
            {
                for (Eigen::Index i = 0; i < rows; i++)
                {
                    if (_fixedTemperatureMask(i, j, k))
                        continue;

                    double diagonal =
                        Stencil::SumNeighbourCoefficients(_fixedTemperatureMask, i, j, k);

                    for (const auto face : Core::Faces::ALL)
                    {
                        if (!Stencil::IsOnFace(face, dimensions, i, j, k))
                            continue;

                        const double area = Stencil::FaceArea(face, dimensions, i, j, k);

                        diagonal += _biotNumbers[(size_t)face] * area;
                    }

                    _diagonal(i, j, k) = diagonal;
                }
            }
        }
    }

    const Core::Tensor3d& SystemMatrix::GetFixedTemperatureMask() const
    {
        return _fixedTemperatureMask;
    }

    const Core::Tensor3d& SystemMatrix::GetDiagonal() const
    {
        return _diagonal;
    }

    const SystemMatrix::BiotNumbers& SystemMatrix::GetBiotNumbers() const
    {
        return _biotNumbers;
    }
}
