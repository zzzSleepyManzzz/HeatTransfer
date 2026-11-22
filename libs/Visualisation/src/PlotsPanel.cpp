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
        , _residualVolumeViewState(
              std::make_shared<VolumeViewState>(VolumeViewState{.Field = VolumeField::RESIDUAL}))
        , _volumeScaleState(std::make_shared<VolumeScaleState>())
        , _sliceState(std::make_shared<SliceState>())
        , _temperatureSurfacePlotState(std::make_shared<TemperatureSurfacePlotState>())
        , _residualSurfacePlotState(std::make_shared<ResidualSurfacePlotState>())
        , _errorConvergencePlotsState(std::make_shared<ErrorConvergencePlotsState>())
        , _residualConvergencePlotsState(std::make_shared<ResidualConvergencePlotsState>())
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
        _residualVolumeViewState->ResetZoom = true;
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
                    CreateVolumeView(*_volumeViewState);

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("3D Residual"))
                {
                    CreateVolumeView(*_residualVolumeViewState);

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Temperature surface"))
                {
                    CreateTemperatureSurfacePlot();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Residual surface"))
                {
                    CreateResidualSurfacePlot();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Error convergence"))
                {
                    CreateErrorConvergencePlots();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Residual convergence"))
                {
                    CreateResidualConvergencePlots();

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void PlotsPanel::CreateTemperatureSurfacePlot()
    {
        auto plotData = _model->GetRenderFrameData();
        const auto slice = CreateSliceSelector(plotData->GetTemperature());

        ImPlot3DSurfaceFlags surfacePlotFlags = ImPlot3DSurfaceFlags_None;

        // Reset zoom button

        if (ImGui::Button("Reset zoom"))
        {
            _temperatureSurfacePlotState->ResetZoom = true;
        }

        // ColorMap combo box, hide lines and remove lines checkboxes

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        ImGui::Combo("Color map type",
                     &_temperatureSurfacePlotState->SelectedColorMap,
                     COLOR_MAP_OPTIONS,
                     IM_ARRAYSIZE(COLOR_MAP_OPTIONS));

        ImGui::SameLine();

        if (ImGui::Checkbox("Hide lines", &_temperatureSurfacePlotState->HideLinesOn))
        {
        }
        if (_temperatureSurfacePlotState->HideLinesOn)
        {
            surfacePlotFlags |= ImPlot3DSurfaceFlags_NoLines;
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Remove fill", &_temperatureSurfacePlotState->RemoveFillOn))
        {
        }
        if (_temperatureSurfacePlotState->RemoveFillOn)
        {
            surfacePlotFlags |= ImPlot3DSurfaceFlags_NoFill;
        }

        // Create surface plot

        ImPlot3D::PushColormap(_temperatureSurfacePlotState->SelectedColorMap);

        ImPlot3DFlags plot3DFlags = ImPlot3DFlags_NoPan;
        auto plotWindowWidth = ImGui::GetWindowSize().x;

        if (ImPlot3D::BeginPlot("## Temperature", ImVec2(plotWindowWidth * 0.93, -1), plot3DFlags))
        {
            ImPlot3D::SetupAxes(slice.ULabel, slice.VLabel, "Temperature [K]");
            SetupLengthAxisTicks(ImAxis3D_X, slice.UMax, GetGridSpacing());
            SetupLengthAxisTicks(ImAxis3D_Y, slice.VMax, GetGridSpacing());

            if (_temperatureSurfacePlotState->ResetZoom)
            {
                ImPlot3D::SetupAxesLimits(slice.UMin,
                                          slice.UMax,
                                          slice.VMin,
                                          slice.VMax,
                                          slice.ValueMin,
                                          slice.ValueMax,
                                          ImPlot3DCond_Always);
                _temperatureSurfacePlotState->ResetZoom = false;
                ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Always);
            }
            else
            {
                const auto cond = _temperatureSurfacePlotState->FitToSlice ? ImPlot3DCond_Always
                                                                           : ImPlot3DCond_Once;
                _temperatureSurfacePlotState->FitToSlice = false;

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

            ImPlot3D::PlotSurface("## Temperature Surface Plot",
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
        ImPlot::PushColormap(_temperatureSurfacePlotState->SelectedColorMap);
        ImPlot::ColormapScale("Temperature [K]", slice.ValueMin, slice.ValueMax, ImVec2(-1, -1));
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

    void PlotsPanel::CreateResidualSurfacePlot()
    {
        auto plotData = _model->GetRenderFrameData();
        const auto slice = CreateSliceSelector(plotData->GetResidual());

        ImPlot3DSurfaceFlags surfacePlotFlags = ImPlot3DSurfaceFlags_None;

        // Reset zoom button

        if (ImGui::Button("Reset zoom"))
        {
            _residualSurfacePlotState->ResetZoom = true;
        }

        // ColorMap combo box, hide lines and remove lines checkboxes

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        ImGui::Combo("Color map type",
                     &_residualSurfacePlotState->SelectedColorMap,
                     COLOR_MAP_OPTIONS,
                     IM_ARRAYSIZE(COLOR_MAP_OPTIONS));

        ImGui::SameLine();

        if (ImGui::Checkbox("Hide lines", &_residualSurfacePlotState->HideLinesOn))
        {
        }
        if (_residualSurfacePlotState->HideLinesOn)
        {
            surfacePlotFlags |= ImPlot3DSurfaceFlags_NoLines;
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Remove fill", &_residualSurfacePlotState->RemoveFillOn))
        {
        }
        if (_residualSurfacePlotState->RemoveFillOn)
        {
            surfacePlotFlags |= ImPlot3DSurfaceFlags_NoFill;
        }

        // Create surface plot

        ImPlot3D::PushColormap(_residualSurfacePlotState->SelectedColorMap);

        ImPlot3DFlags plot3DFlags = ImPlot3DFlags_NoPan;
        auto plotWindowWidth = ImGui::GetWindowSize().x;

        if (ImPlot3D::BeginPlot("## Temperature", ImVec2(plotWindowWidth * 0.93, -1), plot3DFlags))
        {
            ImPlot3D::SetupAxes(slice.ULabel, slice.VLabel, "Residual");
            SetupLengthAxisTicks(ImAxis3D_X, slice.UMax, GetGridSpacing());
            SetupLengthAxisTicks(ImAxis3D_Y, slice.VMax, GetGridSpacing());

            if (_residualSurfacePlotState->ResetZoom)
            {
                ImPlot3D::SetupAxesLimits(slice.UMin,
                                          slice.UMax,
                                          slice.VMin,
                                          slice.VMax,
                                          slice.ValueMin,
                                          slice.ValueMax,
                                          ImPlot3DCond_Always);
                _residualSurfacePlotState->ResetZoom = false;
                ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Always);
            }
            else
            {
                const auto cond =
                    _residualSurfacePlotState->FitToSlice ? ImPlot3DCond_Always : ImPlot3DCond_Once;
                _residualSurfacePlotState->FitToSlice = false;

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

            ImPlot3D::PlotSurface("## Residual Surface Plot",
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
        ImPlot::PushColormap(_residualSurfacePlotState->SelectedColorMap);
        ImPlot::ColormapScale("Residual", slice.ValueMin, slice.ValueMax, ImVec2(-1, -1));
        ImPlot::PopColormap();
    }

    void PlotsPanel::CreateErrorConvergencePlots()
    {
        auto plotData = _model->GetRenderFrameData();

        const auto& iterations = plotData->Get_Iterations();
        const auto& maxErrors = plotData->Get_MaxErrors();
        const auto& meanErrors = plotData->Get_MeanErrors();
        const auto& rmsErrors = plotData->Get_RMS_Errors();

        auto iterations_min = plotData->Get_Iterations_Min();
        auto iterations_max = plotData->Get_Iterations_Max();

        auto maxErrors_min = plotData->Get_MaxErrors_Min();
        auto maxErrors_max = plotData->Get_MaxErrors_Max();

        auto meanErrors_min = plotData->Get_MeanErrors_Min();
        auto meanErrors_max = plotData->Get_MeanErrors_Max();

        auto rmsErrors_min = plotData->Get_RMS_Errors_Min();
        auto rmsErrors_max = plotData->Get_RMS_Errors_Max();

        // Reset zoom

        if (ImGui::Button("Reset zoom"))
        {
            _errorConvergencePlotsState->ResetZoom = true;
        }

        // Print error history button

        if (ImGui::Button("Print error history"))
        {
            _logger->AddSpace();
            _logger->Add("================ Errors ================");
            _logger->AddSpace();

            for (auto i = 0u; i < iterations.size(); i++)
            {
                _logger->Add(std::format("Iter {:4}  |  Max {:.4e}  |  Mean {:.4e}  |  RMS {:.4e}",
                                         (int)iterations[i],
                                         maxErrors[i],
                                         meanErrors[i],
                                         rmsErrors[i]));
            }

            _logger->AddSpace();
            _logger->Add("========================================");
            _logger->AddSpace();
        }

        // Create line plots

        const ImPlotSpec lineSpec(ImPlotProp_LineWeight, 3.0f);

        if (ImPlot::BeginPlot("Max errors against iterations"))
        {
            ImPlot::SetupAxes("Iterations", "Max error");

            if (_errorConvergencePlotsState->ResetZoom)
            {
                ImPlot::SetupAxisLimits(
                    ImAxis_X1, iterations_min, iterations_max, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, maxErrors_min, maxErrors_max, ImPlotCond_Always);
            }

            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(
                "## Max Error", iterations.data(), maxErrors.data(), iterations.size(), lineSpec);
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("Mean errors against iteration"))
        {
            ImPlot::SetupAxes("Iterations", "Mean error");

            if (_errorConvergencePlotsState->ResetZoom)
            {
                ImPlot::SetupAxisLimits(
                    ImAxis_X1, iterations_min, iterations_max, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(
                    ImAxis_Y1, meanErrors_min, meanErrors_max, ImPlotCond_Always);
            }

            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(
                "## Mean Error", iterations.data(), meanErrors.data(), iterations.size(), lineSpec);
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("RMS errors against iteration"))
        {
            ImPlot::SetupAxes("Iterations", "RMS error");

            if (_errorConvergencePlotsState->ResetZoom)
            {
                ImPlot::SetupAxisLimits(
                    ImAxis_X1, iterations_min, iterations_max, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, rmsErrors_min, rmsErrors_max, ImPlotCond_Always);
            }

            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(
                "## RMS Error", iterations.data(), rmsErrors.data(), iterations.size(), lineSpec);
            ImPlot::EndPlot();
        }

        _errorConvergencePlotsState->ResetZoom = false;
    }

    void PlotsPanel::CreateResidualConvergencePlots()
    {
        auto plotData = _model->GetRenderFrameData();

        const auto& iterations = plotData->Get_Iterations();
        const auto& maxResiduals = plotData->Get_MaxResiduals();
        const auto& meanResiduals = plotData->Get_MeanResiduals();
        const auto& rmsResiduals = plotData->Get_RMS_Residuals();

        auto iterations_min = plotData->Get_Iterations_Min();
        auto iterations_max = plotData->Get_Iterations_Max();

        auto maxResiduals_min = plotData->Get_MaxResiduals_Min();
        auto maxResiduals_max = plotData->Get_MaxResiduals_Max();

        auto meanResiduals_min = plotData->Get_MeanResiduals_Min();
        auto meanResiduals_max = plotData->Get_MeanResiduals_Max();

        auto rmsResiduals_min = plotData->Get_RMS_Residuals_Min();
        auto rmsResiduals_max = plotData->Get_RMS_Residuals_Max();

        // Reset zoom

        if (ImGui::Button("Reset zoom"))
        {
            _residualConvergencePlotsState->ResetZoom = true;
        }

        // Print residual metrics history button

        if (ImGui::Button("Print residual history"))
        {
            _logger->AddSpace();
            _logger->Add("================ Residuals ================");
            _logger->AddSpace();

            for (auto i = 0u; i < iterations.size(); i++)
            {
                _logger->Add(std::format("Iter {:4}  |  Max {:.4e}  |  Mean {:.4e}  |  RMS {:.4e}",
                                         (int)iterations[i],
                                         maxResiduals[i],
                                         meanResiduals[i],
                                         rmsResiduals[i]));
            }

            _logger->AddSpace();
            _logger->Add("========================================");
            _logger->AddSpace();
        }

        // Create line plots

        const ImPlotSpec lineSpec(ImPlotProp_LineWeight, 3.0f);

        if (ImPlot::BeginPlot("Max residual against iterations"))
        {
            ImPlot::SetupAxes("Iterations", "Max residual");

            if (_residualConvergencePlotsState->ResetZoom)
            {
                ImPlot::SetupAxisLimits(
                    ImAxis_X1, iterations_min, iterations_max, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(
                    ImAxis_Y1, maxResiduals_min, maxResiduals_max, ImPlotCond_Always);
            }

            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine("## Max Residual",
                             iterations.data(),
                             maxResiduals.data(),
                             iterations.size(),
                             lineSpec);
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("Mean residuals against iteration"))
        {
            ImPlot::SetupAxes("Iterations", "Mean residual");

            if (_residualConvergencePlotsState->ResetZoom)
            {
                ImPlot::SetupAxisLimits(
                    ImAxis_X1, iterations_min, iterations_max, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(
                    ImAxis_Y1, meanResiduals_min, meanResiduals_max, ImPlotCond_Always);
            }

            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine("## Mean Residual",
                             iterations.data(),
                             meanResiduals.data(),
                             iterations.size(),
                             lineSpec);
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("RMS residuals against iteration"))
        {
            ImPlot::SetupAxes("Iterations", "RMS residual");

            if (_residualConvergencePlotsState->ResetZoom)
            {
                ImPlot::SetupAxisLimits(
                    ImAxis_X1, iterations_min, iterations_max, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(
                    ImAxis_Y1, rmsResiduals_min, rmsResiduals_max, ImPlotCond_Always);
            }

            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine("## RMS Residual",
                             iterations.data(),
                             rmsResiduals.data(),
                             iterations.size(),
                             lineSpec);
            ImPlot::EndPlot();
        }

        _residualConvergencePlotsState->ResetZoom = false;
    }

    void PlotsPanel::CreateVolumeView(VolumeViewState& state)
    {
        auto data = _model->GetRenderFrameData();

        const bool isResidual = state.Field == VolumeField::RESIDUAL;
        const auto& T = isResidual ? data->GetResidual() : data->GetTemperature();

        // Reset view button (rotation), view scale, color map and display options

        if (ImGui::Button("Reset view"))
        {
            state.ResetZoom = true;
        }

        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        ImGui::SliderFloat("View scale", &_volumeScaleState->ViewScale, 0.5f, 3.0f, "%.2f");

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        ImGui::Combo("Color map type",
                     &state.SelectedColorMap,
                     COLOR_MAP_OPTIONS,
                     IM_ARRAYSIZE(COLOR_MAP_OPTIONS));

        ImGui::SameLine();
        ImGui::Checkbox("Show lines", &state.ShowLines);

        ImGui::SameLine();
        ImGui::Checkbox("Show surface", &state.ShowOuterFaces);

        // One checkbox and slider per slicing plane

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

        RebuildVolumeMeshIfNeeded(state);

        // Create mesh plot

        const double xMax = std::max(1.0, (double)Slicer::AxisSize(T, SliceAxis::X) - 1);
        const double yMax = std::max(1.0, (double)Slicer::AxisSize(T, SliceAxis::Y) - 1);
        const double zMax = std::max(1.0, (double)Slicer::AxisSize(T, SliceAxis::Z) - 1);
        const double longest = std::max({xMax, yMax, zMax});

        ImPlot3D::PushColormap(state.SelectedColorMap);
        ImPlot3D::PushStyleVar(ImPlot3DStyleVar_ViewScaleFactor, _volumeScaleState->ViewScale);

        auto plotWindowWidth = ImGui::GetWindowSize().x;

        // Each field gets its own plot, so each keeps its own rotation and zoom
        if (ImPlot3D::BeginPlot(isResidual ? "## Residual Volume" : "## Temperature Volume",
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

            for (std::size_t i = 0; i < state.FaceMeshes.size(); i++)
            {
                const bool facesCamera = (rotation * outwardNormals[i]).z > 0.0;

                if (facesCamera)
                    plotMesh(FACE_LABELS[i], state.FaceMeshes[i]);
            }

            plotMesh("##Slices", state.SliceMesh);

            ImPlot3D::EndPlot();
        }

        ImPlot3D::PopStyleVar();
        ImPlot3D::PopColormap();

        // Create color bar

        ImGui::SameLine();
        ImPlot::PushColormap(state.SelectedColorMap);

        if (isResidual)
            ImPlot::ColormapScale(
                "Residual", data->Get_Residual_Min(), data->Get_Residual_Max(), ImVec2(-1, -1));
        else
            ImPlot::ColormapScale("Temperature [K]",
                                  data->Get_Temperature_Min(),
                                  data->Get_Temperature_Max(),
                                  ImVec2(-1, -1));

        ImPlot::PopColormap();
    }

    void PlotsPanel::RebuildVolumeMeshIfNeeded(VolumeViewState& state)
    {
        auto data = _model->GetRenderFrameData();

        const bool isResidual = state.Field == VolumeField::RESIDUAL;
        const auto& T = isResidual ? data->GetResidual() : data->GetTemperature();

        std::array<int, 3> planeIndex;

        for (int axis = 0; axis < 3; axis++)
            planeIndex[axis] = Slicer::ClampIndex(T, (SliceAxis)axis, state.PlaneIndex[axis]);

        const bool upToDate = state.MeshSource == data && state.MeshShowPlane == state.ShowPlane &&
                              state.MeshPlaneIndex == planeIndex &&
                              state.MeshShowOuterFaces == state.ShowOuterFaces &&
                              state.MeshColorMap == state.SelectedColorMap;

        if (upToDate)
            return;

        ZoneScoped;

        const double tMin = isResidual ? data->Get_Residual_Min() : data->Get_Temperature_Min();
        const double tMax = isResidual ? data->Get_Residual_Max() : data->Get_Temperature_Max();
        const int colormap = state.SelectedColorMap;

        auto slice = [&](SliceAxis axis, int index)
        { return Slicer::Slice(T, axis, index, VOLUME_PLANE_POINTS); };

        state.SliceMesh.Clear();

        for (int axis = 0; axis < 3; axis++)
        {
            if (state.ShowPlane[axis])
                state.SliceMesh.AddSlice(
                    slice((SliceAxis)axis, planeIndex[axis]), tMin, tMax, colormap);
        }

        // The outer faces, ordered as Core::Faces::ALL (left, right, top, bottom, front, back)
        const std::array<std::pair<SliceAxis, int>, 6> faces = {
            std::pair{SliceAxis::X, 0},
            std::pair{SliceAxis::X, Slicer::AxisSize(T, SliceAxis::X) - 1},
            std::pair{SliceAxis::Z, Slicer::AxisSize(T, SliceAxis::Z) - 1},
            std::pair{SliceAxis::Z, 0},
            std::pair{SliceAxis::Y, 0},
            std::pair{SliceAxis::Y, Slicer::AxisSize(T, SliceAxis::Y) - 1}};

        for (std::size_t i = 0; i < faces.size(); i++)
        {
            state.FaceMeshes[i].Clear();

            if (state.ShowOuterFaces)
                state.FaceMeshes[i].AddSlice(
                    slice(faces[i].first, faces[i].second), tMin, tMax, colormap);
        }

        state.MeshSource = data;
        state.MeshShowPlane = state.ShowPlane;
        state.MeshPlaneIndex = planeIndex;
        state.MeshShowOuterFaces = state.ShowOuterFaces;
        state.MeshColorMap = state.SelectedColorMap;
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

        // ImPlot3D copies the labels, so the temporary strings are fine
        ImPlot3D::SetupAxisTicks(
            axis, positions.data(), (int)positions.size(), labelPointers.data());
    }

    double PlotsPanel::GetGridSpacing()
    {
        return _model->GetSimulationConfig()->Parameters.GridSpacing;
    }
}
