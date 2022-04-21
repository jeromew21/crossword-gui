/**
 * @file crossword.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Implementation for crossword puzzle methods
 * @version 0.1
 * @date 2022-04-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "crossword/crossword.hpp"

using namespace crossword_backend;

/**
 * @brief Returns true iff the pattern of barriers is a valid crossword puzzle.
 *
 */
bool Crossword::IsValidPattern() const {
  std::vector<Clue> across = UnfilteredClues(kACROSS);
  std::vector<Clue> down = UnfilteredClues(kDOWN);
  for (auto it = std::begin(across); it != std::end(across); ++it) {
    if (it->GetSize() == 2)
      return false;
  }
  for (auto it = std::begin(down); it != std::end(down); ++it) {
    if (it->GetSize() == 2)
      return false;
  }
  return true;
}

/**
 * @brief Debug string representation. 
 * 
 * Writes out the 2d array.
 * 
 * @return std::string 
 */
std::string Crossword::ReprString() const {
  std::string s;
  for (std::size_t i = 0; i < height_; i++) {
    s += "|";
    for (std::size_t j = 0; j < width_; j++) {
      Cell cell = Get(i, j);
      if (cell.IsBarrier()) {
        s += "=";
      } else if (cell.ToString() == "") {
        s += " ";
      } else {
        s += cell.ToString();
      }
      s += "|";
    }
    s += "\n";
  }
  return "\n" + s;
}

/**
 * @brief Get the String object associated with clue.
 *
 * @param clue
 * @return std::string
 */
std::string Crossword::GetHint(Clue const &clue) const {
  std::vector<Clue> clues = Clues();
  for (auto it = std::begin(clues); it != std::end(clues); ++it) {
    if (it->SameCoords(clue)) {
      return GetHint(clue.GetStart(), clue.GetDirection());
    }
  }
  assert(false);
  return "";
}

/**
 * @brief Get the String object associated with index num.
 *
 * @param num the clue identifier
 * @param direction the clue direction
 * @return std::string
 */
std::string Crossword::GetHint(const ClueNumber num, const WordDirection direction) const {
  std::vector<Clue> clues = Clues();
  for (auto it = std::begin(clues); it != std::end(clues); ++it) {
    if (it->GetNumber() == num && it->GetDirection() == direction) {
      return GetHint(it->GetStart(), direction);
    }
  }
  assert(false);
  return "";
}

/**
 * @brief Get the String object associated with coordinate coord.
 *
 * @param coord
 * @param direction
 * @return std::string
 */
std::string Crossword::GetHint(const Coord coord, const WordDirection direction) const {
  return clue_strings_[coord.row][coord.col][direction];
}

/**
 * @brief Set the String object associated with clue.
 *
 * @param clue
 * @param hint
 */
void Crossword::SetHint(Clue const &clue, std::string const &hint) {
  std::vector<Clue> clues = Clues();
  for (auto it = std::begin(clues); it != std::end(clues); ++it) {
    if (it->SameCoords(clue)) {
      SetHint(clue.GetStart(), clue.GetDirection(), hint);
      return;
    }
  }
  assert(false);
}

/**
 * @brief Set the hint associated with coordinate c.
 *
 * @param coord
 * @param direction
 * @param hint
 */
void Crossword::SetHint(const Coord coord, const WordDirection direction, std::string const &hint) {
  clue_strings_[coord.row][coord.col][direction] = hint;
}

/**
 * @brief Set the hint associated with clue identifier num.
 *
 * @param num
 * @param direction
 * @param hint
 */
void Crossword::SetHint(const ClueNumber num, const WordDirection direction, std::string const &hint) {
  std::vector<Clue> clues = Clues();
  for (auto it = std::begin(clues); it != std::end(clues); ++it) {
    if (it->GetNumber() == num && it->GetDirection() == direction) {
      SetHint(it->GetStart(), direction, hint);
      return;
    }
  }
  assert(false);
}

/**
 * @brief Returns the cell at coordinate c.
 *
 * Copies the underlying cell struct.
 *
 * @param coord
 * @return Cell
 */
Cell Crossword::Get(const Coord coord) const {
  assert(InBounds(coord));

  return grid_[coord.row][coord.col];
}

/**
 * @brief Overloaded method that retuns a cell given a row and a column index.
 *
 * @param row
 * @param col
 * @return Cell
 */
Cell Crossword::Get(const std::size_t row, const std::size_t col) const {
  return Get(Coord(row, col));
}

/**
 * @brief Set the value of cell at coordinate c.
 *
 * Indirectly creates a new CrosswordSetAction object and pushes it to the stack.
 *
 * @param val new cell value
 * @param coord coordinate to set
 */
void Crossword::Set(const Atom val, const Coord coord) {
  assert(InBounds(coord));
  assert(!Get(coord).IsBarrier());

  ApplyAction(new CrosswordSetAction(val, Get(coord).GetContents(), coord));
}

