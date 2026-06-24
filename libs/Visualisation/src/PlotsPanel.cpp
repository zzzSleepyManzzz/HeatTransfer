#include "HeatTransfer/Visualisation/PlotsPanel.h"

namespace HeatTransfer::Visualisation
{
    PlotsPanel::PlotsPanel(GLFWwindow* window, std::shared_ptr<ConsoleLogger> logger)
        : IPanel(window, logger)
        , _surfacePlotState(std::make_shared<SurfacePlotState>())
        , _heatMapState(std::make_shared<HeatMapState>())
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
                    CreateSurfacePlot();

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
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }

    void PlotsPanel::CreateSurfacePlot()
    {
        auto plotData = _model->GetRenderFrameData();

        ImPlot3DSurfaceFlags surfacePlotFlags = ImPlot3DSurfaceFlags_None;

        // Reset zoom button

        if (ImGui::Button("Reset zoom"))
        {
            _surfacePlotState->resetZoom = true;
        }

        // ColorMap combo box, hide lines and remove lines checkboxes

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        ImGui::Combo("Color map type",
                     &_surfacePlotState->selectedColorMap,
                     COLOR_MAP_OPTIONS,
                     IM_ARRAYSIZE(COLOR_MAP_OPTIONS));

        ImGui::SameLine();

        if (ImGui::Checkbox("Hide lines", &_surfacePlotState->hideLinesOn))
        {
        }
        if (_surfacePlotState->hideLinesOn)
        {
            surfacePlotFlags |= ImPlot3DSurfaceFlags_NoLines;
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Remove fill", &_surfacePlotState->removeFillOn))
        {
        }
        if (_surfacePlotState->removeFillOn)
        {
            surfacePlotFlags |= ImPlot3DSurfaceFlags_NoFill;
        }

        // Create surface plot

        ImPlot3D::PushColormap(_surfacePlotState->selectedColorMap);

        ImPlot3DFlags plot3DFlags = ImPlot3DFlags_NoPan;
        auto plotWindowWidth = ImGui::GetWindowSize().x;

        if (ImPlot3D::BeginPlot("## Temperature", ImVec2(plotWindowWidth * 0.93, -1), plot3DFlags))
        {
            ImPlot3D::SetupAxes("Width [pixels]", "Length [pixels]", "Temperature [°C]");

            if (_surfacePlotState->resetZoom)
            {
                ImPlot3D::SetupAxesLimits(plotData->Get_X_Min(),
                                          plotData->Get_X_Max(),
                                          plotData->Get_Y_Min(),
                                          plotData->Get_Y_Max(),
                                          plotData->Get_Z_Min(),
                                          plotData->Get_Z_Max(),
                                          ImPlot3DCond_Always);
                _surfacePlotState->resetZoom = false;
                ImPlot3D::SetupBoxRotation(30, -45, true, ImPlot3DCond_Always);
            }
            else
            {
                ImPlot3D::SetupAxesLimits(plotData->Get_X_Min(),
                                          plotData->Get_X_Max(),
                                          plotData->Get_Y_Min(),
                                          plotData->Get_Y_Max(),
                                          plotData->Get_Z_Min(),
                                          plotData->Get_Z_Max(),
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
                                  plotData->Get_Z_Values().data(),
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
        ImPlot::PushColormap(_surfacePlotState->selectedColorMap);
        ImPlot::ColormapScale(
            "Temperature [°C]", plotData->Get_Z_Min(), plotData->Get_Z_Max(), ImVec2(-1, -1));
        ImPlot::PopColormap();
    }

    void PlotsPanel::CreateHeatMap()
    {
        auto data = _model->GetRenderFrameData();

        // Color map combo box

        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.25f);

        ImGui::Combo("Color map type",
                     &_heatMapState->selectedColorMap,
                     COLOR_MAP_OPTIONS,
                     IM_ARRAYSIZE(COLOR_MAP_OPTIONS));

        // Create heat map

        ImPlot::PushColormap(_heatMapState->selectedColorMap);

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
                                data->Get_Z_Values().data(),
                                data->GetRows(),
                                data->GetCols(),
                                data->Get_Z_Min(),
                                data->Get_Z_Max(),
                                nullptr,
                                ImPlotPoint(data->Get_X_Min(), data->Get_Y_Min()),
                                ImPlotPoint(data->Get_X_Max(), data->Get_Y_Max()));

            ImPlot::EndPlot();
        }

        // Create color bar

        ImGui::SameLine();
        ImPlot::ColormapScale(
            "Temperature [°C]", data->Get_Z_Min(), data->Get_Z_Max(), ImVec2(-1, -1));
        ImPlot::PopColormap();
    }

    void PlotsPanel::CreateErrorPlots()
    {
        auto plotData = _model->GetRenderFrameData();

        const auto& iterations = plotData->Get_Iterations();
        const auto& maxErrors = plotData->Get_MaxErrors();
        const auto& meanErrors = plotData->Get_MeanErrors();
        const auto& rmsErrors = plotData->Get_RMS_Errors();

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
            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(
                "## Max Error", iterations.data(), maxErrors.data(), iterations.size());
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("Mean errors against iteration"))
        {
            ImPlot::SetupAxes("Iterations", "Mean error");
            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(
                "## Max Error", iterations.data(), meanErrors.data(), iterations.size());
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("RMS errors against iteration"))
        {
            ImPlot::SetupAxes("Iterations", "RMS error");
            ImPlot::SetupAxisScale(ImAxis_Y1, ImPlotScale_Log10);
            ImPlot::PlotLine(
                "## Max Error", iterations.data(), rmsErrors.data(), iterations.size());
            ImPlot::EndPlot();
        }

        ImPlot::PopStyleVar();
    }
}