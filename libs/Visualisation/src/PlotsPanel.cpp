#include "HeatTransfer/Visualisation/PlotsPanel.h"

namespace HeatTransfer::Visualisation
{
    PlotsPanel::PlotsPanel(std::shared_ptr<ConsoleLogger> logger)
        : IPanel(logger)
        , _temperatureSurfacePlotState(std::make_shared<SurfacePlotState>())
        , _residualSurfacePlotState(std::make_shared<ResidualSurfacePlotState>())
        , _heatMapState(std::make_shared<HeatMapState>())
        , _errorPlotsState(std::make_shared<ErrorPlotsState>())
        , _residualConvergencePlotsState(std::make_shared<ResidualConvergencePlotsState>())
    {
    }

    void PlotsPanel::Render(std::shared_ptr<RendererModel> model)
    {
        _model = model;
        ShowPlotsWindow();
    }

    void PlotsPanel::ShowPlotsWindow()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x * 4.0f / 5.0f, displaySize.y * 3.0f / 4.0f));
        ImGui::SetNextWindowPos(ImVec2(displaySize.x / 5.0f, 0));

        ImGui::Begin("Plots", nullptr, windowFlags);
        {
            ImGui::Dummy(ImVec2(0.0f, 10.0f));
            if (ImGui::BeginTabBar("Plot Tabs"))
            {
                if (ImGui::BeginTabItem("Surface Plot"))
                {
                    CreateTemperatureSurfacePlot();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Heat Map"))
                {
                    CreateHeatMap();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Error plot"))
                {
                    CreateErrorPlots();

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Residual plot"))
                {
                    CreateResidualSurfacePlot();

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
            ImPlot3D::SetupAxes("Width [pixels]", "Length [pixels]", "Temperature [°C]");

            if (_temperatureSurfacePlotState->ResetZoom)
            {
                ImPlot3D::SetupAxesLimits(plotData->Get_X_Min(),
                                          plotData->Get_X_Max(),
                                          plotData->Get_Y_Min(),
                                          plotData->Get_Y_Max(),
                                          plotData->Get_Temperature_Min(),
                                          plotData->Get_Temperature_Max(),
                                          ImPlot3DCond_Always);
                _temperatureSurfacePlotState->ResetZoom = false;
                ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Always);
            }
            else
            {
                ImPlot3D::SetupAxesLimits(plotData->Get_X_Min(),
                                          plotData->Get_X_Max(),
                                          plotData->Get_Y_Min(),
                                          plotData->Get_Y_Max(),
                                          plotData->Get_Temperature_Min(),
                                          plotData->Get_Temperature_Max(),
                                          ImPlot3DCond_Once);
                ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Once);
            }

            ImPlot3DSpec spec;
            spec.FillAlpha = 1.0f;
            spec.Flags = surfacePlotFlags;
            spec.LineColor = ImPlot3D::GetColormapColor(1);

            ImPlot3D::PlotSurface("## Temperature Surface Plot",
                                  plotData->Get_X_Values().data(),
                                  plotData->Get_Y_Values().data(),
                                  plotData->Get_Temperature_Values().data(),
                                  plotData->GetCols(),
                                  plotData->GetRows(),
                                  0.0,
                                  0.0,
                                  spec);

            ImPlot3D::EndPlot();
        }

        ImPlot3D::PopColormap();

        // Create color bar

        ImGui::SameLine();
        ImPlot::PushColormap(_temperatureSurfacePlotState->SelectedColorMap);
        ImPlot::ColormapScale("Temperature [°C]",
                              plotData->Get_Temperature_Min(),
                              plotData->Get_Temperature_Max(),
                              ImVec2(-1, -1));
        ImPlot::PopColormap();
    }

    void PlotsPanel::CreateResidualSurfacePlot()
    {
        auto plotData = _model->GetRenderFrameData();

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
            ImPlot3D::SetupAxes("Width [pixels]", "Length [pixels]", "Residual");

            if (_residualSurfacePlotState->ResetZoom)
            {
                ImPlot3D::SetupAxesLimits(plotData->Get_X_Min(),
                                          plotData->Get_X_Max(),
                                          plotData->Get_Y_Min(),
                                          plotData->Get_Y_Max(),
                                          plotData->Get_Residual_Min(),
                                          plotData->Get_Residual_Max(),
                                          ImPlot3DCond_Always);
                _residualSurfacePlotState->ResetZoom = false;
                ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Always);
            }
            else
            {
                ImPlot3D::SetupAxesLimits(plotData->Get_X_Min(),
                                          plotData->Get_X_Max(),
                                          plotData->Get_Y_Min(),
                                          plotData->Get_Y_Max(),
                                          plotData->Get_Residual_Min(),
                                          plotData->Get_Residual_Max(),
                                          ImPlot3DCond_Once);
                ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Once);
            }

            ImPlot3DSpec spec;
            spec.FillAlpha = 1.0f;
            spec.Flags = surfacePlotFlags;
            spec.LineColor = ImPlot3D::GetColormapColor(1);

            ImPlot3D::PlotSurface("## Residual Surface Plot",
                                  plotData->Get_X_Values().data(),
                                  plotData->Get_Y_Values().data(),
                                  plotData->Get_Residual_Values().data(),
                                  plotData->GetCols(),
                                  plotData->GetRows(),
                                  0.0,
                                  0.0,
                                  spec);

            ImPlot3D::EndPlot();
        }

        ImPlot3D::PopColormap();

        // Create color bar

        ImGui::SameLine();
        ImPlot::PushColormap(_residualSurfacePlotState->SelectedColorMap);
        ImPlot::ColormapScale(
            "Residual", plotData->Get_Residual_Min(), plotData->Get_Residual_Max(), ImVec2(-1, -1));
        ImPlot::PopColormap();
    }

    void PlotsPanel::CreateHeatMap()
    {
        auto data = _model->GetRenderFrameData();

        // Color map combo box

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        ImGui::Combo("Color map type",
                     &_heatMapState->SelectedColorMap,
                     COLOR_MAP_OPTIONS,
                     IM_ARRAYSIZE(COLOR_MAP_OPTIONS));

        // Create heat map

        ImPlot::PushColormap(_heatMapState->SelectedColorMap);

        auto plotWindowWidth = ImGui::GetWindowSize().x;

        if (ImPlot::BeginPlot(
                "Temperature Heat Map", ImVec2(plotWindowWidth * 0.93, -1), ImPlotFlags_NoLegend))
        {
            ImPlot::SetupAxes(
                "Length [pixels]", "Width [pixels]", ImPlotAxisFlags_None, ImPlotAxisFlags_None);
            ImPlot::SetupAxesLimits(data->Get_X_Min(),
                                    data->Get_X_Max(),
                                    data->Get_Y_Min(),
                                    data->Get_Y_Max(),
                                    ImPlotCond_Always);

            ImPlot::PlotHeatmap("Plotted Heat Map",
                                data->Get_Temperature_Values().data(),
                                data->GetRows(),
                                data->GetCols(),
                                data->Get_Temperature_Min(),
                                data->Get_Temperature_Max(),
                                nullptr,
                                ImPlotPoint(data->Get_X_Min(), data->Get_Y_Min()),
                                ImPlotPoint(data->Get_X_Max(), data->Get_Y_Max()));

            ImPlot::EndPlot();
        }

        // Create color bar

        ImGui::SameLine();
        ImPlot::ColormapScale("Temperature [°C]",
                              data->Get_Temperature_Min(),
                              data->Get_Temperature_Max(),
                              ImVec2(-1, -1));
        ImPlot::PopColormap();
    }

    void PlotsPanel::CreateErrorPlots()
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
            _errorPlotsState->ResetZoom = true;
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

        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3.0f);

        if (ImPlot::BeginPlot("Max errors against iterations"))
        {
            ImPlot::SetupAxes("Iterations", "Max error");

            if (_errorPlotsState->ResetZoom)
            {
                ImPlot::SetupAxisLimits(
                    ImAxis_X1, iterations_min, iterations_max, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, maxErrors_min, maxErrors_max, ImPlotCond_Always);
            }

            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(
                "## Max Error", iterations.data(), maxErrors.data(), iterations.size());
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("Mean errors against iteration"))
        {
            ImPlot::SetupAxes("Iterations", "Mean error");

            if (_errorPlotsState->ResetZoom)
            {
                ImPlot::SetupAxisLimits(
                    ImAxis_X1, iterations_min, iterations_max, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(
                    ImAxis_Y1, meanErrors_min, meanErrors_max, ImPlotCond_Always);
            }

            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(
                "## Mean Error", iterations.data(), meanErrors.data(), iterations.size());
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("RMS errors against iteration"))
        {
            ImPlot::SetupAxes("Iterations", "RMS error");

            if (_errorPlotsState->ResetZoom)
            {
                ImPlot::SetupAxisLimits(
                    ImAxis_X1, iterations_min, iterations_max, ImPlotCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, rmsErrors_min, rmsErrors_max, ImPlotCond_Always);
            }

            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(
                "## RMS Error", iterations.data(), rmsErrors.data(), iterations.size());
            ImPlot::EndPlot();
        }

        _errorPlotsState->ResetZoom = false;

        ImPlot::PopStyleVar();
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
            _logger->Add("================ Residual Metrics ================");
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

        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 3.0f);

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
            ImPlot::PlotLine(
                "## Max Residual", iterations.data(), maxResiduals.data(), iterations.size());
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
            ImPlot::PlotLine(
                "## Mean Residual", iterations.data(), meanResiduals.data(), iterations.size());
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
            ImPlot::PlotLine(
                "## RMS Residual", iterations.data(), rmsResiduals.data(), iterations.size());
            ImPlot::EndPlot();
        }

        _residualConvergencePlotsState->ResetZoom = false;

        ImPlot::PopStyleVar();
    }
}