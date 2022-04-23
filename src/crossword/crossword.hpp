/**
 * @file crossword.hpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Header for crossword backend
 * @version 0.1
 * @date 2022-04-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CROSSWORD_H
#define CROSSWORD_H

#include <string>
#include <sstream>
#include <vector>
#include <memory>
#include <iostream>
#include <mutex>
#include <cassert>

#include "crossword/logging.hpp"
#include "crossword/base.hpp"
#include "crossword/database.hpp"
#include "crossword/clue.hpp"
#include "crossword/crossword_action.hpp"
#include "crossword/search.hpp"

/**
 * @brief Namespace for puzzle backend.
 * 
 */
namespace crossword_backend {
  /**
   * @brief Object representing the state of a crossword grid cell.
   *
   */
  class Cell {
  public:
    [[nodiscard]] Atom GetContents() const;

    [[nodiscard]] bool IsBarrier() const;

    [[nodiscard]] bool IsLocked() const;

    void SetBarrier(bool val);

    void SetContents(Atom atom);

    void Lock(bool value);

    [[nodiscard]] std::string ToString() const;

    [[maybe_unused]] [[nodiscard]] std::string ReprString() const;

    /**
     * @brief Construct a new Cell object.
     *
     * By default, initialized to an empty, non-barrier cell.
     *
     */
    Cell() : is_barrier(false), contents(Atom()), locked(false) {};

  private:
    /**
     * @brief True iff this cell is a barrier (blacked-out) cell.
     */
    bool is_barrier;

    /**
     * @brief True iff this cell is locked.
     *
     */
    bool locked;

    /**
     * @brief The value of the cell.
     *
     */
    Atom contents;
  };

  /**
   * @brief Enum representing the state of a crossword with regards
   * to its (potential) solution.
   *
   */
  enum class Solvability {
    /**
     * @brief Means what you think it does.
     *
     */
    Solvable,

    /**
     * @brief Crossword is unsolvable given the database.
     *
     */
    Overdetermined,

    /**
     * @brief Crossword has an invalid filled word.
     *
     */
    Invalid,

    /**
     * @brief Crossword has a duplicate word.
     *
     */
    Duplicate,

    /**
     * @brief Overdetermined by score minimum. Generally not applicable.
     *
     */
    Weak,
  };

  /**
   * @brief Cache that holds clues.
   *
   * Currently, the cache cares not about what contents (constraints) of the clues are, but about numbering and stuff.
   *
   * Motivation for this is firstly the UI elements being rather sluggish.
   * When we optimize search this should gain the functionality of clue constraints as well.
   *
   * Does incremental update when state changes.
   *
   */
  struct ClueStructure {
    /**
     * @brief Owns clue objects
     *
     */
    std::vector<Clue> clues;

    /**
     * @brief Mapping from index to clue number.
     *
     */
    std::array<std::array<ClueNumber, kMAX_DIM>, kMAX_DIM> numberings;

    /**
     * @brief Mapping from coordinate to list of pointers to clues starting at that location.
     *
     */
    std::array<std::array<std::vector<Clue *>, kMAX_DIM>, kMAX_DIM> cell_mapping;

    /**
     * @brief True iff the structure needs to be recalculated.
     *
     */
    bool dirty;

    /**
     * @brief Default constructor.
     *
     */
    ClueStructure() : numberings{}, dirty(true) {};
  };

  /**
   * @brief Class representing a single crossword puzzle.
   *
   */
  class Crossword {
  public:
    /**
     * @brief Owned logging object.
     *
     */
    Logger logger;

    /**
     * @brief Flags search interruption.
     *
     */
    std::atomic<bool> stop_searching;

    /**
     * @brief Flags completion of search.
     *
     */
    std::atomic<bool> done_searching;

    /* Autofill related */
    void Autofill(AutofillParams &params);

    void StopAutofill();

    [[nodiscard]] std::vector<CrosswordActionGroup *>
    GetWordFills(std::vector<Clue> const &all_clues, AutofillParams const &params) const;

    bool IsSolved(std::vector<Clue> const &all_clues, WordDatabase &db) const;

    Solvability IsInvalidPartial(std::vector<Clue> const &all_clues, WordDatabase &db, int score_min) const;

    /* Action methods (stack changers) */
    bool Undo();

    bool Redo();

