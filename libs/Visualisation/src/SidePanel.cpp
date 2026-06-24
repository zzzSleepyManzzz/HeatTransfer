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
        }
    }

    void SidePanel::AddStatistics()
    {
        if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // For now, statistics will use big Temperature matrix, not small field vector

            auto statisticsData = _model->GetStatisticsData();

            auto minTemperature = statisticsData->GetMinTemperature();
            auto maxTemperature = statisticsData->GetMaxTemperature();
            auto meanTemperature = statisticsData->GetMeanTemperature();

            auto numIterations = statisticsData->GetNumIterations();
            auto finalMaxError = statisticsData->GetFinalMaxError();
            auto finalMeanError = statisticsData->GetFinalMeanError();
            auto finalRMSError = statisticsData->GetFinal_RMS_Error();

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Indent(10.0f);

            ImGui::Text("FPS: %.1f FPS", ImGui::GetIO().Framerate);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Max Temperature: %.2f °C", maxTemperature);
            ImGui::Text("Min Temperature: %.2f °C", minTemperature);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Average Temperature: %.2f °C", meanTemperature);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Number of iterations: %zu", numIterations);
            ImGui::Text("Final max  error:     %.4f", finalMaxError);
            ImGui::Text("Final mean error:     %.4f", finalMeanError);
            ImGui::Text("Final RMS  error:     %.4f", finalRMSError);

            ImGui::Unindent(10.0f);
            ImGui::Dummy(ImVec2(0.0f, 5.0f));
        }
    }
}