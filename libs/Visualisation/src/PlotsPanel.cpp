#include "HeatTransfer/Visualisation/PlotsPanel.h"

#include <algorithm>
#include <cmath>
#include <format>
#include <string>
#include <vector>

#include <tracy/Tracy.hpp>

#include "HeatTransfer/Visualisation/Slicer.h"

namespace HeatTransfer::Visualisation
{
    PlotsPanel::PlotsPanel(std::shared_ptr<ConsoleLogger> logger)
        : IPanel(logger)
        , _volumeViewState(std::make_shared<VolumeViewState>())
        , _temperatureVolumeMeshCache(std::make_shared<VolumeMeshCache>())
        , _residualVolumeMeshCache(std::make_shared<VolumeMeshCache>())
        , _sliceState(std::make_shared<SliceState>())
        , _temperatureSurfacePlotState(std::make_shared<SurfacePlotState>())
        , _residualSurfacePlotState(
              std::make_shared<SurfacePlotState>(SurfacePlotState{.Field = PlotField::RESIDUAL}))
        , _errorConvergencePlotsState(std::make_shared<ConvergencePlotsState>())
        , _residualConvergencePlotsState(std::make_shared<ConvergencePlotsState>(
              ConvergencePlotsState{.Metric = ConvergenceMetric::RESIDUAL}))
    {
    }

    void PlotsPanel::Render(std::shared_ptr<RendererModel> model)
    {
        _model = model;
        ResetViewsIfNeeded();
        ShowPlotsWindow();
    }

    void PlotsPanel::ResetViewsIfNeeded()
    {
        const auto data = _model->GetRenderFrameData();

        if (_shownRenderFrameData == data)
            return;

        _shownRenderFrameData = data;

        _volumeViewState->ResetZoom = true;

        _temperatureSurfacePlotState->ResetZoom = true;
        _residualSurfacePlotState->ResetZoom = true;

        _errorConvergencePlotsState->ResetZoom = true;
        _residualConvergencePlotsState->ResetZoom = true;
    }

