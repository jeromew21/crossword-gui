/**
 * @file cell.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Implementation of cell methods
 * @version 0.1
 * @date 2022-04-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "crossword/crossword.hpp"

using namespace crossword_backend;

/**
 * @brief Set the cell to be a barrier cell.
 *
 * @param val
 */
void Cell::SetBarrier(const bool val) {
  is_barrier = val;
}

/**
 * @brief True iff cell is a barrier.
 *
 * @return bool
 */
bool Cell::IsBarrier() const {
  return is_barrier;
}

/**
 * @brief Get the contents of a cell.
 *
 * @return Atom
 */
Atom Cell::GetContents() const {
  assert(!IsBarrier());
  return contents;
}

/**
 * @brief Set the contents of a cell.
 *
 * @param atom new contents
 */
void Cell::SetContents(const Atom atom) {
  contents = atom;
}

/**
 * @brief Lock or unlock a cell.
 *
 * @param value
 */
void Cell::Lock(const bool value) {
  locked = value;
}

/**
 * @brief Get whether a cell is locked or not.
 *
 * @return true
 * @return false
 */
bool Cell::IsLocked() const {
  return locked;
}

/**
 * @brief String representation of a cell.
 *
 * Not meaningful in the case that the cell is a barrier.
 *
 * @return std::string
 */
std::string Cell::ToString() const {
  assert(!is_barrier);
  return GetContents().ToString();
}

/**
 * @brief Debug representation of a cell.
 *
 * @return std::string
 */
std::string Cell::ReprString() const {
  if (is_barrier) {
    return "Cell{is_barrier}";
  }
  return "Cell{contents=" + contents.ToString() + "}";
}
