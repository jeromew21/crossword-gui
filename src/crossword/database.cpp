/**
 * @file database.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Database method implementations
 * @version 0.1
 * @date 2022-04-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "crossword/database.hpp"

#include <algorithm>
#include <random>
#include <thread>
#include <sstream>

using namespace crossword_backend;

/**
 * @brief Letter frequency scores corresponding to each atom.
 *
 * Calculated as a fraction of each letter appearance in the default database.csv
 *
 * We break the abstraction barrier a little bit here.
 *
 */
static const std::array<const double, kATOM_COUNT> ATOM_SCORES = {
        0, 0.09062575314771874, 0.02097899760363229, 0.03434604298486668,
        0.037221082553848074, 0.11781545127357801, 0.016996862225737236,
        0.025051385357254, 0.032380546802375045, 0.06766254035033298,
        0.0028382700825742386, 0.013840703219770217, 0.05106980820701622,
        0.02758446870264884, 0.06579765885337364, 0.07380371311644462,
        0.026900221987417483, 0.0014162937627920208, 0.07111285524755726,
        0.07549811790369804, 0.07228279345142133, 0.028721314368746472,
        0.009132871887421193, 0.014259541096753555, 0.002845289711797423,
        0.017204776957966794, 0.002612639143257596};

/**
 * @brief Return true if the clue (partially filled) exists in the database, with score greater or equal to score_min.
 *
 * @param clue
 * @param score_min
 * @return true
 * @return false
 */
bool FixedSizeWordDatabase::HasSolution(Clue const &clue, const int score_min) {
  Word const &clue_partial = clue.ToWord();
  int count = partial_word_cache_.count(clue_partial);
  if (count == 1) {
    return partial_word_cache_[clue_partial];
  }
  for (auto it = std::begin(entries_); it != std::end(entries_); ++it) {
    // BUG!!! cache loses score information
    // for now just flush cache more often
    if (it->frequency_score >= score_min && clue.FitsWord(it->entry)) {
      partial_word_cache_.insert(clue_partial, true);
      return true;
    }
  }
  partial_word_cache_.insert(clue_partial, false);
  return false;
}

/**
 * @brief Calculate the letter score for a given word.
 *
 * The idea here is want finer granularity when it comes to sorting child nodes in search,
 * and not leave it up to the human error and bias that might exist in the frequency score.
 *
 *
 * @param word
 * @return int
 */
int WordDatabase::GetLetterScore(Word const &word) const {
  double score = 0;
  std::vector<Atom> unique;
  for (auto const &atom: word.atoms_) {
    score += ATOM_SCORES[atom.GetCode()] * 1000.;
    bool flag = true;
    for (auto const &found: unique) {
      if (found == atom) {
        flag = false;
        break;
      }
    }
    if (flag)
      unique.push_back(atom);
  }
  score *= unique.size();
  return static_cast<int>(score);
}

/**
 * @brief Add a new entry to the sub-database.
 *
 * TODO: weighted average of score and letter score?
 * TODO: should we add some partials ...?
 *
 * @param entry
 * @param score
 */
void FixedSizeWordDatabase::AddEntry(Word const &entry, const int frequency_score, const int letter_score) {
  assert(entry.size() == size_);
  auto db_entry = DatabaseEntry(entry, frequency_score, letter_score);
  entries_.push_back(db_entry);
  word_set_[entry] = frequency_score;
  trie_.Insert(db_entry.entry);
}

/**
 * @brief Reverse search the database for a word.
 *
 * Fast, because it checks a set.
 *
 * @param word
 * @return true contains word
 * @return false does not contain word
 */
bool WordDatabase::ContainsEntry(Word const &word) const {
  return databases_[word.size()].ContainsEntry(word);
}

/**
 * @brief Reverse search the database for a word.
 *
 * Fast because it uses a map.
 *
 * @param word
 * @return true
 * @return false
 */
bool FixedSizeWordDatabase::ContainsEntry(Word const &word) const {
  return word_set_.find(word) != word_set_.end();
}

/**
 * @brief
 *
 * @param entry
 * @param score
 */
void WordDatabase::AddEntry(Word const &entry, const int frequency_score, const int letter_score) {
  databases_[entry.size()].AddEntry(entry, frequency_score, letter_score);
}

/**
 * @brief Get the frequency score for a word in the database.
 *
 * @param word
 * @return int
 */
int FixedSizeWordDatabase::GetFrequencyScore(Word const &word) const {
  assert(ContainsEntry(word));
  return word_set_.find(word)->second;
}

/**
 * @brief Get the score corresponding to a particular word's frequency
 * in past crossword puzzles.
 *
 * @param word
 */
int WordDatabase::GetFrequencyScore(Word const &word) const {
  return databases_[word.size()].GetFrequencyScore(word);
}

/**
 * @brief Returns true iff one or more solutions to the clue exist in the database, with score greater than or equal to score_min.
 *
 * @param clue
 * @return true
 * @return false
 */
bool WordDatabase::HasSolution(Clue const &clue, const int score_min) {
  return databases_[clue.GetSize()].HasSolution(clue, score_min);
}

/**
 * @brief Normalize word frequency scores to [1, 100].
 *
 * TODO: Fix to make distribution better.
 *
 */
