#include "HeatTransfer/Visualisation/SidePanel.h"

namespace HeatTransfer::Visualisation
{
    SidePanel::SidePanel(std::shared_ptr<ConsoleLogger> logger)
        : IPanel(logger), _settingsState(std::make_shared<SettingsState>())
    {
    }

    void SidePanel::Render(std::shared_ptr<RendererModel> model)
    {
        _model = model;
        ShowSideBar();
    }

    void SidePanel::ShowSideBar()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x / 5.0f, displaySize.y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("Side Bar", nullptr, windowFlags);
        {
            AddSettings();
            AddStatistics();
        }
        ImGui::End();
    }

    void SidePanel::AddSettings()
    {
        if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            // Light mode check box

            ImGui::Text("Turn light mode on");

            ImGui::SameLine();

            if (ImGui::Checkbox("##Turn light mode on", &_settingsState->lightModeOn))
            {
                if (_settingsState->lightModeOn)
                    ImGui::StyleColorsLight();
                else
                    ImGui::StyleColorsDark();
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            // Iterative method combo box

            if (ImGui::Combo("## Iterative Method",
                             &_settingsState->iterativeMethod,
                             METHOD_OPTIONS,
                             IM_ARRAYSIZE(METHOD_OPTIONS)))
            {
                _model->GetSimulationConfig()->method =
                    METHOD_ARRAY[_settingsState->iterativeMethod];

                _model->UpdateModel();
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            // Add simulation parameters and boundary conditions button

            AddParametersButton();
            AddBoundaryConditionsButton();
        }
    }

    void SidePanel::AddParametersButton()
    {
        if (ImGui::Button("Simulation Parameters"))
        {
            ImGui::OpenPopup("Modify simulation parameters");
        }

        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x * 1.0f / 3.0f, displaySize.y * 2.0f / 3.0f));

        ImGui::SetNextWindowPos(
            ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(
                "Modify simulation parameters", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Set new simulation parameters here");
            ImGui::Text("Pressing OK will start a new run");

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Separator();

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            if (ImGui::InputInt("Number of rows", &_settingsState->temporaryParameters.rows))
            {
            }
            if (ImGui::InputInt("Number of columns", &_settingsState->temporaryParameters.columns))
            {
            }
            if (ImGui::InputInt("Expansion factor", &_settingsState->temporaryParameters.expansion))
            {
            }
            if (ImGui::InputInt("Num of expansions",
                                &_settingsState->temporaryParameters.numExpansions))
            {
            }
            if (ImGui::InputDouble("Initial tolerance",
                                   &_settingsState->temporaryParameters.initialTolerance))
            {
            }
            if (ImGui::InputDouble("Expanded tolerance",
                                   &_settingsState->temporaryParameters.expandedTolerance))
            {
            }
            if (ImGui::InputDouble("Relaxation factor",
                                   &_settingsState->temporaryParameters.relaxationFactor))
            {
            }
            if (ImGui::InputInt("Max initial iterations",
                                &_settingsState->temporaryParameters.maxInitialIterations))
            {
            }
            if (ImGui::InputInt("Max expanded iterations",
                                &_settingsState->temporaryParameters.maxExpandedIterations))
            {
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            if (ImGui::Button("OK", ImVec2(displaySize.x * 1.0f / 6.0f, 0)))
            {
                _model->GetSimulationConfig()->parameters = _settingsState->temporaryParameters;
                _model->UpdateModel();

                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(-1, 0)))
            {
                _settingsState->temporaryParameters = _model->GetSimulationConfig()->parameters;

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
    }

    void SidePanel::AddBoundaryConditionsButton()
    {
        if (ImGui::Button("Boundary Conditions"))
        {
            ImGui::OpenPopup("Modify boundary conditions");
        }

        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x * 1.0f / 3.0f, displaySize.y * 2.0f / 3.0f));

        ImGui::SetNextWindowPos(
            ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(
                "Modify boundary conditions", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Set new boundary conditions here");
            ImGui::Text("Pressing OK will start a new run");

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Separator();

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            if (ImGui::InputDouble("Top edge temperature",
                                   &_settingsState->temporaryBoundaryConditions.topEdge))
            {
            }
            if (ImGui::InputDouble("Bottom edge temperature",
                                   &_settingsState->temporaryBoundaryConditions.bottomEdge))
            {
            }
            if (ImGui::InputDouble("Left edge temperature",
                                   &_settingsState->temporaryBoundaryConditions.leftEdge))
            {
            }
            if (ImGui::InputDouble("Right edge temperature",
                                   &_settingsState->temporaryBoundaryConditions.rightEdge))
            {
            }
            if (ImGui::InputDouble("Inner square temperature",
                                   &_settingsState->temporaryBoundaryConditions.innerSquare))
            {
            }
            if (ImGui::InputDouble("Center point temperature",
                                   &_settingsState->temporaryBoundaryConditions.centerPoint))
            {
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            if (ImGui::Button("OK", ImVec2(displaySize.x * 1.0f / 6.0f, 0)))
            {
                _model->GetSimulationConfig()->boundaryConditions =
                    _settingsState->temporaryBoundaryConditions;
                _model->UpdateModel();

                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(-1, 0)))
            {
                _settingsState->temporaryBoundaryConditions =
                    _model->GetSimulationConfig()->boundaryConditions;

                ImGui::CloseCurrentPopup();
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Note: In the future we'll allow arbitrary boundary conditions!");

            ImGui::EndPopup();
        }

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
    }

    void SidePanel::AddStatistics()
    {
        if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // For now, statistics will use big Temperature matrix, not small field vector

            auto statisticsData = _model->GetStatisticsData();

            auto durationInSeconds = statisticsData->GetDurationInSeconds();

            auto minTemperature = statisticsData->GetMinTemperature();
            auto maxTemperature = statisticsData->GetMaxTemperature();
            auto meanTemperature = statisticsData->GetMeanTemperature();

            auto numIterations = statisticsData->GetNumIterations();
            auto finalMaxError = statisticsData->GetFinalMaxError();
            auto finalMeanError = statisticsData->GetFinalMeanError();
            auto finalRMSError = statisticsData->GetFinal_RMS_Error();

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Indent(10.0f);

            ImGui::SeparatorText("Performance");
            ImGui::Text("FPS : %.1f FPS", ImGui::GetIO().Framerate);
            ImGui::Text("Simulation duration : %.2f seconds", durationInSeconds);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::SeparatorText("Temperature statistics");
            ImGui::Text("Max Temperature  : %.2f °C", maxTemperature);
            ImGui::Text("Min Temperature  : %.2f °C", minTemperature);
            ImGui::Text("Mean Temperature : %.2f °C", meanTemperature);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::SeparatorText("Final error information");
            ImGui::Text("Number of iterations : %zu", numIterations);
            ImGui::Text("Final max  error     : %.4f", finalMaxError);
            ImGui::Text("Final mean error     : %.4f", finalMeanError);
            ImGui::Text("Final RMS  error     : %.4f", finalRMSError);

            ImGui::Unindent(10.0f);
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }
    }
}