/**
 * @brief Set the value of cell at coordinate c without affecting stack.
 * 
 * @param val
 * @param coord
 */
void Crossword::Set_(const Atom val, const Coord coord) {
  assert(InBounds(coord));
  assert(!Get(coord).IsBarrier());

  grid_[coord.row][coord.col].SetContents(val);
  for (auto const &clue_pointer: clue_cache_.cell_mapping[coord.row][coord.col]) {
    std::size_t index = clue_pointer->IndexOfCoord(coord); // indexOfCoord is linear... TODO make it constant
    clue_pointer->SetConstraint(index, val);
  }
}

/**
 * @brief Apply an action and push it to the stack.
 *
 * Takes unique ownership of action. As such, best practice is directly call `new` in invocation.
 *
 * @param action pointer to an action
 */
void Crossword::ApplyAction(CrosswordAction *action) {
  action->Apply(*this);
  action_stack_.Push(action);
}

/**
 * @brief Undo an action
 *
 * @return true action was undone
 * @return false action was not undone
 */
bool Crossword::Undo() {
  return action_stack_.Pop(*this);
}

/**
 * @brief Redo an action
 *
 * @return true action was redone
 * @return false action was not redone
 */
bool Crossword::Redo() {
  return action_stack_.Replace(*this);
}

/**
 * @brief Toggle whether a particular cell is barrier or not.
 *
 * @param coord coordinate
 * @param enforce_symmetry true iff we want to enforce rotational symmetry.
 */
void Crossword::ToggleBarrier(const Coord coord, const bool enforce_symmetry) {
  SetBarrier(!grid_[coord.row][coord.col].IsBarrier(), coord, enforce_symmetry);
}

/**
 * @brief Set the barrier condition of a particular cell.
 * 
 * @param val boolean value to set to
 * @param coord coordinate
 * @param enforce_symmetry true iff we want to enforce rotational symmetry.
 */
void Crossword::SetBarrier(const bool val, const Coord coord, bool enforce_symmetry) {
  DirtyClueStructure();

  grid_[coord.row][coord.col].SetBarrier(val);
  if (enforce_symmetry) {
    Coord pair = GetRotationalPair(coord);
    if (pair.row != coord.row || pair.col != coord.col) {
      grid_[pair.row][pair.col].SetBarrier(val);
    }
  }

  PopulateClueStructure();
}

/**
 * @brief Returns all possible words of direction direction.
 *
 * Does not filter based on size or assign a clue number.
 * Useful to verify whether a pattern of barriers is valid or not.
 *
 * @param direction 
 * @return std::vector<Clue> 
 */
std::vector<Clue> Crossword::UnfilteredClues(const WordDirection direction) const {
  std::vector<Clue> vec;
  int state = 0; // 0, ended, 1, collecting
  std::size_t size = 0;
  std::size_t startk = 0;
  std::vector<Atom> constraint_vector;
  std::vector<Coord> coord_vector;
  std::size_t iMax = height_;
  std::size_t kMax = width_;
  if (direction == kDOWN) {
    iMax = width_;
    kMax = height_;
  }
  for (std::size_t i = 0; i < iMax; i++) {
    state = 0;
    size = 0;
    startk = 0;
    constraint_vector.clear();
    coord_vector.clear();
    for (std::size_t k = 0; k < kMax + 1; k++) {
      Coord c = Coord(i, k);
      if (direction == kDOWN) {
        c = Coord(k, i);
      }
      if (k == kMax || Get(c).IsBarrier()) {
        if (state == 1) {
          assert(size = constraint_vector.size());
          assert(coord_vector.size() == constraint_vector.size());
          Coord start_coord = Coord(i, startk);
          if (direction == kDOWN) {
            start_coord = Coord(startk, i);
          }
          vec.push_back(Clue(
                  direction, start_coord, size, constraint_vector, coord_vector));
        }
        constraint_vector.clear();
        coord_vector.clear();
        size = 0;
        state = 0;
      } else {
        size += 1;
        constraint_vector.push_back(Get(c).GetContents());
        coord_vector.push_back(c);
        if (state == 0) {
          state = 1;
          startk = k;
        }
      }
    }
  }
  return vec;
}

/**
 * @brief Get the rotationally symmetric pair of coordinate c.
 *
 * @param coord
 * @return Coord
 */
Coord Crossword::GetRotationalPair(const Coord coord) const {
  return Coord(height_ - 1 - coord.row, width_ - 1 - coord.col);
}

/**
 * @brief Checks whether a coordinate is in bounds.
 * 
 * @param coord 
 * @return true 
 * @return false 
 */
bool Crossword::InBounds(const Coord coord) const {
  return coord.row < height_ && coord.col < width_;
}

/**
 * @brief Sets the cells corresponding to a particular clue to a particular word.
 *
 * Not meaningful if the word does not match the clue's state.
 *
 * @param clue the clue in question
 * @param word the word in question
 */
