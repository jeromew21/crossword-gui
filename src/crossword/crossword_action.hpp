#ifndef CROSSWORD_ACTION_HPP
#define CROSSWORD_ACTION_HPP

#include "crossword/clue.hpp"

#include <vector>
#include <memory>


namespace crossword_backend {
  class Crossword;

  /**
   * @brief Abstract class representing an action applied to the crossword.
   *
   * Action needs to be invertible; i.e.
   * the identity Invert(Apply(X)) = X must hold
   * and ideally the commutative Apply(Invert(X)) = X holds as well.
   *
   */
  class CrosswordAction {
  public:
    /**
     * @brief Apply an action on the crossword.
     *
     * @param crossword
     */
    virtual void Apply(Crossword &crossword) const = 0;

    /**
     * @brief Invert an action on the crossword.
     *
     * @param crossword
     */
    virtual void Invert(Crossword &crossword) const = 0;

    /**
     * @brief Implementation for "pure virtual" destructor.
     *
     * Since class is abstract, this does nothing.
     *
     */
    virtual ~CrosswordAction() = default;
  };

  /**
   * @brief Action corresponding to a Crossword.Set() operation (i.e.) a single cell change.
   *
   */
  class CrosswordSetAction : public CrosswordAction {
  public:
    void Apply(Crossword &crossword) const override;

    void Invert(Crossword &crossword) const override;

    /**
     * @brief Constructor for CrosswordSetAction
     *
     * @param new_value value to set to
     * @param old_value previous value (to restore)
     * @param coord coordinate to change
     */
    CrosswordSetAction(Atom new_value, Atom old_value, Coord coord) : coord_(coord), old_value_(old_value),
                                                                      new_value_(new_value) {};

    /**
     * @brief Default destructor
     *
     */
    ~CrosswordSetAction() override = default;

  private:
    /**
     * @brief Coordinate to set
     *
     */
    Coord coord_;

    /**
     * @brief Value to set to.
     *
     */
    Atom new_value_;

    /**
     * @brief Previous value at coordinate
     *
     */
    Atom old_value_;
  };

  /**
   * @brief Class representing an ordered collection of actions applied to a crossword.
   *
   * Needs to be invertible and performant as this will be used in the DFS implementation.
   * Applied in order of element addition. Inverted in opposite order.
   *
   * For performance the goal is to make this as tiny as possible.
   * We may want to create a new class that doesn't use heap vectors for marginally better cache performance.
   *
   *
   */
  class CrosswordActionGroup : public CrosswordAction {
  public:
    void Apply(Crossword &crossword) const override;

    void Invert(Crossword &crossword) const override;

    void AddAction(CrosswordAction *action);

    /**
     * @brief Instantiate an empty action group.
     * Can be used as a dummy (null) action.
     *
     */
    CrosswordActionGroup() = default;

    CrosswordActionGroup(Crossword const &crossword, Clue const &clue, Word const &word);

    /**
     * @brief Default destructor.
     *
     */
    ~CrosswordActionGroup() override = default;

  private:
    /**
     * @brief Underlying vector of actions.
     *
     * Holds unique references to each child.
     *
     */
    std::vector<std::unique_ptr<CrosswordAction>> actions_;
  };

  /**
   * @brief Data structure representing the history and future of actions applied to a crossword.
   *
   */
  class CrosswordActionStack {
  public:
    void Push(CrosswordAction *action);

    bool Pop(Crossword &crossword);

    bool Replace(Crossword &crossword);

    [[nodiscard]] bool IsEmpty() const;

    /**
     * @brief Get the current size of the stack.
     *
     * @return int
     */
    [[nodiscard]] std::size_t GetSize() const { return index_; };

    /**
     * @brief Construct a new Crossword Action Stack object
     *
     */
    CrosswordActionStack() : index_(0) {};

  private:
    /**
     * @brief Head index of stack.
     *
     * Equal to current number of elements in the stack, and points to one beyond the current final element.
     * Initialized at zero.
     *
     */
    std::size_t index_;

    /**
     * @brief Underlying stack, implemented with a vector.
     *
     */
    std::vector<std::unique_ptr<CrosswordAction>> stack_;
  };
}

#endif