    void Set(Atom val, Coord coord);

    void SetClue(Clue const &clue, Word const &word);

    void ClearAtoms();

    void ClearClue(Clue const &clue);

    /* Parameter methods (might be annoying to interact with stack, TODO: implement) */
    void SetBarrier(bool val, Coord coord, bool enforce_symmetry);

    void SetDimensions(std::size_t height, std::size_t width);

    void LockCell(Coord coord, bool value);

    void ToggleBarrier(Coord coord, bool enforce_symmetry);

    void ToggleLockCell(Coord coord);

    /* Fundamental action methods that don't affect stack */
    void Set_(Atom val, Coord coord);

    /* Const getters */
    [[nodiscard]] bool IsFilled(Coord coord) const;

    [[nodiscard]] bool IsLocked(Coord coord) const;

    [[nodiscard]] bool InBounds(Coord coord) const;

    [[nodiscard]] bool IsValidPattern() const;

    [[nodiscard]] Cell Get(Coord coord) const;

    [[nodiscard]] Cell Get(std::size_t row, std::size_t col) const;

    [[maybe_unused]] [[nodiscard]] std::string ReprString() const;

    /**
     * @brief Get the height of the crossword grid.
     *
     */
    [[nodiscard]] std::size_t GetHeight() const { return height_; };

    /**
     * @brief Get the width of the crossword grid.
     *
     */
    [[nodiscard]] std::size_t GetWidth() const { return width_; };

    /* Cached clues */
    [[nodiscard]] std::vector<Clue> const &Clues() const;

    [[nodiscard]] std::vector<Clue> CluesStartingAt(Coord coord) const;

    [[nodiscard]] ClueNumber GetClueNumber(Coord coord) const;

    /* Hint-related TODO: make private certain overloads and cleanup API */
    [[nodiscard]] std::string GetHint(ClueNumber num, WordDirection direction) const;

    [[nodiscard]] std::string GetHint(Coord coord, WordDirection direction) const;

    [[nodiscard]] std::string GetHint(Clue const &clue) const;

    void SetHint(Clue const &clue, std::string const &hint);

    [[maybe_unused]] void SetHint(ClueNumber num, WordDirection direction, std::string const &hint);

    void SetHint(Coord coord, WordDirection direction, std::string const &hint);

    /* Import/Export related */
    [[nodiscard]] std::vector<std::string> Serialize() const;

    void Unserialize(std::vector<std::string> &lines);

    /**
     * @brief Construct a new Crossword object.
     *
     * Defaults to a width of kSTART_WIDTH and a
     * height of kSTART_HEIGHT.
     *
     */
    Crossword() : height_(kSTART_HEIGHT), width_(kSTART_WIDTH), done_searching{false}, stop_searching{false} {
      PopulateClueStructure();
    }

  private:
    /**
     * @brief An array that stores underlying Cell objects
     * that comprise the crossword puzzle.
     *
     */
    std::array<std::array<Cell, kMAX_DIM>, kMAX_DIM> grid_;

    /**
     * @brief The height of the crossword puzzle.
     *
     */
    std::size_t height_;

    /**
     * @brief The width of the crossword puzzle.
     *
     */
    std::size_t width_;

    /**
     * @brief Owned clue cache.
     *
     */
    ClueStructure clue_cache_;

    /**
     * @brief An array that stores underlying strings
     * that represent puzzle clues.
     *
     * We two clues per cell, representing one for
     * ACROSS and one for DOWN.
     *
     */
    std::array<std::array<std::array<std::string, 2>, kMAX_DIM>, kMAX_DIM>
            clue_strings_;

    /**
     * @brief Action stack object for the crossword.
     *
     */
    CrosswordActionStack action_stack_;

    void PopulateClueStructure();

    void DirtyClueStructure();

    /* Raw clue calculation methods */
    [[nodiscard]] std::vector<Clue> Clues_() const;

    std::vector<Clue *> CluesStartingAt_(std::vector<Clue> &all_clues, Coord coord) const;

    ClueNumber GetClueNumber_(std::vector<Clue> &all_clues, Coord coord) const;

    void ApplyAction(CrosswordAction *action);

    [[nodiscard]] Coord GetRotationalPair(Coord coord) const;

    [[nodiscard]] std::vector<Clue> UnfilteredClues(WordDirection direction) const;
  };

}

#endif
