#pragma once

#include <array>
#include <string>

#include "HeatTransfer/Core/Letter.h"

namespace HeatTransfer::Core
{
    class Glyph
    {
    public:
        Glyph() = delete;

        static bool Contains(Letter letter, double s, double t);

    private:
        // 5 x 7 bitmap
        static constexpr int COLUMNS = 5;
        static constexpr int ROWS = 7;

        // Top row first, '#' is inside the letter
        static constexpr std::array<std::string, ROWS> LETTER_H = {
            "#...#", "#...#", "#...#", "#####", "#...#", "#...#", "#...#"};

        static constexpr std::array<std::string, ROWS> LETTER_T = {
            "#####", "..#..", "..#..", "..#..", "..#..", "..#..", "..#.."};

        static const std::array<std::string, ROWS>& GetBitmap(Letter letter);
    };
}
