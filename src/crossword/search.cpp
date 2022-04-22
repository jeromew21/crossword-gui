/**
 * @file solver.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Methods for search
 * @version 0.1
 * @date 2022-04-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "crossword/crossword.hpp"

#include <thread>
#include <chrono>
#include <atomic>
#include <map>
#include <algorithm>
#include <random>

#if defined(__gnu_linux__)

#include "valgrind/callgrind.h"

#else
const int CALLGRIND_TOGGLE_COLLECT = 0;
const int CALLGRIND_START_INSTRUMENTATION = 0;
const int CALLGRIND_STOP_INSTRUMENTATION = 0;
#endif

using namespace crossword_backend;

/**
 * @brief Returns true iff crossword is a valid solution.
 *
 * Equivalent to "Accept" from backtracking literature.
 *
 * Optimization: This is called on every validated node.
 * The behavior should not change here.
 *
 * @return true
 * @return false
 */
bool Crossword::IsSolved(std::vector<Clue> const &all_clues, WordDatabase &db) const {
  for (auto it = std::begin(all_clues); it != std::end(all_clues); ++it) {
    if (!it->IsSolved(db))
      return false;
  }
  return true;
}

/**
 * @brief Returns true iff current state should not be searched further for a solution.
 *
 * Limits solution space to words of score_min or higher.
 *
 * Equivalent to "Reject" from backtracking literature (i.e. Wikipedia)
 *
 * Top-1 hotspot.
 *
 * @return true state can be eliminated as possible solution
 * @return false state is a potential solution
 */
Solvability
Crossword::IsInvalidPartial(std::vector<Clue> const &all_clues, WordDatabase &db, const int score_min) const {
  for (auto it = std::begin(all_clues); it != std::end(all_clues); ++it) {
    if (it->IsFilled()) {
      if (it->IsLocked())
        continue; // filled and locked is OK
      if (!it->IsSolved(db)) {
        return Solvability::Invalid; // clue is solved incorrectly
      } else {
        // solved correctly, but we want to eliminate words with worse score than score_min
        Word w = it->ToWord();
        if (db.GetFrequencyScore(w) < score_min) {
          return Solvability::Weak; // solved with an unsatisfactory score
        }
      }
      // otherwise, it's solved to our standards, so we go forth
    } else {
      // not filled
      if (!db.HasSolution(*it, score_min)) {
        return Solvability::Overdetermined;
      }
      // otherwise, it can be potentially filled, so we are onwards.
    }
  }

  // create set to eliminate solutions with duplicate words
  // do we want to do this before or after the previous? guessing after
  // if this ends up being hotspot we can TODO: make it incrementally updated
  std::map<Word, int> word_set;
  for (auto it = std::begin(all_clues); it != std::end(all_clues); ++it) {
    if (it->IsFilled()) {
      Word word = it->ToWord();
      if (word_set.find(word) == word_set.end()) {
        word_set[word] = 0;
      }
      word_set[word] += 1;
      if (word_set[word] >= 2) {
        return Solvability::Duplicate;
      }
    }
  }

  return Solvability::Solvable;
}

/**
 * @brief Returns actions corresponding to possible word fills, subject to a minimum score.
 *
 * Caller is responsible to clean up each element which are allocated on the heap.
 *
 * This is clearly not a comprehensive list of possible actions; it is only the set of
 * possible words we can fill.
 *
 * Right now, it seems emprically that doing it
 * this way, where we only give out fills for exactly one clue,
 * is the best search method.
 *
 * @return std::vector<CrosswordActionGroup *>
 */
