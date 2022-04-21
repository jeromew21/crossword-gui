/**
 * @file crossword_serialization.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Crossword object serialization.
 * @version 0.1
 * @date 2022-04-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "crossword/crossword.hpp"

using namespace crossword_backend;

static std::string const &DELIM = ",";
static std::string const &BARRIER = "-";
static std::string const &BLANK = " ";

std::vector<std::string> Crossword::Serialize() const {
  std::vector<std::string> vec;
  vec.push_back(std::to_string(width_));
  vec.push_back(std::to_string(height_));
  for (std::size_t i = 0; i < height_; i++) {
    std::string row_string;
    for (std::size_t j = 0; j < width_; j++) {
      Cell cell = Get(i, j);
      if (cell.IsBarrier()) {
        row_string += BARRIER;
      } else if (cell.ToString() == "") {
        row_string += BLANK;
      } else {
        row_string += cell.ToString();
      }
      row_string += DELIM;
    }
    vec.push_back(row_string);
  }
  // TODO: serialize clue hints
  return vec;
}

/**
 * @brief Unserialize from a vector of lines.
 *
 * @param lines
 */
void Crossword::Unserialize(std::vector<std::string> &lines) {
  // TODO: some kind of regex validation...
  // For now, just throw exception if malformatted.

  // Clear to get ready to load
  for (std::size_t r = 0; r < height_; r++) {
    for (std::size_t c = 0; c < width_; c++) {
      SetBarrier(false, Coord(r, c), false);
    }
  }
  ClearAtoms();

  assert((int) lines.size() >= 2);

  int w = std::stoi(lines[0]);
  int h = std::stoi(lines[1]);

  SetDimensions(h, w);

  assert((int) lines.size() >= 2 + h);

  for (int i = 2; i < 2 + h; i++) {
    // Assume each atom is 1 character
    int col = 0;
    int row = i - 2;
    for (int k = 0; k < (int) lines[i].size(); k++) {
      std::string const &val = lines[i].substr(k, 1);
      Coord coord(row, col);
      if (val == DELIM) {
        // do nothing
      } else if (val == BARRIER) {
        SetBarrier(true, coord, false);
        col++;
      } else {
        if (val != BLANK) {
          Set(Atom(val), coord);
        }
        col++;
      }
    }
  }
}