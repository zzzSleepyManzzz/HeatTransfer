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
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x / 5.0f, displaySize.y));
        ImGui::SetNextWindowPos(ImVec2(0, 0));

        ImGui::Begin("Side Bar", nullptr, windowFlags);
        {
            AddProgress();
            AddSettings();
            AddStatistics();
        }
        ImGui::End();
    }

    void SidePanel::AddProgress()
    {
        if (_model->IsSimulationRunning())
        {
            ImGui::Text("Simulation running... iteration %d", _model->GetCurrentIteration());
            ImGui::Separator();
        }
    }

    void SidePanel::AddSettings()
    {
        if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            // Light mode check box

            ImGui::Text("Turn light mode on");

            ImGui::SameLine();

            if (ImGui::Checkbox("##Turn light mode on", &_settingsState->LightModeOn))
            {
                if (_settingsState->LightModeOn)
                    ImGui::StyleColorsLight();
                else
                    ImGui::StyleColorsDark();
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            // Font Scaling

            if (ImGui::SliderFloat(
                    "Font Scaling", &_settingsState->FontScaling, 0.5f, 2.0f, "%.2f"))
            {
                ImGui::GetStyle().FontScaleMain = _settingsState->FontScaling;
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            // Add simulation parameters, material properties and boundary conditions button

            AddParametersButton();
            AddMaterialPropertiesButton();
            AddBoundaryConditionsButton();
            AddRunButton();
        }
    }

    void SidePanel::AddParametersButton()
    {
        if (ImGui::Button("Simulation Parameters", ImVec2(-1, 0)))
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
            ImGui::Text("Press Run to start a new run with them");

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Separator();

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            if (ImGui::InputInt("Number of rows (y, width)",
                                &_settingsState->TemporaryParameters.Rows))
            {
                _settingsState->TemporaryParameters.Rows =
                    std::max(4, _settingsState->TemporaryParameters.Rows);
            }
            if (ImGui::InputInt("Number of columns (x, length)",
                                &_settingsState->TemporaryParameters.Columns))
            {
                _settingsState->TemporaryParameters.Columns =
                    std::max(4, _settingsState->TemporaryParameters.Columns);
            }
            if (ImGui::InputInt("Number of layers (z, height)",
                                &_settingsState->TemporaryParameters.Layers))
            {
                _settingsState->TemporaryParameters.Layers =
                    std::max(4, _settingsState->TemporaryParameters.Layers);
            }

            // Shown in millimetres, stored in metres
            double gridSpacingMm = _settingsState->TemporaryParameters.GridSpacing * 1000.0;

            if (ImGui::InputDouble("Grid spacing [mm]", &gridSpacingMm))
            {
                _settingsState->TemporaryParameters.GridSpacing =
                    std::max(1e-3, gridSpacingMm) / 1000.0;
            }

            const auto& parameters = _settingsState->TemporaryParameters;

            ImGui::Text("Plate size: %.3g x %.3g x %.3g m",
                        (parameters.Columns - 1) * parameters.GridSpacing,
                        (parameters.Rows - 1) * parameters.GridSpacing,
                        (parameters.Layers - 1) * parameters.GridSpacing);

            if (ImGui::InputDouble("Residual tolerance",
                                   &_settingsState->TemporaryParameters.ResidualTolerance,
                                   0.0,
                                   0.0,
                                   "%.10f"))
            {
                _settingsState->TemporaryParameters.ResidualTolerance =
                    std::max(1e-10, _settingsState->TemporaryParameters.ResidualTolerance);
            }
            if (ImGui::InputDouble("Error tolerance",
                                   &_settingsState->TemporaryParameters.ErrorTolerance,
                                   0.0,
                                   0.0,
                                   "%.10f"))
            {
                _settingsState->TemporaryParameters.ErrorTolerance =
                    std::max(1e-10, _settingsState->TemporaryParameters.ErrorTolerance);
            }
            if (ImGui::InputDouble("Relaxation factor",
                                   &_settingsState->TemporaryParameters.RelaxationFactor))
            {
                _settingsState->TemporaryParameters.RelaxationFactor =
                    std::clamp(_settingsState->TemporaryParameters.RelaxationFactor, 0.01, 1.99);
            }
            if (ImGui::InputInt("Max iterations",
                                &_settingsState->TemporaryParameters.MaxIterations))
            {
                _settingsState->TemporaryParameters.MaxIterations =
                    std::max(1, _settingsState->TemporaryParameters.MaxIterations);
            }
            if (ImGui::Combo("Residual tracking metric",
                             &_settingsState->ResidualTrackingMetric,
                             TRACKER_OPTIONS,
                             IM_ARRAYSIZE(TRACKER_OPTIONS)))
            {
                _settingsState->TemporaryParameters.ResidualTrackingMetric =
                    TRACKER_ARRAY[_settingsState->ResidualTrackingMetric];
            }
            if (ImGui::Combo("Error tracking metric",
                             &_settingsState->ErrorTrackingMetric,
                             TRACKER_OPTIONS,
                             IM_ARRAYSIZE(TRACKER_OPTIONS)))
            {
                _settingsState->TemporaryParameters.ErrorTrackingMetric =
                    TRACKER_ARRAY[_settingsState->ErrorTrackingMetric];
            }
            if (ImGui::Combo("Smoother",
                             &_settingsState->Smoother,
                             SMOOTHER_OPTIONS,
                             IM_ARRAYSIZE(SMOOTHER_OPTIONS)))
            {
                _settingsState->TemporaryParameters.Smoother =
                    SMOOTHER_ARRAY[_settingsState->Smoother];
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            if (ImGui::Button("OK", ImVec2(displaySize.x * 1.0f / 6.0f, 0)))
            {
                _model->GetSimulationConfig()->Parameters = _settingsState->TemporaryParameters;

                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(-1, 0)))
            {
                _settingsState->TemporaryParameters = _model->GetSimulationConfig()->Parameters;
                _settingsState->Smoother = (int)_model->GetSimulationConfig()->Parameters.Smoother;
                _settingsState->ResidualTrackingMetric =
                    (int)_model->GetSimulationConfig()->Parameters.ResidualTrackingMetric;
                _settingsState->ErrorTrackingMetric =
                    (int)_model->GetSimulationConfig()->Parameters.ErrorTrackingMetric;

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
    }

    void SidePanel::AddMaterialPropertiesButton()
    {
        if (ImGui::Button("Material Properties", ImVec2(-1, 0)))
        {
            ImGui::OpenPopup("Modify material properties");
        }

        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x * 1.0f / 3.0f, displaySize.y * 2.0f / 3.0f));

        ImGui::SetNextWindowPos(
            ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(
                "Modify material properties", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Text("Set new material properties here");
            ImGui::Text("Press Run to start a new run with them");

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Separator();

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            if (ImGui::InputDouble(
                    "Thermal conductivity [W/(m K)]",
                    &_settingsState->TemporaryMaterialProperties.ThermalConductivity))
            {
                _settingsState->TemporaryMaterialProperties.ThermalConductivity =
                    std::max(1e-6, _settingsState->TemporaryMaterialProperties.ThermalConductivity);
            }
            if (ImGui::InputDouble(
                    "Internal heat generation [W/m³]",
                    &_settingsState->TemporaryMaterialProperties.InternalHeatGeneration))
            {
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            if (ImGui::Button("OK", ImVec2(displaySize.x * 1.0f / 6.0f, 0)))
            {
                _model->GetSimulationConfig()->MaterialProperties =
                    _settingsState->TemporaryMaterialProperties;

                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(-1, 0)))
            {
                _settingsState->TemporaryMaterialProperties =
                    _model->GetSimulationConfig()->MaterialProperties;

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
    }

    void SidePanel::AddBoundaryConditionsButton()
    {
        if (ImGui::Button("Boundary Conditions", ImVec2(-1, 0)))
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
            ImGui::Text("Press Run to start a new run with them");

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::Separator();

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            auto& boundaryConditions = _settingsState->TemporaryBoundaryConditions;

            for (std::size_t i = 0; i < Core::Faces::ALL.size(); i++)
            {
                const auto face = Core::Faces::ALL[i];
                auto boundaryCondition = boundaryConditions.GetFace(face);

                ImGui::PushID((int)i);
                ImGui::SeparatorText(FACE_NAMES[i]);

                int type = (int)boundaryCondition.Type;

                if (ImGui::Combo(
                        "Type", &type, BOUNDARY_TYPE_OPTIONS, IM_ARRAYSIZE(BOUNDARY_TYPE_OPTIONS)))
                {
                    boundaryCondition.Type = (Core::BoundaryType)type;
                }

                switch (boundaryCondition.Type)
                {
                    case Core::BoundaryType::FIXED_TEMPERATURE:
                        if (ImGui::InputDouble("Temperature [K]", &boundaryCondition.Temperature))
                        {
                            boundaryCondition.Temperature =
                                std::max(0.0, boundaryCondition.Temperature);
                        }
                        break;

                    case Core::BoundaryType::HEAT_FLUX:
                        ImGui::InputDouble("Heat flux into plate [W/m²]",
                                           &boundaryCondition.HeatFlux);
                        break;

                    case Core::BoundaryType::CONVECTION:
                        if (ImGui::InputDouble("Heat transfer coefficient [W/(m² K)]",
                                               &boundaryCondition.HeatTransferCoefficient))
                        {
                            boundaryCondition.HeatTransferCoefficient =
                                std::max(0.0, boundaryCondition.HeatTransferCoefficient);
                        }
                        if (ImGui::InputDouble("Ambient temperature [K]",
                                               &boundaryCondition.AmbientTemperature))
                        {
                            boundaryCondition.AmbientTemperature =
                                std::max(0.0, boundaryCondition.AmbientTemperature);
                        }
                        break;
                }

                boundaryConditions.SetFace(face, boundaryCondition);

                ImGui::PopID();
            }

            ImGui::SeparatorText("Letters (fixed temperature)");

            for (std::size_t i = 0; i < boundaryConditions.Letters.size(); i++)
            {
                auto& letter = boundaryConditions.Letters[i];

                ImGui::PushID(100 + (int)i);

                ImGui::Checkbox("##Enabled", &letter.Enabled);
                ImGui::SameLine();
                ImGui::Text("Letter %s on the %s",
                            LETTER_NAMES[(int)letter.Letter],
                            FACE_NAMES[(int)letter.Surface]);

                if (letter.Enabled && ImGui::InputDouble("Temperature [K]", &letter.Temperature))
                {
                    letter.Temperature = std::max(0.0, letter.Temperature);
                }

                ImGui::PopID();
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::TextWrapped("A heat flux of 0 is an insulated (adiabatic) face");

            const bool isWellPosed = boundaryConditions.IsWellPosed();

            if (!isWellPosed)
                ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f),
                                   "At least one face needs a fixed temperature or convection");

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::BeginDisabled(!isWellPosed);

            if (ImGui::Button("OK", ImVec2(displaySize.x * 1.0f / 6.0f, 0)))
            {
                _model->GetSimulationConfig()->BoundaryConditions =
                    _settingsState->TemporaryBoundaryConditions;

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndDisabled();
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();

            if (ImGui::Button("Cancel", ImVec2(-1, 0)))
            {
                _settingsState->TemporaryBoundaryConditions =
                    _model->GetSimulationConfig()->BoundaryConditions;

                ImGui::CloseCurrentPopup();
            }

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::EndPopup();
        }

        ImGui::Dummy(ImVec2(0.0f, 5.0f));
    }

    void SidePanel::AddRunButton()
    {
        const bool isRunning = _model->IsSimulationRunning();
        const ImVec2 size(-1, 1.4f * ImGui::GetFrameHeight());

        if (ImGui::Button(isRunning ? "Cancel" : "Run", size))
        {
            if (isRunning)
                _model->CancelSimulation();
            else
            {
                auto config = _model->GetSimulationConfig();

                config->Parameters = _settingsState->TemporaryParameters;
                config->MaterialProperties = _settingsState->TemporaryMaterialProperties;
                config->BoundaryConditions = _settingsState->TemporaryBoundaryConditions;

                _model->UpdateModel();
            }
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

            auto finalMaxResidual = statisticsData->GetFinalMaxResidual();
            auto finalMeanResidual = statisticsData->GetFinalMeanResidual();
            auto finalRMSResidual = statisticsData->GetFinal_RMS_Residual();

            const char* convergenceStatement =
                statisticsData->IsConverged()
                    ? "Convergence achieved - residual metric below required tolerance value"
                    : "Failed to converge - either stopped due to reaching max iterations "
                      "or lack of progression (low max error)";

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::Indent(10.0f);

            ImGui::SeparatorText("Performance");
            ImGui::Text("FPS : %.1f FPS", ImGui::GetIO().Framerate);
            ImGui::Text("Simulation duration : %.2f seconds", durationInSeconds);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::SeparatorText("Temperature statistics");
            ImGui::Text("Max Temperature  : %.2f K", maxTemperature);
            ImGui::Text("Min Temperature  : %.2f K", minTemperature);
            ImGui::Text("Mean Temperature : %.2f K", meanTemperature);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::SeparatorText("Final error information");
            ImGui::Text("Number of iterations : %u", numIterations);
            ImGui::Text("Final max  error     : %.4g", finalMaxError);
            ImGui::Text("Final mean error     : %.4g", finalMeanError);
            ImGui::Text("Final RMS  error     : %.4g", finalRMSError);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));

            ImGui::SeparatorText("Final residual information");
            ImGui::Text("Number of iterations : %u", numIterations);
            ImGui::Text("Final max  residual  : %.4g", finalMaxResidual);
            ImGui::Text("Final mean residual  : %.4g", finalMeanResidual);
            ImGui::Text("Final RMS  residual  : %.4g", finalRMSResidual);

            ImGui::Dummy(ImVec2(0.0f, 5.0f));
            ImGui::TextWrapped("%s", convergenceStatement);

            float bottomPadding = 80.0f;
            ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y - bottomPadding));
            ImGui::TextWrapped("Note: Error refers to the per-iteration temperature change (update "
                               "difference) for brevity");

            ImGui::Unindent(10.0f);
        }
    }
}