std::vector<CrosswordActionGroup *>
Crossword::GetWordFills(std::vector<Clue> const &all_clues, AutofillParams const &params) const {
  double entropy = params.entropy;
  int limit = params.branching_factor_limit;
  WordDatabase &db = *params.db;
  int score_min = params.score_min;

  assert(0 <= entropy && entropy <= 100);

  // TODO: move this to global
  auto rng = std::default_random_engine{};
  rng.seed(static_cast<unsigned int>(time(0)));

  std::vector<Clue> clues = all_clues;

  // Sort by distance to upper left. TODO: put this in the GetClues method and cache it.
  // TODO: manhatten or Euclidean? and break ties properly
  std::sort(clues.begin(), clues.end(), [](Clue const &clue_a, Clue const &clue_b) {
    Coord a = clue_a.GetStart();
    Coord b = clue_b.GetStart();
    if (a == b) return clue_a.GetDirection() == kACROSS;

    double da = a.row + a.col;//std::sqrt(static_cast<double>(a.row*a.row + a.col*a.col));
    double db = b.row + b.col;//std::sqrt(static_cast<double>(b.row*b.row + b.col*b.col));

    if (da == db) {
      if (a.row == b.row) {
        return clue_a.GetDirection() == kACROSS;
      }
      return a.row < b.row;
    }

    return da < db;
  });

  std::vector<CrosswordActionGroup *> actions;
  if (limit != kNO_NUMBER)
    actions.reserve(limit);
  else
    actions.reserve(100); // TODO: fix pre allocation

  int i = 0;
  for (auto clue_it = std::begin(clues); clue_it != std::end(clues); ++clue_it) {
    if (clue_it->IsFilled())
      continue;

    std::vector<Word> sols = db.GetSolutions(*clue_it, kNO_NUMBER, score_min);
    // assume sorted; should be guranteed...

    // Apply randomness as parameterized by entropy
    std::size_t shuffle_count = static_cast<std::size_t>(std::min(1., entropy / 100.) *
                                                         static_cast<double>(sols.size()));
    assert(shuffle_count <= sols.size());
    std::shuffle(std::begin(sols), std::begin(sols) + shuffle_count, rng);

    for (auto entry = std::begin(sols); entry != std::end(sols); ++entry) {
      CrosswordActionGroup *group = new CrosswordActionGroup(*this, *clue_it, *entry);
      actions.push_back(group);

      if (limit != kNO_NUMBER && i >= limit)
        break;
      i++;
    }
    break; // Only allow the first open slot to be filled, as noted above.
  }
  return actions;
}

/**
 * @brief Thread function that stops searching the crossword after some time.
 *
 * Exits early if done_searching becomes true.
 *
 * @param args
 */
void StopSearchingAfterTime(Crossword *crossword, int msecs) {
  for (int i = 0; i < msecs; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    if (crossword->done_searching || crossword->stop_searching)
      return;
  }
  crossword->StopAutofill();
}

/**
 * @brief Stop the autofilling method in progress.
 *
 * Not so well defined if there is no autofill going on.
 * Shouldn't be harmful either way.
 *
 */
void Crossword::StopAutofill() {
  stop_searching = true;
}

/**
 * @brief Run tree search algorithm.
 * 
 * Database must be finished loading.
 *
 * TODO: use monte carlo algorithm to perform pertubations. measure performance
 * TODO: assure we do not revisit nodes? (hashing? does it depend on node ordering?)
 *
 * @param params search parameters
 */
