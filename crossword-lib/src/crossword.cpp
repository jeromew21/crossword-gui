#include "crossword.hpp"

using namespace crossword_lib;





Crossword::Crossword(std::size_t width, std::size_t height) : width_(width), height_(height) {

}

std::size_t Crossword::Height() const {
    return height_;
}

std::size_t Crossword::Width() const {
    return width_;
}