void FixedSizeWordDatabase::NormalizeFrequencyScores() {
  if (entries_.empty())
    return;

  double total_raw = 0;
  int max_raw = 0;
  double N = static_cast<double>(entries_.size());
  Word max_word = Word();
  for (auto it = std::begin(entries_); it != std::end(entries_); ++it) {
    total_raw += it->frequency_score;
    if (max_raw < it->frequency_score)
      max_word = it->entry;
    max_raw = std::max(max_raw, it->frequency_score);
  }
  double mean = total_raw / N;

  double total_sq_dev = 0;
  for (auto it = std::begin(entries_); it != std::end(entries_); ++it) {
    double sq_dev = std::pow(static_cast<double>(it->frequency_score) - mean, 2);
    total_sq_dev += sq_dev;
  }
  double sd = std::sqrt(total_sq_dev / N);

  // Log("");
  // LOGEXPR(mean);
  // LOGEXPR(max_raw);
  // LOGEXPR(max_word);

  double max_sigma = 1;
  double min_sigma = 2; // divide by more to make the left side of the distribution closer to avg.

  for (auto it = std::begin(entries_); it != std::end(entries_); ++it) {
    double sigma = (static_cast<double>(it->frequency_score) - mean) / sd;
    if (sigma > 0)
      sigma = sigma / max_sigma;
    else
      sigma = sigma / min_sigma; // negative, squish
    sigma = std::min(1., std::max(-1., sigma));

    // Sigma should now be in [-1, 1]

    // Center at 50
    double new_score = 50. + (50. * sigma);

    // Truncate into [1, 100]
    new_score = std::min(100., std::max(1., new_score));

    it->frequency_score = static_cast<int>(new_score);
    word_set_[it->entry] = it->frequency_score;
  }
}

/**
 * @brief Get solutions for a given clue.
 *
 * @param clue
 * @param limit
 * @param score_min
 * @return std::vector<Word>
 */
std::vector<Word> WordDatabase::GetSolutions(Clue const &clue, const int limit, const int score_min) const {
  return databases_[clue.GetSize()].GetSolutions(clue, limit, score_min);
}

/**
 * @brief Get solutions for a given clue, for a sub-database.
 *
 * 60% hotspot.
 * 
 * TODO: do we want to return words instead of entries... do we need to pull entire entry as well as word???
 *
 * @param clue
 * @param limit
 * @param score_min
 * @return std::vector<DatabaseEntry>
 */
std::vector<Word>
FixedSizeWordDatabase::GetSolutions(Clue const &clue, const int limit, const int score_min) const {
//  std::vector<DatabaseEntry> vec;
//  if (limit != kNO_NUMBER) {
//    vec.reserve(limit);
//  } else {
//    vec.reserve(100); //TODO: test performance?
//  }
//  std::size_t entry_count = entries_.size();
//  for (std::size_t i = 0; i < entry_count; ++i) {
//    DatabaseEntry const &entry = entries_[i];
//    if (entry.frequency_score >= score_min && clue.FitsWord(entry.entry)) {
//      vec.push_back(entry);
//    }
//    if (limit != kNO_NUMBER && i >= (std::size_t) limit)
//      break;
//  }
//  return vec;
  const Word clue_partial = clue.ToWord();
  std::vector<Word> all_solutions = trie_.Find(clue_partial);
  std::vector<Word> passing_solutions{};

  for (auto const &solution_word : all_solutions) {
    if (true || GetFrequencyScore(solution_word) >= score_min) passing_solutions.push_back(solution_word);
  }

  return passing_solutions;
}

/**
 * @brief Wait for database to complete loading.
 *
 */
void WordDatabase::WaitForLock() {
  const std::lock_guard<std::mutex> lock(db_lock_);
}

/**
 * @brief Flush sub-database partial word cache.
 *
 */
void FixedSizeWordDatabase::FlushPartialCache() {
  partial_word_cache_.clear();
}

/**
 * @brief Flush all partial word caches.
 *
 */
void WordDatabase::FlushCaches() {
  const std::lock_guard<std::mutex> lock(db_lock_);
  for (std::size_t i = 0; i < kMAX_DIM; ++i) {
    databases_[i].FlushPartialCache();
  }
}

/**
 * @brief Load thread function
 *
 * @param db
 * @param filename
 */
void WordDatabase::LoadThread(WordDatabase *db, std::string const filename) {
  const std::lock_guard<std::mutex> lock(db->db_lock_);
  db->LoadFromFile(filename);
}

/**
 * @brief Load entries in a new std::thread which we detach.
 *
 */
void WordDatabase::LoadDeferred(std::string const &filename) {
  is_finished_loading_ = false;
  std::thread loader(LoadThread, this, filename);
  loader.detach();
}

/**
 * @brief Load entries from file to the database.
 *
 * TODO: error checking.
 *
 * @param filename
 */
void WordDatabase::LoadFromFile(std::string const &filename) {
  std::string line;
  std::ifstream db(filename);
  if (db.is_open()) {
    while (std::getline(db, line)) {
      for (std::size_t i = 0; i < line.size(); i++) {
        if (line.at(i) == ' ') {
          std::size_t size = i;
          std::string word = line.substr(0, i);
          int score = std::stoi(line.substr(i + 1, line.size() - (i + 1)));

          if (size < kMAX_DIM) {
            Word word_obj = Word(word);
            databases_[size].AddEntry(word_obj, score, GetLetterScore(word_obj));
          }
        }
      }
    }
  } else {
    return;
  }

  for (std::size_t i = 0; i < kMAX_DIM; ++i) {
    databases_[i].NormalizeFrequencyScores();
    std::sort(databases_[i].entries_.begin(), databases_[i].entries_.end());
    std::reverse(databases_[i].entries_.begin(), databases_[i].entries_.end());
  }
  is_finished_loading_ = true;
}

/**
 * @brief Initialize a new WordDatabase object.
 *
 */
WordDatabase::WordDatabase() : is_finished_loading_{false} {
  for (std::size_t i = 0; i < kMAX_DIM; ++i) {
    databases_[i].SetSize(i);
  }
}
