/**
 * @file word.hpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Contains header for Atom and Word classes.
 * @version 0.1
 * @date 2022-04-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef WORD_H
#define WORD_H

#include "base.hpp"

#include <vector>
#include <array>
#include <assert.h>

namespace crossword_backend {
  /**
   * @brief The number of valid cell contents.
   *
   */
  constexpr std::size_t kATOM_COUNT = 27;

  /**
   * @brief Underlying value corresponding to the empty value.
   *
   */
  constexpr std::size_t kEMPTY_CODE = 0;

  /**
   * @brief Mapping from underlying value to single-character string.
   *
   */
  const std::array<const std::string, kATOM_COUNT> kATOM_MAPPING = {
          "", "A", "B", "C", "D", "E",
          "F", "G", "H", "I", "J", "K",
          "L", "M", "N", "O", "P", "Q",
          "R", "S", "T", "U", "V", "W",
          "X", "Y", "Z"};

  /**
   * @brief Wrapper for value of cell; for now a single, uppercase letter or empty string.
   *
   */
  class Atom {
  public:
    /**
     * @brief String representation of an Atom.
     *
     * @return std::string const&
     */
    std::string const &ToString() const { return kATOM_MAPPING[code]; };

    /**
     * @brief Returns the underlying unsigned integer value of the atom.
     *
     * @return std::size_t
     */
    inline std::size_t GetCode() const { return code; };

    /**
     * @brief Returns whether or not an atom represents the empty string.
     *
     * @return true
     * @return false
     */
    inline bool IsEmpty() const { return code == kEMPTY_CODE; };

    /**
     * @brief Default constructor entails an empty string.
     *
     */
    Atom() : code(0) {};

    /**
     * @brief Constructs a new Atom from a string.
     *
     * Runtime error if value is not valid.
     *
     * @param value [A-Z] or empty string.
     */
    Atom(std::string const &value) {
      for (std::size_t i = 0; i < kATOM_COUNT; ++i) {
        if (kATOM_MAPPING[i] == value) {
          code = i;
          return;
        }
      }
      code = 0;
      assert(false);
    }

    /**
     * @brief Equality operator between atoms.
     *
     * @param a
     * @return true
     * @return false
     */
    inline bool operator==(const Atom &a) const { return code == a.code; };

    /**
     * @brief Not equal operator between atoms.
     *
     * @param a
     * @return true
     * @return false
     */
    inline bool operator!=(const Atom &a) const { return code != a.code; };

    /**
     * @brief Comparison operator between atoms.
     *
     * @param a
     * @return true
     * @return false
     */
    inline bool operator<(const Atom &a) const { return code < a.code; };

  private:
    /**
     * @brief Underlying unsigned integer representation.
     *
     */
    std::size_t code;
  };

  /**
   * @brief A complete or partial word built from some atoms.
   * 
   * Mirrors somewhat the std::vector API.
   *
   */
  class Word {
  public:
    /**
     * @brief Underlying vector of atoms that comprise a word.
     *
     * TODO: store this on the stack? small vector optimization
     *
     */
    std::vector<Atom> atoms_;

    std::string ToString() const;

    std::string ReprString() const;

    /**
     * @brief Return the size of the word; i.e. the size of the underlying vector.
     *
     * Includes any empty letters.
     *
     * @return int Exact length of word.
     */
    std::size_t size() const { return atoms_.size(); };

    Word(std::string const &word);

    /**
     * @brief Construct a new Word object from vector of atoms.
     *
     * Takes ownership of the vector.
     *
     * @param vec
     */
    Word(std::vector<Atom> const &vec) : atoms_{std::move(vec)} {};

    /**
     * @brief Constructs an empty word.
     *
     */
    Word() {};

    bool operator==(const Word &word) const;

    bool operator<(const Word &word) const;

    /**
     * @brief Subscript operator for word.
     *
     * @param index
     * @return Atom
     */
    Atom operator[](const std::size_t index) const { return atoms_[index]; };
  };

  /**
   * @brief Hashing methods for Word
   *
   */
  class WordHash {
  public:
    /**
     * @brief Return the 32-bit hash of a given word.
     * 
     * TODO: make sure collisions aren't common.
     *
     * @param word Word object to hash
     * @return std::size_t 32-bit hash.
     */
    std::size_t operator()(Word const &word) const {
      std::size_t h{0};

      for (auto const &atom: word.atoms_)
        h = (37 * h) + (atom.GetCode() + 64); // convert code to ASCII
      return h;
    }
  };
}

#endif