void Crossword::Autofill(AutofillParams &params) {
  auto all_clues = Clues();
  assert(params.db != NULL);
  assert(params.db->IsFinishedLoading());
  assert(IsValidPattern());
  assert(IsInvalidPartial(all_clues, *params.db, 1) == Solvability::Solvable);

  logger.Log("Autofilling...");

  WordDatabase &db = *params.db;

  int *seconds_limit = &params.seconds_limit;
  int *hard_min = &params.score_min;
  double *hard_min_decay = &params.score_min_decay;
  int *entropy = &params.entropy;
  double *entropy_decay = &params.entropy_decay; // TODO: make this a function
  int *branching_factor_limit = &params.branching_factor_limit;
  bool *rollback = &params.rollback;


  stop_searching = false;
  done_searching = false;
  std::thread stop_thread(StopSearchingAfterTime, this, *seconds_limit * 1000);

  std::vector<Coord> locked_coords{};
  for (std::size_t row = 0; row < height_; ++row) {
    for (std::size_t col = 0; col < width_; ++col) {
      Coord c{row, col};
      if (IsFilled(c)) {
        if (!IsLocked(c)) {
          locked_coords.push_back(c);
        }
        LockCell(c, true);
      }
    }
  }

  CALLGRIND_START_INSTRUMENTATION;
  CALLGRIND_TOGGLE_COLLECT;

  int nodes_searched = 0;
  auto start = std::chrono::high_resolution_clock::now();
  bool found = false;          // true if solution was found
  bool complete_search = true; // true if entire search completed
  std::vector<std::unique_ptr<DFSNode>> dfs_stack{};
  while (!found && !stop_searching && *hard_min > 0) {
    logger.Log("Searching with hard minimum score of " + std::to_string(*hard_min) + " and entropy score " +
               std::to_string(*entropy));
    if (*branching_factor_limit != kNO_NUMBER) {
      logger.Log("...with branching factor " + std::to_string(*branching_factor_limit));
    }

    // TODO: fix cache so we don't have to do this...
    db.FlushCaches();

    dfs_stack.clear();

    std::size_t initial_depth = action_stack_.GetSize();

    CrosswordActionGroup *dummy = new CrosswordActionGroup();
    dfs_stack.push_back(std::unique_ptr<DFSNode>(new DFSNode(dummy, initial_depth + 1)));

    int iterations = 0;
    complete_search = true;
    while (!dfs_stack.empty()) {
      iterations++;

      if (stop_searching) {
        logger.Log("Externally stopped with no solution found. Cleaning up");
        complete_search = false;
        break;
      }

      // TODO: log size of stack and memory usage.
      // if (iterations % branching_factor_limit * 10)
      // {
      // log stack size to debug log
      // }

      int relative_depth = dfs_stack.back().get()->depth;
      int current_depth = action_stack_.GetSize();

      // roll back to necessary depth (1 less than popped node depth)
      while (static_cast<int>(action_stack_.GetSize()) > relative_depth - 1) {
        Undo();
      }
      ApplyAction(dfs_stack.back().get()->action);
      dfs_stack.pop_back();
      nodes_searched++;

      std::vector<Clue> all_clues = Clues();

      // Leaf case 1: Invalid, we abandon this branch
      if (IsInvalidPartial(all_clues, db, *hard_min) != Solvability::Solvable) {
        continue;
      }

      // Leaf case 2: Solution found, exit
      if (IsSolved(all_clues, db)) {
        logger.Log("Found solution! Exiting");
        found = true;
        break;
      }

      std::vector<CrosswordActionGroup *> adj = GetWordFills(all_clues, params);

      // Leaf case 3: no valid fills from this direction.
      if (adj.empty()) {
        continue;
      }


      // Reverse iterator because we want best on top of the stack.
      for (auto it = adj.rbegin(); it != adj.rend(); ++it) {
        dfs_stack.push_back(std::unique_ptr<DFSNode>(new DFSNode(*it, relative_depth + 1)));
      }
    } // end DFS while

    for (auto it = std::begin(dfs_stack); it != std::end(dfs_stack); ++it) {
      delete (*it).get()->action;
    }

    if (!found) {
      if (complete_search) {
        logger.Log("Full tree search completed and no solution found. Relaxing constraints...");
      }

      if (rollback) {
        assert(action_stack_.GetSize() >= initial_depth);
        std::size_t depth_reached = action_stack_.GetSize() - initial_depth;
        for (std::size_t i = 0; i < depth_reached; i++) {
          Undo();
        }
      }
    } // end if

    *hard_min = static_cast<int>(*hard_min * *hard_min_decay);
    *entropy = static_cast<int>(*entropy * *entropy_decay);
  } // end while

  auto stop = std::chrono::high_resolution_clock::now();

  CALLGRIND_TOGGLE_COLLECT;
  CALLGRIND_STOP_INSTRUMENTATION;

  if (!found && complete_search) {
    logger.Log("Autofill completed full tree search and did not find a solution");
  }

  double msecs = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count());
  if (nodes_searched > 2) {
    double npms = (static_cast<double>(nodes_searched)) / msecs;
    double nps = npms * 1000.;
    logger.Log("Nodes per second: " + std::to_string(static_cast<int>(nps)));
  }

  for (auto &coord: locked_coords) {
    ToggleLockCell(coord);
  }

  done_searching = true;
  stop_thread.join();
}
