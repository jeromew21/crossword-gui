/**
 * @file word.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Contains implementation of word methods.
 * @version 0.1
 * @date 2022-04-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "crossword/word.hpp"

using namespace crossword_backend;

/**
 * @brief Construct a new word from string.
 *
 * @param word
 */
Word::Word(std::string const &word) {
  atoms_.reserve(word.size());
  for (size_t i = 0; i < word.size(); ++i) {
    atoms_.push_back(Atom(word.substr(i, 1)));
  }
}

/**
 * @brief String value of a word.
 *
 * @return std::string
 */
std::string Word::ToString() const {
  std::string s;
  for (std::size_t i = 0; i < atoms_.size(); ++i) {
    if (atoms_[i].IsEmpty())
      s += " ";
    else
      s += atoms_[i].ToString();
  }
  return s;
}

/**
 * @brief Debug representation of a word.
 *
 * @return std::string
 */
std::string Word::ReprString() const {
  return "Word{" + ToString() + "}";
}


/**
 * @brief Lexical comparator for Word.
 * 
 * @param word 
 * @return true 
 * @return false 
 */
bool Word::operator<(const Word &word) const {
  if (size() != word.size())
    return size() < word.size();

  for (std::size_t i = 0; i < size(); ++i) {
    if (atoms_[i] != word.atoms_[i])
      return atoms_[i] < word.atoms_[i];
  }
  return false;
}

/**
 * @brief Equality comparator between words.
 *
 * @param word
 * @return true
 * @return false
 */
bool Word::operator==(const Word &word) const {
  if (size() != word.size())
    return false;

  for (std::size_t i = 0; i < size(); ++i) {
    if (this->atoms_[i] != word.atoms_[i])
      return false;
  }
  return true;
}