    void PlotsPanel::ShowPlotsWindow()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x * 4.0f / 5.0f, displaySize.y * 3.0f / 4.0f));
        ImGui::SetNextWindowPos(ImVec2(displaySize.x / 5.0f, 0));

        ImGui::Begin("Plots", nullptr, windowFlags);
        {
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            if (ImGui::BeginTabBar("Plot Tabs"))
            {
                if (ImGui::BeginTabItem("3D Temperature"))
                {
                    CreateVolumeView(PlotField::TEMPERATURE);

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("3D Residual"))
                {
                    CreateVolumeView(PlotField::RESIDUAL);

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Temperature surface"))
                {
                    CreateSurfacePlot(*_temperatureSurfacePlotState);

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Residual surface"))
                {
                    CreateSurfacePlot(*_residualSurfacePlotState);

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Error convergence"))
                {
                    CreateConvergencePlots(*_errorConvergencePlotsState);

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Residual convergence"))
                {
                    CreateConvergencePlots(*_residualConvergencePlotsState);

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void PlotsPanel::CreateVolumeView(PlotField field)
    {
        auto data = _model->GetRenderFrameData();
        auto& state = *_volumeViewState;

        const bool isResidual = field == PlotField::RESIDUAL;
        auto& meshCache = isResidual ? *_residualVolumeMeshCache : *_temperatureVolumeMeshCache;
        const auto& T = data->GetField(field);
        const char* valueLabel = isResidual ? "Residual" : "Temperature [K]";
        int& colorMap = isResidual ? state.ResidualColorMap : state.TemperatureColorMap;

        if (ImGui::Button("Reset view"))
        {
            state.ResetZoom = true;
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        ImGui::SliderFloat("View scale", &state.ViewScale, 0.5f, 3.0f, "%.2f");

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        ImGui::Combo(
            "Color map type", &colorMap, COLOR_MAP_OPTIONS, IM_ARRAYSIZE(COLOR_MAP_OPTIONS));

        ImGui::SameLine();
        ImGui::Checkbox("Show lines", &state.ShowLines);

        ImGui::SameLine();
        ImGui::Checkbox("Show surface", &state.ShowOuterFaces);

        // One checkbox and slider per slicing plane, disabled while the surface hides the slices

        ImGui::BeginDisabled(state.ShowOuterFaces);

        for (int axis = 0; axis < 3; axis++)
        {
            ImGui::PushID(axis);

            ImGui::Checkbox(PLANE_LABELS[axis], &state.ShowPlane[axis]);
            ImGui::SameLine();

            int index = Slicer::ClampIndex(T, (SliceAxis)axis, state.PlaneIndex[axis]);

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.4f);

            if (ImGui::SliderInt("Index", &index, 0, Slicer::AxisSize(T, (SliceAxis)axis) - 1))
            {
                state.PlaneIndex[axis] = index;
            }

            ImGui::PopID();
        }

        ImGui::EndDisabled();

        meshCache.Update(data, field, state); // Only updates when needed

        // Create mesh plot

        const double xMax = std::max(1.0, (double)Slicer::AxisSize(T, SliceAxis::X) - 1);
        const double yMax = std::max(1.0, (double)Slicer::AxisSize(T, SliceAxis::Y) - 1);
        const double zMax = std::max(1.0, (double)Slicer::AxisSize(T, SliceAxis::Z) - 1);
        const double longest = std::max({xMax, yMax, zMax});

        ImPlot3D::PushColormap(colorMap);
        ImPlot3D::PushStyleVar(ImPlot3DStyleVar_ViewScaleFactor, state.ViewScale);

        auto plotWindowWidth = ImGui::GetWindowSize().x;

        if (ImPlot3D::BeginPlot("## Volume",
                                ImVec2(plotWindowWidth * 0.93, -1),
                                ImPlot3DFlags_NoPan | ImPlot3DFlags_NoZoom))
        {
            ImPlot3D::SetupAxes("Length [mm]", "Width [mm]", "Height [mm]");
            SetupLengthAxisTicks(ImAxis3D_X, xMax, GetGridSpacing());
            SetupLengthAxisTicks(ImAxis3D_Y, yMax, GetGridSpacing());
            SetupLengthAxisTicks(ImAxis3D_Z, zMax, GetGridSpacing());

            const auto cond = state.ResetZoom ? ImPlot3DCond_Always : ImPlot3DCond_Once;

            ImPlot3D::SetupAxesLimits(0.0, xMax, 0.0, yMax, 0.0, zMax, cond);
            ImPlot3D::SetupBoxRotation(30, -45, true, cond);

            ImPlot3D::SetupBoxScale(xMax / longest, yMax / longest, zMax / longest);

            state.ResetZoom = false;

            ImPlot3DSpec spec;
            spec.FillAlpha = 1.0f;
            spec.LineColor = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);
            spec.Marker = ImPlot3DMarker_None;
            spec.Flags = ImPlot3DMeshFlags_NoLegend;

            if (!state.ShowLines)
                spec.Flags |= ImPlot3DMeshFlags_NoLines;

            auto plotMesh = [&](const char* label, VolumeMesh& mesh)
            {
                if (mesh.IsEmpty())
                    return;

                spec.FillColors = mesh.GetColours().data();

                ImPlot3D::PlotMesh(label,
                                   mesh.GetXs().data(),
                                   mesh.GetYs().data(),
                                   mesh.GetZs().data(),
                                   mesh.GetIndices().data(),
                                   (int)mesh.GetXs().size(),
                                   (int)mesh.GetIndices().size(),
                                   spec);
            };

            // Back-face culling. ImPlot3D has no depth buffer, it draws triangles sorted by the
            // depth of their centre, which gets the order wrong where a hidden face meets a
            // visible one. A face is visible when its outward normal points towards the camera,
            // i.e. has a positive depth after the plot rotation. Visible faces of a box never
            // overlap on screen, so they can be drawn in any order

            // Ordered as Core::Faces::ALL (left, right, top, bottom, front, back)
            const std::array<ImPlot3DPoint, 6> outwardNormals = {ImPlot3DPoint(-1, 0, 0),
                                                                 ImPlot3DPoint(1, 0, 0),
                                                                 ImPlot3DPoint(0, 0, 1),
                                                                 ImPlot3DPoint(0, 0, -1),
                                                                 ImPlot3DPoint(0, -1, 0),
                                                                 ImPlot3DPoint(0, 1, 0)};

            const auto& rotation = ImPlot3D::GetCurrentPlot()->Rotation;

            auto& faceMeshes = meshCache.GetFaceMeshes();

            for (std::size_t i = 0; i < faceMeshes.size(); i++)
            {
                const bool facesCamera = (rotation * outwardNormals[i]).z > 0.0;

                if (facesCamera)
                    plotMesh(FACE_LABELS[i], faceMeshes[i]);
            }

            plotMesh("##Slices", meshCache.GetSliceMesh()); // Empty when outer faces are shown

            ImPlot3D::EndPlot();
        }

        ImPlot3D::PopStyleVar();
        ImPlot3D::PopColormap();

        // Create color bar

        ImGui::SameLine();
        ImPlot::PushColormap(colorMap);

        ImPlot::ColormapScale(
            valueLabel, data->GetFieldMin(field), data->GetFieldMax(field), ImVec2(-1, -1));

        ImPlot::PopColormap();
    }

    void PlotsPanel::SetupLengthAxisTicks(ImAxis3D axis, double maxPoint, double gridSpacing)
    {
        // Ticks at round millimetre values on an axis whose coordinates are grid points

        const double lengthMm = maxPoint * gridSpacing * 1000.0;

        if (lengthMm <= 0.0)
            return;

        // About 5 ticks with a step of 1, 2 or 5 times a power of ten
        const double rawStep = lengthMm / 5.0;
        const double magnitude = std::pow(10.0, std::floor(std::log10(rawStep)));
        const double normalised = rawStep / magnitude;
        const double step = (normalised < 1.5   ? 1.0
                             : normalised < 3.5 ? 2.0
                             : normalised < 7.5 ? 5.0
                                                : 10.0) *
                            magnitude;

        std::vector<double> positions;
        std::vector<std::string> labels;

        for (double mm = 0.0; mm <= lengthMm + 1e-9 * lengthMm; mm += step)
        {
            positions.push_back(mm / (gridSpacing * 1000.0));
            labels.push_back(std::format("{:g}", mm));
        }

        std::vector<const char*> labelPointers;

        for (const auto& label : labels)
            labelPointers.push_back(label.c_str());

        ImPlot3D::SetupAxisTicks(
            axis, positions.data(), (int)positions.size(), labelPointers.data());
    }

    double PlotsPanel::GetGridSpacing()
    {
        return _model->GetSimulationConfig()->Parameters.GridSpacing;
    }

    void PlotsPanel::CreateSurfacePlot(SurfacePlotState& state)
    {
        auto plotData = _model->GetRenderFrameData();

        const bool isResidual = state.Field == PlotField::RESIDUAL;
        const auto& T = plotData->GetField(state.Field);
        const char* valueLabel = isResidual ? "Residual" : "Temperature [K]";
        const char* plotId = isResidual ? "## Residual" : "## Temperature";

        const auto slice = CreateSliceSelector(T);

        ImPlot3DSurfaceFlags surfacePlotFlags = ImPlot3DSurfaceFlags_None;

        // Reset zoom button

        if (ImGui::Button("Reset zoom"))
        {
            state.ResetZoom = true;
        }

        // ColorMap combo box, hide lines and remove lines checkboxes

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        ImGui::Combo("Color map type",
                     &state.SelectedColorMap,
                     COLOR_MAP_OPTIONS,
                     IM_ARRAYSIZE(COLOR_MAP_OPTIONS));

        ImGui::SameLine();

        ImGui::Checkbox("Hide lines", &state.HideLinesOn);

        if (state.HideLinesOn)
        {
            surfacePlotFlags |= ImPlot3DSurfaceFlags_NoLines;
        }

        ImGui::SameLine();

        ImGui::Checkbox("Remove fill", &state.RemoveFillOn);

        if (state.RemoveFillOn)
        {
            surfacePlotFlags |= ImPlot3DSurfaceFlags_NoFill;
        }

        // Create surface plot

        ImPlot3D::PushColormap(state.SelectedColorMap);

        ImPlot3DFlags plot3DFlags = ImPlot3DFlags_NoPan;
        auto plotWindowWidth = ImGui::GetWindowSize().x;

        if (ImPlot3D::BeginPlot(plotId, ImVec2(plotWindowWidth * 0.93, -1), plot3DFlags))
        {
            ImPlot3D::SetupAxes(slice.ULabel, slice.VLabel, valueLabel);
            SetupLengthAxisTicks(ImAxis3D_X, slice.UMax, GetGridSpacing());
            SetupLengthAxisTicks(ImAxis3D_Y, slice.VMax, GetGridSpacing());

            if (state.ResetZoom)
            {
                ImPlot3D::SetupAxesLimits(slice.UMin,
                                          slice.UMax,
                                          slice.VMin,
                                          slice.VMax,
                                          slice.ValueMin,
                                          slice.ValueMax,
                                          ImPlot3DCond_Always);
                state.ResetZoom = false;
                ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Always);
            }
            else
            {
                const auto cond = state.FitToSlice ? ImPlot3DCond_Always : ImPlot3DCond_Once;
                state.FitToSlice = false;

                ImPlot3D::SetupAxesLimits(slice.UMin,
                                          slice.UMax,
                                          slice.VMin,
                                          slice.VMax,
                                          slice.ValueMin,
                                          slice.ValueMax,
                                          cond);
                ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Once);
            }

            ImPlot3DSpec spec;
            spec.FillAlpha = 1.0f;
            spec.Flags = surfacePlotFlags;
            spec.LineColor = ImPlot3D::GetColormapColor(1);

            ImPlot3D::PlotSurface("## Surface Plot",
                                  slice.U.data(),
                                  slice.V.data(),
                                  slice.Values.data(),
                                  slice.UCount,
                                  slice.VCount,
                                  0.0,
                                  0.0,
                                  spec);

            ImPlot3D::EndPlot();
        }

        ImPlot3D::PopColormap();

        // Create color bar

        ImGui::SameLine();
        ImPlot::PushColormap(state.SelectedColorMap);
        ImPlot::ColormapScale(valueLabel, slice.ValueMin, slice.ValueMax, ImVec2(-1, -1));
        ImPlot::PopColormap();
    }

    SliceData PlotsPanel::CreateSliceSelector(const Core::Tensor3d& field)
    {
        auto& state = *_sliceState;

        auto fitToSlice = [&]()
        {
            _temperatureSurfacePlotState->FitToSlice = true;
            _residualSurfacePlotState->FitToSlice = true;
        };

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        if (ImGui::Combo("Slice axis", &state.Axis, AXIS_OPTIONS, IM_ARRAYSIZE(AXIS_OPTIONS)))
        {
            fitToSlice();
        }

        const auto axis = (SliceAxis)state.Axis;
        int index = Slicer::ClampIndex(field, axis, state.Index);

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.4f);

        if (ImGui::SliderInt("Slice index", &index, 0, Slicer::AxisSize(field, axis) - 1))
        {
            state.Index = index;
            fitToSlice();
        }

        return Slicer::Slice(field, axis, index, SLICE_PLOT_POINTS);
    }

    void PlotsPanel::CreateConvergencePlots(ConvergencePlotsState& state)
    {
        using enum Core::TrackingMetric;

        auto plotData = _model->GetRenderFrameData();

        const bool isResidual = state.Metric == ConvergenceMetric::RESIDUAL;

        const auto& iterations = plotData->GetIterations().Values;
        const auto& maxValues = plotData->GetConvergence(state.Metric, MAX).Values;
        const auto& meanValues = plotData->GetConvergence(state.Metric, MEAN).Values;
        const auto& rmsValues = plotData->GetConvergence(state.Metric, RMS).Values;

        // Reset zoom

        if (ImGui::Button("Reset zoom"))
        {
            state.ResetZoom = true;
        }

        // Print history button

        if (ImGui::Button(isResidual ? "Print residual history" : "Print error history"))
        {
            _logger->AddSpace();
            _logger->Add(isResidual ? "================ Residuals ================"
                                    : "================ Errors ================");
            _logger->AddSpace();

            for (auto i = 0u; i < iterations.size(); i++)
            {
                _logger->Add(std::format("Iter {:4}  |  Max {:.4e}  |  Mean {:.4e}  |  RMS {:.4e}",
                                         (int)iterations[i],
                                         maxValues[i],
                                         meanValues[i],
                                         rmsValues[i]));
            }

            _logger->AddSpace();
            _logger->Add("========================================");
            _logger->AddSpace();
        }

        // Create line plots

        CreateConvergencePlot(state, MAX, "Max");
        CreateConvergencePlot(state, MEAN, "Mean");
        CreateConvergencePlot(state, RMS, "RMS");

        state.ResetZoom = false;
    }

    void PlotsPanel::CreateConvergencePlot(const ConvergencePlotsState& state,
                                           Core::TrackingMetric statistic,
                                           const std::string& statisticLabel)
    {
        auto plotData = _model->GetRenderFrameData();

        const auto& iterations = plotData->GetIterations();
        const auto& series = plotData->GetConvergence(state.Metric, statistic);

        const std::string metric =
            state.Metric == ConvergenceMetric::RESIDUAL ? "residual" : "error";

        const auto title = std::format("{} {}s against iterations", statisticLabel, metric);
        const auto yLabel = std::format("{} {}", statisticLabel, metric);
        const auto lineLabel = std::format("## {} {}", statisticLabel, metric);

        const ImPlotSpec lineSpec(ImPlotProp_LineWeight, 3.0f);

        if (ImPlot::BeginPlot(title.c_str()))
        {
            ImPlot::SetupAxes("Iterations", yLabel.c_str());

            if (state.ResetZoom)
            {
                ImPlot::SetupAxisLimits(
                    ImAxis_X1, iterations.Min, iterations.Max, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, series.Min, series.Max, ImPlotCond_Always);
            }

            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(lineLabel.c_str(),
                             iterations.Values.data(),
                             series.Values.data(),
                             iterations.Values.size(),
                             lineSpec);
            ImPlot::EndPlot();
        }
    }
}
