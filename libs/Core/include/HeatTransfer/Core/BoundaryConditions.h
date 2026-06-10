#pragma once

namespace HeatTransfer::Core
{
    struct BoundaryConditions
    {
        double topEdge;
        double bottomEdge;
        double leftEdge;
        double rightEdge;
        double innerSquare;
        double centerPoint;
    };
}