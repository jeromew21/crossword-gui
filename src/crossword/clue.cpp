/**
 * @file clue.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Clue implementation
 * @version 0.1
 * @date 2022-04-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "crossword/crossword.hpp"

#include <map>

using namespace crossword_backend;

/**
 * @brief Checks whether two clue objects have the same
 * grid coordinates.
 *
 * Not a meaningful notion of equality,
 * since it might be possible that the two clues
 * have differing filled-in states.
 *
 * @param other the object of comparison
 */
bool Clue::SameCoords(Clue const &other) const {
  if (size_ != other.GetSize())
    return false;
  for (std::size_t i = 0; i < size_; i++) {
    if (coord_list_[i] != other.coord_list_[i]) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Produce a list of clue objects for the current puzzle.
 *
 * Runtime: O(mn), where (m x n) is the dimension of the puzzle.
 * In other words, linear in the number of cells.
 *
 * @return std::vector<Clue> 
 */
std::vector<Clue> Crossword::Clues_() const {
  std::vector<Clue> clue_vector;
  std::vector<Clue> across = UnfilteredClues(kACROSS);
  std::vector<Clue> down = UnfilteredClues(kDOWN);
  clue_vector.reserve(across.size() + down.size());

  std::map<Coord, std::vector<std::size_t>> start_table;

  std::size_t index = 0;
  for (auto it = std::begin(across); it != std::end(across); ++it) {
    if (it->GetSize() >= 3) {
      clue_vector.push_back(*it);
      start_table[it->GetStart()].push_back(index++);
    }
  }
  for (auto it = std::begin(down); it != std::end(down); ++it) {
    if (it->GetSize() >= 3) {
      clue_vector.push_back(*it);
      start_table[it->GetStart()].push_back(index++);
    }
  }

  for (auto it = std::begin(clue_vector); it != std::end(clue_vector); ++it) {
    bool locked = true;
    for (auto coord = std::begin(it->coord_list_); coord != std::end(it->coord_list_); ++coord) {
      if (!IsLocked(*coord) || Get(*coord).GetContents().IsEmpty()) {
        locked = false;
      }
    }
    it->SetLocked(locked);
  }

  // Give each clue its number in context of puzzle
  int cn = 1;
  for (std::size_t i = 0; i < height_; i++) {
    for (std::size_t j = 0; j < width_; j++) {
      Coord c(i, j);
      if (start_table.find(c) != start_table.end() && start_table[c].size() > 0) {
        for (std::size_t k = 0; k < start_table[c].size(); k++) {
          int index = start_table[c][k];
          clue_vector[index].SetNumber(cn);
        }
        cn++;
      }
    }
  }

  return clue_vector;
}

/**
 * @brief Returns a vector of pointers to clues at a particular coordinate coord.
 * 
 * @param all_clues 
 * @param coord 
 * @return std::vector<Clue*> 
 */
std::vector<Clue *> Crossword::CluesStartingAt_(std::vector<Clue> &all_clues, const Coord coord) const {
  assert(InBounds(coord));
  std::vector<Clue *> result;
  for (auto cl = std::begin(all_clues); cl != std::end(all_clues); ++cl) {
    for (auto it = std::begin(cl->coord_list_); it != std::end(cl->coord_list_); ++it) {
      if (*it == coord) {
        result.push_back(&(*cl));
      }
    }
  }
  assert(result.size() <= 2);
  return result;
}

/**
 * @brief Get the clue identifier (i.e, 1 as in 1-ACROSS) starting at coordinate c.
 * 
 * If the coordinate is not the start of a clue, then return kNO_NUMBER.
 * 
 * @param all_clues 
 * @param coord 
 * @return ClueNumber 
 */
ClueNumber Crossword::GetClueNumber_(std::vector<Clue> &all_clues, const Coord coord) const {
  std::vector<Clue *> clues = CluesStartingAt_(all_clues, coord);
  if (clues.size() == 2) {
    if (clues[0]->GetStart() == coord) {
      return clues[0]->GetNumber();
    } else if (clues[1]->GetStart() == coord) {
      return clues[1]->GetNumber();
    } else {
      return kNO_NUMBER;
    }
  } else if (clues.size() == 0) {
    return kNO_NUMBER;
  } else if (clues[0]->GetStart() == coord) {
    return clues[0]->GetNumber();
  }
  return kNO_NUMBER;
}

/**
 * @brief Get the ignore value for a clue.
 *
 * @return true
 * @return false
 */
bool Clue::IsLocked() const {
  return locked_;
}

/**
 * @brief Returns true iff the clue has no blank spots.
 * 
 * @return true 
 * @return false 
 */
bool Clue::IsFilled() const {
  if (size_ == 0)
    return false;
  for (auto const &value: constraints_.atoms_) {
    if (value.IsEmpty()) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Return true if and only if word fits constraints.
 * 
 * @param word 
 * @return true 
 * @return false 
 */
bool Clue::FitsWord(Word const &word) const {
  assert(size_ == constraints_.size());

  for (std::size_t i = 0; i < size_; i++) {
    const Atom &current_atom = constraints_[i];
    if (!current_atom.IsEmpty() && current_atom != word[i]) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Debug representation of a clue.
 *
 * @return std::string
 */
std::string Clue::ReprString() const {
  std::ostringstream s;
  s << "Clue{id=" << clue_number_ << ", ";
  s << "start=" << start_.ReprString() << ", size=" << size_;

  if (direction_ == kDOWN) {
    s << ", Down";
  } else {
    s << ", Across";
  }

  s << ", constraints=|";
  for (auto it = std::begin(constraints_.atoms_); it != std::end(constraints_.atoms_); ++it) {
    std::string val = it->ToString();
    if (val == "")
      s << " ";
    s << val << "|";
  }

  s << "}";
  return s.str();
}

/**
 * @brief Returns true iff the clue is filled with a valid word.
 *
 * @param db
 * @return true
 * @return false
 */
bool Clue::IsSolved(WordDatabase &db) const {
  if (!IsFilled())
    return false;
  return db.ContainsEntry(ToWord());
}

/**
 * @brief Returns true iff the clue is not filled or a valid word.
 *
 * Blank spots are O.K.
 *
 * @param db
 * @return true
 * @return false
 */
bool Clue::IsValid(WordDatabase &db) const {
  if (!IsFilled())
    return true; // Blank spots are O.K.

  return Clue::IsSolved(db);
}

/**
 * @brief Returns the index of coord c, kNO_NUMBER if it doesn't exist.
 *
 * @param coord
 * @return int
 */
int Clue::IndexOfCoord(const Coord coord) const {
  for (std::size_t i = 0; i < coord_list_.size(); i++) {
    if (coord_list_[i] == coord)
      return static_cast<int>(i);
  }
  return kNO_NUMBER;
}

/**
 * @brief Returns whether a clue is completely empty or not.
 *
 * @return true
 * @return false
 */
bool Clue::IsEmpty() const {
  return GetOpenSpots() == size_;
}

/**
 * @brief Returns number of free constraints.
 * 
 * @return std::size_t 
 */
std::size_t Clue::GetOpenSpots() const {
  std::size_t val = 0;
  for (std::size_t i = 0; i < size_; i++) {
    if (constraints_[i].IsEmpty())
      val++;
  }
  return val;
}

/**
 * @brief Return the word corresponding to the state of the clue.
 *
 * @return Word
 */
Word const &Clue::ToWord() const {
  return constraints_;
}
