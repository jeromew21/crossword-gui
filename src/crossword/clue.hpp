/**
 * @file clue.hpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Clue class declaration.
 * @version 0.1
 * @date 2022-04-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CLUE_HPP
#define CLUE_HPP

#include "crossword/base.hpp"
#include "crossword/word.hpp"

namespace crossword_backend {

  class WordDatabase;

  /**
   * @brief A partial word built from some cells, as well as a puzzle clue.
   *
   * These are a bit heavy, and allocating them is a bottleneck atm. TODO: optimize
   *
   */
  class Clue {
  public:
    /**
     * @brief The clue's coordinates, listed in order from begin to end.
     *
     */
    std::vector<Coord> coord_list_;

    [[nodiscard]] bool IsLocked() const;

    [[nodiscard]] bool FitsWord(Word const &word) const;

    [[nodiscard]] bool IsFilled() const;

    [[maybe_unused]] [[nodiscard]] bool IsEmpty() const;

    [[nodiscard]] bool SameCoords(Clue const &other) const;

    bool IsValid(WordDatabase &db) const;

    bool IsSolved(WordDatabase &db) const;

    [[nodiscard]] std::size_t GetOpenSpots() const;

    [[nodiscard]] int IndexOfCoord(Coord coord) const;

    /**
     * @brief Get whether a clue is ACROSS or DOWN.
     *
     * @return WordDirection
     */
    [[nodiscard]] WordDirection GetDirection() const { return direction_; }

    /**
     * @brief Get the starting coordinate of a clue.
     *
     * @return Coord
     */
    [[nodiscard]] Coord GetStart() const { return start_; }

    /**
     * @brief Get the clue number (i.e. 5 in 5-ACROSS)
     * of a clue.
     *
     * @return int
     */
    [[nodiscard]] int GetNumber() const { return clue_number_; }

    /**
     * @brief Set the Number of a clue.
     *
     * @param num
     */
    void SetNumber(int num) { clue_number_ = num; }

    /**
     * @brief Lock or unlock a clue.
     *
     * @param locked_value
     */
    void SetLocked(bool locked_value) { locked_ = locked_value; }

    /**
     * @brief Get the size (length) of a clue.
     *
     * @return int
     */
    [[nodiscard]] std::size_t GetSize() const { return size_; }

    /**
     * @brief Get the word corresponding to a clue.
     *
     * @return Word
     */
    [[nodiscard]] Word const &ToWord() const;

    [[maybe_unused]] [[nodiscard]] std::string ReprString() const;

    /**
     * @brief Get the constraint at index index.
     *
     * @param index
     * @return Atom
     */
    [[nodiscard]] Atom GetConstraint(const std::size_t index) const { return constraints_[index]; }

    /**
     * @brief Set the constraint at index index.
     *
     * @param index
     */
    void SetConstraint(const std::size_t index, const Atom new_value) { constraints_.atoms_[index] = new_value; }

    /**
     * @brief Construct a new Clue object from parameters.
     *
     * @param direction
     * @param start
     * @param size
     * @param constraints
     * @param coords
     */
    Clue(WordDirection direction, Coord start, std::size_t size, std::vector<Atom> constraints,
         std::vector<Coord> coords)
            : direction_(direction), start_(start), size_(size),
              coord_list_(std::move(coords)), clue_number_(kNO_NUMBER),
              locked_(false), constraints_{constraints} {};

  private:
    /**
     * @brief The current (partially) filled state of the clue.
     */
    Word constraints_;

    /**
     * @brief The clue's direction; is it up or down?
     *
     */
    WordDirection direction_;

    /**
     * @brief The clue's starting coordinates.
     *
     */
    Coord start_;

    /**
     * @brief The number of letters in a clue.
     *
     */
    std::size_t size_;

    /**
     * @brief The clue's identification number in the context of a puzzle.
     *
     */
    int clue_number_;

    /**
     * @brief Flag that is true when every cell of the clue is locked (wrt parent crossword).
     *
     */
    bool locked_;
  };
}

#endif