void Crossword::SetClue(Clue const &clue, Word const &word) {
  assert(clue.FitsWord(word));

  CrosswordActionGroup *group = new CrosswordActionGroup(*this, clue, word);
  ApplyAction(group);
}

/**
 * @brief Clears the cells corresponding to a particular clue.
 *
 * @param clue the clue in question
 */
void Crossword::ClearClue(Clue const &clue) {
  CrosswordActionGroup *group = new CrosswordActionGroup();
  for (std::size_t i = 0; i < clue.GetSize(); i++) {
    Coord c = clue.coord_list_[i];
    group->AddAction(new CrosswordSetAction(Atom(), Get(c).GetContents(), c));
  }
  ApplyAction(group);
}

/**
 * @brief Load the clue cache up and set the dirty flag to false.
 *
 */
void Crossword::PopulateClueStructure() {
  assert(clue_cache_.dirty);

  clue_cache_.clues = Clues_();
  for (std::size_t r = 0; r < height_; ++r) {
    for (std::size_t c = 0; c < width_; ++c) {
      clue_cache_.numberings[r][c] = GetClueNumber_(clue_cache_.clues, Coord(r, c));
      clue_cache_.cell_mapping[r][c] = CluesStartingAt_(clue_cache_.clues, Coord(r, c));
    }
  }
  clue_cache_.dirty = false;
}

/**
 * @brief Getter for clues.
 * 
 * Returns a copy of the vector owned by incrementally updated structure.
 * 
 * @return std::vector<Clue> 
 */
std::vector<Clue> Crossword::Clues() const {
  assert(clue_cache_.dirty == false);
  return clue_cache_.clues;
}

/**
 * @brief Get a copy of the clues starting at particular coordinate.
 * 
 * @param coord 
 * @return std::vector<Clue> A copy of the clues starting at coord.
 */
std::vector<Clue> Crossword::CluesStartingAt(const Coord coord) const {
  assert(clue_cache_.dirty == false);
  std::vector<Clue> clue_copies;
  for (auto const &pointer: clue_cache_.cell_mapping[coord.row][coord.col]) {
    clue_copies.push_back(*pointer);
  }
  return clue_copies;
}

/**
 * @brief Cached getter for GetClueNumber
 *
 * @param coord
 * @return ClueNumber
 */
ClueNumber Crossword::GetClueNumber(const Coord coord) const {
  assert(clue_cache_.dirty == false);
  return clue_cache_.numberings[coord.row][coord.col];
}

/**
 * @brief Clear the elements of a puzzle. Leave barriers and clues.
 *
 */
void Crossword::ClearAtoms() {
  CrosswordActionGroup *group = new CrosswordActionGroup();
  for (std::size_t i = 0; i < height_; i++) {
    for (std::size_t k = 0; k < width_; k++) {
      Coord c = Coord(i, k);
      if (!Get(c).IsBarrier()) {
        group->AddAction(new CrosswordSetAction(Atom(), Get(c).GetContents(), c));
      }
    }
  }
  ApplyAction(group);
}

/**
 * @brief Set the crossword puzzle dimensions.
 * 
 * @param height Puzzle height. Must be greater than 2 and leq than kMAX_DIM.
 * @param width Puzzle width. Must be greater than 2 and leq than kMAX_DIM.
 */
void Crossword::SetDimensions(const std::size_t height, const std::size_t width) {
  assert(height <= kMAX_DIM && width <= kMAX_DIM);
  assert(height > 2 && width > 2);

  DirtyClueStructure();

  height_ = height;
  width_ = width;

  PopulateClueStructure();
}

/**
 * @brief Lock or unlock a cell.
 *
 * @param coord
 * @param value
 */
void Crossword::LockCell(const Coord coord, const bool value) {
  assert(InBounds(coord));
  grid_[coord.row][coord.col].Lock(value);
}

/**
 * @brief Toggle Lock or unlock a cell.
 *
 * @param coord
 * @param value
 */
void Crossword::ToggleLockCell(const Coord coord) {
  LockCell(coord, !IsLocked(coord));
}

/**
 * @brief Signal that the clue cache is dirty.
 *
 */
void Crossword::DirtyClueStructure() {
  clue_cache_.dirty = true;
}

/**
 * @brief Get whether a certain cell is locked or unlocked.
 *
 * @param coord
 * @return true
 * @return false
 */
bool Crossword::IsLocked(const Coord coord) const {
  assert(InBounds(coord));
  return grid_[coord.row][coord.col].IsLocked();
}

/**
 * @brief Return whether a particular cell contains a letter.
 *
 * @return true if a cell has a letter
 * @return false otherwise; cell is empty or a barrier
 */
bool Crossword::IsFilled(const Coord coord) const {
  return !Get(coord).IsBarrier() && !Get(coord).GetContents().IsEmpty();
}
