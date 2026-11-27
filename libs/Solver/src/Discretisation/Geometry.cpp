#include "HeatTransfer/Solver/Discretisation/Geometry.h"

#include <stdexcept>

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Core/Glyph.h"
#include "HeatTransfer/Solver/Discretisation/Stencil.h"

namespace HeatTransfer::Solver
{
    Geometry::Geometry(const Core::SimulationParameters& parameters,
                       const Core::MaterialProperties& materialProperties,
                       const Core::BoundaryConditions& boundaryConditions)
        : _parameters(parameters)
        , _materialProperties(materialProperties)
        , _boundaryConditions(boundaryConditions)
        , _dimensions(parameters.Rows, parameters.Columns, parameters.Layers)
    {
        if (!_boundaryConditions.IsWellPosed())
            throw std::invalid_argument("Problem is ill-posed");
    }

    Core::Tensor3d Geometry::BuildFixedTemperatureMask() const
    {
        ZoneScoped;

        const Eigen::Index rows = _parameters.Rows;
        const Eigen::Index cols = _parameters.Columns;
        const Eigen::Index layers = _parameters.Layers;

        auto fixedTemperatureMask = Core::Tensor3d(rows, cols, layers);
        fixedTemperatureMask.setZero();

#pragma omp parallel for collapse(2) schedule(static)
        for (Eigen::Index k = 0; k < layers; k++)
        {
            for (Eigen::Index j = 0; j < cols; j++)
            {
                for (Eigen::Index i = 0; i < rows; i++)
                {
                    if (HasFixedTemperature(i, j, k))
                        fixedTemperatureMask(i, j, k) = 1.0;
                }
            }
        }

        return fixedTemperatureMask;
    }

    bool Geometry::HasFixedTemperature(Eigen::Index i, Eigen::Index j, Eigen::Index k) const
    {
        for (const auto& letter : _boundaryConditions.Letters)
        {
            if (IsInsideLetter(letter, i, j, k))
                return true;
        }

        for (const auto face : Core::Faces::ALL)
        {
            if (IsOnFixedFace(face, i, j, k))
                return true;
        }

        return false;
    }

    bool Geometry::IsInsideLetter(const Core::LetterPatch& letter,
                                  Eigen::Index i,
                                  Eigen::Index j,
                                  Eigen::Index k) const
    {
        if (!letter.Enabled || !Stencil::IsOnFace(letter.Surface, _dimensions, i, j, k))
            return false;

        auto rows = _parameters.Rows;
        auto cols = _parameters.Columns;
        auto layers = _parameters.Layers;

        // Normalize between [0, 1]
        const auto x = (double)j / (double)(cols - 1);
        const auto y = (double)i / (double)(rows - 1);
        const auto z = (double)k / (double)(layers - 1);

        double u;
        double v;

        switch (letter.Surface)
        {
            case Core::Face::LEFT:
            case Core::Face::RIGHT:
                u = y;
                v = z;
                break;
            case Core::Face::TOP:
            case Core::Face::BOTTOM:
                u = x;
                v = y;
                break;
            case Core::Face::FRONT:
            case Core::Face::BACK:
                u = x;
                v = z;
                break;
            default:
                throw std::runtime_error("Invalid face");
        }

        // Position within the letter's box
        const double s = (u - letter.UMin) / (letter.UMax - letter.UMin);
        const double t = (v - letter.VMin) / (letter.VMax - letter.VMin);

        return Core::Glyph::Contains(letter.Letter, s, t);
    }

    bool

    Geometry::IsOnFixedFace(Core::Face face, Eigen::Index i, Eigen::Index j, Eigen::Index k) const
    {
        return _boundaryConditions.GetFace(face).Type == Core::BoundaryType::FIXED_TEMPERATURE &&
               Stencil::IsOnFace(face, _dimensions, i, j, k);
    }

    void Geometry::ApplyFixedTemperatures(Core::Tensor3d& temperature) const
    {
        ZoneScoped;

        const Eigen::Index rows = _parameters.Rows;
        const Eigen::Index cols = _parameters.Columns;
        const Eigen::Index layers = _parameters.Layers;

#pragma omp parallel for collapse(2) schedule(static)
        for (Eigen::Index k = 0; k < layers; k++)
        {
            for (Eigen::Index j = 0; j < cols; j++)
            {
                for (Eigen::Index i = 0; i < rows; i++)
                {
                    if (HasFixedTemperature(i, j, k))
                        temperature(i, j, k) = FixedTemperature(i, j, k);
                }
            }
        }
    }

    double Geometry::FixedTemperature(Eigen::Index i, Eigen::Index j, Eigen::Index k) const
    {
        for (const auto& letter : _boundaryConditions.Letters)
        {
            if (IsInsideLetter(letter, i, j, k))
                return letter.Temperature;
        }

        for (const auto face : Core::Faces::ALL)
        {
            if (IsOnFixedFace(face, i, j, k))
                return _boundaryConditions.GetFace(face).Temperature;
        }

        throw std::logic_error("Point does not have a fixed temperature");
    }

