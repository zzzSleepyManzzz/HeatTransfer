#include "HeatTransfer/Visualisation/ConsolePanel.h"

namespace HeatTransfer::Visualisation
{
    ConsolePanel::ConsolePanel(std::shared_ptr<ConsoleLogger> logger) : IPanel(logger) {}

    void ConsolePanel::Render(std::shared_ptr<RendererModel> model)
    {
        _model = model;
        ShowConsole();
    }

    void ConsolePanel::ShowConsole()
    {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoResize;
        ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGui::SetNextWindowSize(ImVec2(displaySize.x * 4.0f / 5.0f, displaySize.y * 1.0f / 4.0f));
        ImGui::SetNextWindowPos(ImVec2(displaySize.x / 5.0f, displaySize.y * 3.0f / 4.0f));

        ImGui::Begin("Console", nullptr, windowFlags);
        {
            if (ImGui::Button("Clear"))
            {
                _logger->Clear();
            }

            ImGui::SameLine();

            if (ImGui::Button("Copy"))
            {
                std::string allText = "";
                for (const auto& line : _logger->GetLines())
                {
                    allText += line + "\n";
                }

                if (!allText.empty())
                {
                    ImGui::SetClipboardText(allText.c_str());
                }
            }

            ImGui::BeginChild(
                "ConsoleScroll", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

            for (const auto& line : _logger->GetLines())
            {
                ImGui::TextUnformatted(line.c_str());
            }

            ImGui::EndChild();
        }
        ImGui::End();
    }
}