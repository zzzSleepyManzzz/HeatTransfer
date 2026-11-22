#pragma once

#include <array>

#include "HeatTransfer/Core/Face.h"

namespace HeatTransfer::Core
{
    class Faces
    {
    public:
        Faces() = delete;

        static constexpr std::array<Face, 6> ALL = {
            Face::LEFT, Face::RIGHT, Face::TOP, Face::BOTTOM, Face::FRONT, Face::BACK};
    };
}
