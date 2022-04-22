/**
 * @file database.hpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Header file for word database
 * @version 0.1
 * @date 2022-04-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <array>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <memory>

#include "crossword/word.hpp"
#include "crossword/clue.hpp"

namespace crossword_backend {
  struct DatabaseEntry;

  /**
   * @brief Node in a trie
   */
  struct TrieNode : public Obj {
    Atom value;
    std::vector<std::unique_ptr<TrieNode>> children;
    TrieNode* parent;

    void AddChild(const Atom child_value);

    TrieNode *FindChild(const Atom queried_child) const;

    bool IsTerminal() const { return children.empty(); }

    Word LeafToWord() const;

    std::vector<Word> Find(const Word &partial, std::size_t substr_start) const;

    std::string ReprString() const override;

    explicit TrieNode(const Atom value, TrieNode* parent) : value{value}, parent{parent} {};
  };

  /**
   * @brief Fixed size Trie that allows lookup
   * from wildcard words.
   * 
   */
  class WordTrie : public Obj {
  public:
    std::unique_ptr<TrieNode> root;

    void Insert(Word const &entry);

    std::string ReprString() const override {
      return root->ReprString();
    };

    WordTrie() : root{std::make_unique<TrieNode>(TrieNode(Atom(), nullptr))} {};
  };

  /**
   * @brief Hash map from (partial) word-> bool.
   *
   * Contains partial words. TODO: eviction policy.
   *
   */
  struct WordHashMap {
    int count(const Word &w) {
      std::size_t count = map_.count(w);
      if (count == 0) {
        misses++;
      }
      if (count == 1) {
        hits++;
      }
      // if ((hits+misses) % 10000 == 0)
      // {
      //   double hit_rate = static_cast<double>(hits) / static_cast<double>(hits+misses);
      //   LOGEXPR(hit_rate);
      //   LOGEXPR(map_.size());
      // }
      return count;
    }

    bool operator[](const Word &word_partial) {
      assert(map_.find(word_partial) != map_.end());
      return map_[word_partial];
    }

    void clear() {
      map_.clear();
    }

    void insert(const Word &w, bool value) {
      // if above maximum size, start deleting
      if (map_.size() >= max_elements) {
        map_.erase(map_.begin());
      }
      map_.insert(std::pair<Word, bool>(w, value));
    }

    std::unordered_map<Word, bool, WordHash> map_;
    int hits;
    int misses;
    std::size_t max_elements;

    WordHashMap() : hits(0), misses(0), max_elements(10000) {};
  };

  /**
   * @brief 3-tuple of (entry, frequency score, letter score).
   *
   * This is the entry stored in FixedSizeWordDatabase.
   * It is a tuple of a Word and a score which represents
   * how desirable a certain word is.
   */
  struct DatabaseEntry {
    /**
     * @brief Corresponding word.
     *
     */
    Word entry;

    /**
     * @brief Frequency score, normalized to lie between [0, 100].
     *
     */
    int frequency_score;

    /**
     * @brief Letter score, not normalized.
     *
     */
    int letter_score;

    /**
     * @brief Default comparator. Uses letter score.
     *
     * @param e
     * @return true
     * @return false
     */
    bool operator<(const DatabaseEntry &e) const { return letter_score < e.letter_score; };

    DatabaseEntry(Word const &entry, const int frequency_score, const int letter_score) : entry(entry),
                                                                                          letter_score(letter_score),
                                                                                          frequency_score(
                                                                                                  frequency_score) {};
  };

  /**
   * @brief Sub-database of words subject to a particular length.
   *
   */
  class FixedSizeWordDatabase {
  public:
    std::vector<DatabaseEntry> GetSolutions(Clue const &clue, const int limit, const int score_min) const;

    void AddEntry(Word const &entry, const int frequency_score, const int letter_score);

    bool HasSolution(Clue const &clue, const int score_min);

    bool ContainsEntry(Word const &word) const;

    int GetFrequencyScore(Word const &word);

    void NormalizeFrequencyScores();

    void FlushPartialCache();

    /**
     * @brief Set the size of a sub-database.
     *
     * @param size
     */
    void SetSize(const std::size_t word_length) { size_ = word_length; }

    FixedSizeWordDatabase() : size_(0) {};

    /**
     * @brief Contains a mapping from word to frequency score.
     *
     */
    std::unordered_map<Word, int, WordHash> word_set_;

    /**
     * @brief Caches partial words that happen to be valid sub-words.
     *
     * Note: Hashing function may take into account ASCII char values and
     * not our basic encoding. May need to translate to ASCII??
     *
     */
    WordHashMap partial_word_cache_;

    WordTrie trie_;

    /**
     * @brief Contains ordered list of entries, ordered by letter score.
     *
     */
    std::vector<DatabaseEntry> entries_;

  private:
    /**
     * @brief Field representing the size of words contained in sub-database.
     *
     */
    std::size_t size_;
  };

  /**
   * @brief Database of crossword puzzle words.
   *
   */
  class WordDatabase {
  public:
    static void LoadThread(WordDatabase *db, std::string const filename);

    /**
     * @brief Getter for whether the database has completed loading yet.
     * 
     * @return true 
     * @return false 
     */
    bool IsFinishedLoading() { return is_finished_loading_; };

    void WaitForLock();

    void AddEntry(Word const &entry, const int frequency_score, const int letter_score);

    bool ContainsEntry(Word const &word) const;

    bool HasSolution(Clue const &clue, const int score_min);

    int GetFrequencyScore(Word const &word);

    int GetLetterScore(Word const &word) const;

    void LoadFromFile(std::string const &filename);

    void LoadDeferred(std::string const &filename);

    void FlushCaches();

    std::vector<DatabaseEntry> GetSolutions(Clue const &clue, const int limit, const int score_min) const;

    WordDatabase();

  private:
    /**
     * @brief Sub-databases indexed by word size.
     *
     */
    std::array<FixedSizeWordDatabase, kMAX_DIM> databases_;

    /**
     * @brief Starts out false, becomes true when the database is done loading.
     *
     */
    std::atomic<bool> is_finished_loading_;

    /**
     * @brief Locks database write operations.
     */
    std::mutex db_lock_;
  };
}

#endif
