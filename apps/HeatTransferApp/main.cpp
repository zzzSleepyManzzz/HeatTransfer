#include <memory>

#include "HeatTransfer/Orchestration/Application.h"

int main()
{
    auto app = std::make_shared<HeatTransfer::Orchestration::Application>();
    app->Run();

    return 0;
}