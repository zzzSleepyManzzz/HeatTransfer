#include <memory>

#include "HeatTransfer/Orchestration/SimpleTransientApplication.h"

int main()
{
    auto simpleApp = std::make_shared<HeatTransfer::Orchestration::SimpleTransientApplication>();
    simpleApp->Run();

    return 0;
}