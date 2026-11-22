#include "HeatTransfer/Solver/Discretisation/Stencil.h"

#include <stdexcept>

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Core/Tensor3dMapper.h"

namespace HeatTransfer::Solver
{
    Core::Tensor3d Stencil::ComputeResidual(const Core::Tensor3d& matrix,
                                            const Core::Tensor3d& rhs,
                                            const SystemMatrix& system)
    {
        // r = b - H * x, zero on fixed points

        ZoneScoped;

        const Eigen::Index rows = matrix.dimension(0);
        const Eigen::Index cols = matrix.dimension(1);
        const Eigen::Index layers = matrix.dimension(2);

        const auto& fixedTemperatureMask = system.GetFixedTemperatureMask();

        auto R = Core::Tensor3d(matrix.dimensions());
        R.setZero();

#pragma omp parallel for collapse(2) schedule(static)
        for (Eigen::Index k = 0; k < layers; k++)
        {
            for (Eigen::Index j = 0; j < cols; j++)
            {
                for (Eigen::Index i = 0; i < rows; i++)
                {
                    if (fixedTemperatureMask(i, j, k))
                        continue;

                    R(i, j, k) = LocalResidual(matrix, rhs, system, i, j, k);
                }
            }
        }

        return R;
    }

    double Stencil::LocalResidual(const Core::Tensor3d& matrix,
                                  const Core::Tensor3d& rhs,
                                  const SystemMatrix& system,
                                  Eigen::Index i,
                                  Eigen::Index j,
                                  Eigen::Index k)
    {
        // r = b - (diagonal * x - sum of neighbours) / 6 for a single point

        const auto& T = matrix;
        const auto& D = system.GetDiagonal();

        return rhs(i, j, k) - (D(i, j, k) * T(i, j, k) - SumNeighbours(T, i, j, k)) / SCALE;
    }

    double Stencil::LocalSolve(const Core::Tensor3d& matrix,
                               const Core::Tensor3d& rhs,
                               const SystemMatrix& system,
                               Eigen::Index i,
                               Eigen::Index j,
                               Eigen::Index k)
    {
        // x = (sum of neighbours + 6 * b) / diagonal for a single point

        const auto& T = matrix;
        const auto& D = system.GetDiagonal();

        return (SumNeighbours(T, i, j, k) + SCALE * rhs(i, j, k)) / D(i, j, k);
    }

    double Stencil::SumNeighbours(const Core::Tensor3d& matrix,
                                  Eigen::Index i,
                                  Eigen::Index j,
                                  Eigen::Index k)
    {
        // Sum of the neighbours of a point, each multiplied by its coefficient

        const Eigen::Index rows = matrix.dimension(0);
        const Eigen::Index cols = matrix.dimension(1);
        const Eigen::Index layers = matrix.dimension(2);

        const auto& T = matrix;

        // The coefficient of a neighbour is the product of the weights of the other two directions
        const double gi = Weight(j, cols) * Weight(k, layers);
        const double gj = Weight(i, rows) * Weight(k, layers);
        const double gk = Weight(i, rows) * Weight(j, cols);

        double sum = 0.0;

        if (i > 0)
            sum += gi * T(i - 1, j, k);

        if (i < rows - 1)
            sum += gi * T(i + 1, j, k);

        if (j > 0)
            sum += gj * T(i, j - 1, k);

        if (j < cols - 1)
            sum += gj * T(i, j + 1, k);

        if (k > 0)
            sum += gk * T(i, j, k - 1);

        if (k < layers - 1)
            sum += gk * T(i, j, k + 1);

        return sum;
    }

    double Stencil::SumNeighbourCoefficients(const Core::Tensor3d& matrix,
                                             Eigen::Index i,
                                             Eigen::Index j,
                                             Eigen::Index k)
    {
        // Sum of the coefficients of the neighbours of a point

        const Eigen::Index rows = matrix.dimension(0);
        const Eigen::Index cols = matrix.dimension(1);
        const Eigen::Index layers = matrix.dimension(2);

        // The coefficient of a neighbour is the product of the weights of the other two directions
        const double gi = Weight(j, cols) * Weight(k, layers);
        const double gj = Weight(i, rows) * Weight(k, layers);
        const double gk = Weight(i, rows) * Weight(j, cols);

        double sum = 0.0;

        if (i > 0)
            sum += gi;

        if (i < rows - 1)
            sum += gi;

        if (j > 0)
            sum += gj;

        if (j < cols - 1)
            sum += gj;

        if (k > 0)
            sum += gk;

        if (k < layers - 1)
            sum += gk;

        return sum;
    }

    double Stencil::Weight(Eigen::Index index, Eigen::Index size)
    {
        // A point on a face owns half a cell in that direction
        return (index == 0 || index == size - 1) ? 0.5 : 1.0;
    }

    bool Stencil::IsOnFace(Core::Face face,
                           const Core::Tensor3d::Dimensions& dimensions,
                           Eigen::Index i,
                           Eigen::Index j,
                           Eigen::Index k)
    {
        const Eigen::Index rows = dimensions[0];
        const Eigen::Index cols = dimensions[1];
        const Eigen::Index layers = dimensions[2];

        switch (face)
        {
            case Core::Face::LEFT:
                return j == 0;
            case Core::Face::RIGHT:
                return j == cols - 1;
            case Core::Face::TOP:
                return k == layers - 1;
            case Core::Face::BOTTOM:
                return k == 0;
            case Core::Face::FRONT:
                return i == 0;
            case Core::Face::BACK:
                return i == rows - 1;
            default:
                throw std::runtime_error("Invalid face");
        }
    }

    double Stencil::FaceArea(Core::Face face,
                             const Core::Tensor3d::Dimensions& dimensions,
                             Eigen::Index i,
                             Eigen::Index j,
                             Eigen::Index k)
    {
        // Area of the control volume of a point that lies on a face, as a fraction of dx^2

        const double wi = Weight(i, dimensions[0]);
        const double wj = Weight(j, dimensions[1]);
        const double wk = Weight(k, dimensions[2]);

        switch (face)
        {
            case Core::Face::LEFT:
            case Core::Face::RIGHT:
                return wi * wk;
            case Core::Face::TOP:
            case Core::Face::BOTTOM:
                return wi * wj;
            case Core::Face::FRONT:
            case Core::Face::BACK:
                return wj * wk;
            default:
                throw std::runtime_error("Invalid face");
        }
    }

    double Stencil::ControlVolume(const Core::Tensor3d::Dimensions& dimensions,
                                  Eigen::Index i,
                                  Eigen::Index j,
                                  Eigen::Index k)
    {
        // Volume of the control volume of a point, as a fraction of dx^3
        // Denoted as w in equations

        const double wi = Weight(i, dimensions[0]);
        const double wj = Weight(j, dimensions[1]);
        const double wk = Weight(k, dimensions[2]);

        return wi * wj * wk;
    }

    void Stencil::ZeroFixedCells(Core::Tensor3d& matrix, const Core::Tensor3d& fixedTemperatureMask)
    {
        auto M = Core::Tensor3dMapper::AsVector(matrix);
        const auto F = Core::Tensor3dMapper::AsVector(fixedTemperatureMask);

        M.array() = F.array().select(0.0, M.array());
    }
}
