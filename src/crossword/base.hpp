/**
 * @file base.hpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Base class for printable objects, as well as holding important constants and Coord class definition.
 * @version 0.1
 * @date 2022-04-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef BASE_H
#define BASE_H

#include <string>
#include <sstream>

namespace crossword_backend {

  /**
   * @brief Max width or height of crossword puzzle.
   *
   */
  constexpr std::size_t kMAX_DIM = 35;

  /**
   * @brief Default crossword starting height.
   *
   */
  constexpr std::size_t kSTART_HEIGHT = 5;

  /**
   * @brief Default crossword starting width.
   *
   */
  constexpr std::size_t kSTART_WIDTH = 5;

  /**
   * @brief Across direction
   *
   */
  constexpr std::size_t kACROSS = 0;

  /**
   * @brief Down direction
   *
   */
  constexpr std::size_t kDOWN = 1;

  /**
   * @brief Represents either kACROSS or kDOWN.
   *
   */
  typedef std::size_t WordDirection;

  /**
   * @brief Represents a clue's number. For example, the 2 in "2-ACROSS".
   *
   */
  typedef int ClueNumber;

  /**
   * @brief Used to represent no value in methods returning int.
   *
   */
  constexpr int kNO_NUMBER = -1;

  /**
   * @brief A 2-tuple representing the coordinates of an array of rows indexed by (row, col).
   *
   * The upper left is (0, 0) and the bottom right is (n-1, n-1).
   *
   */
  struct Coord {
    /**
     * @brief Row index
     */
    std::size_t row;

    /**
     * @brief Column index
     */
    std::size_t col;

    /**
     * @brief Debug representation of a Coord object
     *
     * @return std::string
     */
    [[nodiscard]] std::string ReprString() const { return "(" + std::to_string(row) + ", " + std::to_string(col) + ")"; };

    /**
     * @brief Construct a new Coord object from given indices.
     *
     * @param row row index
     * @param col col index
     */
    Coord(const std::size_t row, const std::size_t col) : row(row), col(col) {};

    /**
     * @brief Overload equality for Coord
     *
     * @param c other Coord
     * @return true
     * @return false
     */
    inline bool operator==(const Coord &c) const { return c.row == row && c.col == col; }

    /**
     * @brief Overload not equal operator for Coord
     *
     * @param c other Coord
     * @return true
     * @return false
     */
    inline bool operator!=(const Coord &c) const { return c.row != row || c.col != col; };

    /**
     * @brief Overload compare operator for Coord.
     *
     * Flattens 2d coordinates (with respect to maximum puzzle dimensions) to make this possible.
     *
     * @param c other Coord
     * @return true
     * @return false
     */
    inline bool operator<(const Coord &c) const { return (row * kMAX_DIM) + col < (c.row * kMAX_DIM) + c.col; };
  };
}

#endif
