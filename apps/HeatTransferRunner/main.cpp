#include <memory>

#include "HeatTransfer/Orchestration/SimpleApplication.h"

int main()
{
    auto simpleApp = std::make_shared<HeatTransfer::Orchestration::SimpleApplication>();
    simpleApp->Run();

    return 0;
}