    SystemMatrix::BiotNumbers Geometry::BuildBiotNumbers() const
    {
        // Bi = h_coeff * h / k_coeff for each convective face

        auto biotNumbers = SystemMatrix::BiotNumbers{};

        for (const auto face : Core::Faces::ALL)
        {
            const auto& faceBoundaryCondition = _boundaryConditions.GetFace(face);

            if (faceBoundaryCondition.Type != Core::BoundaryType::CONVECTION)
                continue;

            auto h_coeff = faceBoundaryCondition.HeatTransferCoefficient;
            auto h = _parameters.GridSpacing;
            auto k_coeff = _materialProperties.ThermalConductivity;

            biotNumbers[(size_t)face] = h_coeff * h / k_coeff;
        }

        return biotNumbers;
    }

    double Geometry::BuildInverseFourierNumber() const
    {
        // 1 / Fo = pow(h, 2) / (alpha * dt), where alpha = k_coeff / (rho * c)

        auto h = _parameters.GridSpacing;
        auto dt = _parameters.TimeStep;
        auto k_coeff = _materialProperties.ThermalConductivity;
        auto rho = _materialProperties.Density;
        auto c = _materialProperties.SpecificHeatCapacity;

        auto alpha = k_coeff / (rho * c);

        return pow(h, 2) / (alpha * dt);
    }

    Core::Tensor3d Geometry::BuildSteadyStateRhs() const
    {
        // b = (heat generation term + heat flux term + convection term) / 6

        // heat generation term  = w * pow(h,2) * q_g / k
        // heat flux term        = sigma_flux_faces(a * h_coeff * q_flux / k)
        // convection term       = sigma_conv_faces(a * Bi * T_ambient)

        ZoneScoped;

        const Eigen::Index rows = _parameters.Rows;
        const Eigen::Index cols = _parameters.Columns;
        const Eigen::Index layers = _parameters.Layers;

        const double h_coeff = _parameters.GridSpacing;
        const double k_coeff = _materialProperties.ThermalConductivity;
        const double q_g = _materialProperties.InternalHeatGeneration;

        const auto biotNumbers = BuildBiotNumbers();

        auto rhs = Core::Tensor3d(rows, cols, layers);
        rhs.setZero();

#pragma omp parallel for collapse(2) schedule(static)
        for (Eigen::Index k = 0; k < layers; k++)
        {
            for (Eigen::Index j = 0; j < cols; j++)
            {
                for (Eigen::Index i = 0; i < rows; i++)
                {
                    if (HasFixedTemperature(i, j, k))
                        continue;

                    double b = 0;

                    double w = Stencil::ControlVolume(_dimensions, i, j, k);
                    double heatGenerationTerm = w * pow(h_coeff, 2) * q_g / k_coeff;

                    b += heatGenerationTerm;

                    for (const auto face : Core::Faces::ALL)
                    {
                        if (!Stencil::IsOnFace(face, _dimensions, i, j, k))
                            continue;

                        const auto& faceBoundaryCondition = _boundaryConditions.GetFace(face);
                        const double area = Stencil::FaceArea(face, _dimensions, i, j, k);

                        if (faceBoundaryCondition.Type == Core::BoundaryType::HEAT_FLUX)
                        {
                            double heatFluxTerm =
                                area * faceBoundaryCondition.HeatFlux * h_coeff / k_coeff;
                            b += heatFluxTerm;
                        }

                        if (faceBoundaryCondition.Type == Core::BoundaryType::CONVECTION)
                        {
                            double convectionTerm = area * biotNumbers[(size_t)face] *
                                                    faceBoundaryCondition.AmbientTemperature;
                            b += convectionTerm;
                        }
                    }

                    rhs(i, j, k) = b / Stencil::SCALE;
                }
            }
        }

        return rhs;
    }

    Core::Tensor3d Geometry::BuildHeatCapacity() const
    {
        // Capacity = w / Fo, the scaled heat capacity of each point for one time step

        ZoneScoped;

        const Eigen::Index rows = _parameters.Rows;
        const Eigen::Index cols = _parameters.Columns;
        const Eigen::Index layers = _parameters.Layers;

        const double inverseFourierNumber = BuildInverseFourierNumber();

        auto capacity = Core::Tensor3d(rows, cols, layers);

#pragma omp parallel for collapse(2) schedule(static)
        for (Eigen::Index k = 0; k < layers; k++)
        {
            for (Eigen::Index j = 0; j < cols; j++)
            {
                for (Eigen::Index i = 0; i < rows; i++)
                {
                    capacity(i, j, k) =
                        Stencil::ControlVolume(_dimensions, i, j, k) * inverseFourierNumber;
                }
            }
        }

        return capacity;
    }
}
