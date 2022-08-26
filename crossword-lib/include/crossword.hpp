#pragma once

#include <vector>
#include <array>

namespace crossword_lib
{

class Crossword
{
public:
    explicit Crossword(std::size_t width, std::size_t height);

    void Resize(std::size_t new_width, std::size_t new_height);

    [[nodiscard]] std::size_t Width() const;
    [[nodiscard]] std::size_t Height() const;
private:
    static constexpr std::size_t MAX_DIM = 100;

    std::array<std::array<char, MAX_DIM>, MAX_DIM> grid_;
    std::vector<int> action_stack_;

    std::size_t width_;
    std::size_t height_;
};

}