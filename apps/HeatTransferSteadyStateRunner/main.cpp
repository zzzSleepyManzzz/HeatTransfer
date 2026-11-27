#include <memory>

#include "HeatTransfer/Orchestration/SimpleSteadyStateApplication.h"

int main()
{
    auto simpleApp = std::make_shared<HeatTransfer::Orchestration::SimpleSteadyStateApplication>();
    simpleApp->Run();

    return 0;
}