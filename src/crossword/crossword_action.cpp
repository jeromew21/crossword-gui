/**
 * @file crossword_action.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Crossword action implementations
 * @version 0.1
 * @date 2022-04-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "crossword/crossword.hpp"

using namespace crossword_backend;

/**
 * @brief Apply a cell change to a crossword.
 *
 * @param crossword
 */
void CrosswordSetAction::Apply(Crossword &crossword) const {
  crossword.Set_(new_value_, coord_);
}

/**
 * @brief Undo a crossword cell change.
 *
 * @param crossword
 */
void CrosswordSetAction::Invert(Crossword &crossword) const {
  crossword.Set_(old_value_, coord_);
}

/**
 * @brief Apply a group of actions.
 *
 * @param crossword
 */
void CrosswordActionGroup::Apply(Crossword &crossword) const {
  for (auto it = std::begin(actions_); it != std::end(actions_); ++it) {
    (*it)->Apply(crossword);
  }
}

/**
 * @brief Invert a group of actions.
 *
 * @param crossword
 */
void CrosswordActionGroup::Invert(Crossword &crossword) const {
  for (auto it = actions_.rbegin(); it != actions_.rend(); ++it) {
    (*it)->Invert(crossword);
  }
}

/**
 * @brief Add an action to a group of actions.
 *
 * Takes *unique* ownership of the action. Best practice is to directly call "new" when invoking this method.
 *
 * @param action
 */
void CrosswordActionGroup::AddAction(CrosswordAction *action) {
  // Note to reader: this works because the value we push into the vector is temporary (rvalue)? (citation needed)
  actions_.push_back(std::unique_ptr<CrosswordAction>(action));
}

/**
 * @brief Add action to the stack
 *
 * Takes unique ownership of reference.
 *
 * @param action
 */
void CrosswordActionStack::Push(CrosswordAction *action) {
  if (stack_.size() > index_) {
    // erase+insert because we want to ensure object destruction
    stack_.erase(stack_.begin() + index_, stack_.end());
    stack_.insert(stack_.begin() + index_, std::unique_ptr<CrosswordAction>(action));

    index_++;
  } else {
    stack_.push_back(std::unique_ptr<CrosswordAction>(action));
    index_++;
  }
}

/**
 * @brief Moves back one action in history.
 *
 * Returns true iff there is an action to undo.
 *
 */
bool CrosswordActionStack::Pop(Crossword &crossword) {
  if (IsEmpty()) {
    return false;
  }
  index_--;
  stack_[index_]->Invert(crossword);
  return true;
}

/**
 * @brief Goes forward one action in history.
 *
 * Returns true iff there is an action to redo.
 *
 * @param crossword
 */
bool CrosswordActionStack::Replace(Crossword &crossword) {
  if (stack_.size() > index_) {
    stack_[index_]->Apply(crossword);
    index_++;
    return true;
  } else {
    return false;
  }
}

/**
 * @brief Checks whether the stack is empty.
 *
 * @return true
 * @return false
 */
bool CrosswordActionStack::IsEmpty() const {
  return index_ == 0;
}

/**
 * @brief Construct a new CrosswordActionGroup object
 * corresponding to an entire word fill.
 *
 * @param clue
 * @param word
 */
CrosswordActionGroup::CrosswordActionGroup(Crossword const &crossword, Clue const &clue, Word const &word) {
  assert(clue.GetSize() == word.size());
  actions_.reserve(clue.GetSize());
  for (std::size_t i = 0; i < word.size(); ++i) {
    Atom val = word[i];
    Coord coord = clue.coord_list_[i];
    Atom old_val = crossword.Get(coord).GetContents();
    assert(old_val == clue.GetConstraint(i));
    if (clue.GetConstraint(i).IsEmpty()) {
      AddAction(new CrosswordSetAction(val, old_val, coord));
    }
  }
}
