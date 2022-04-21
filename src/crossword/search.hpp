/**
 * @file search.hpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Search data structures.
 * @version 0.1
 * @date 2022-04-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef SEARCH_HPP
#define SEARCH_HPP

namespace crossword_backend {
  /**
   * @brief Search parameters.
   *
   */
  struct AutofillParams {
    /**
     * @brief Pointer to database containing word information.
     *
     */
    WordDatabase *db;

    /**
     * @brief Number of seconds to search for.
     *
     */
    int seconds_limit;

    /**
     * @brief Limit branching factor.
     *
     */
    int branching_factor_limit;

    /**
     * @brief Amount of randomness, in [0, 100]
     *
     */
    int entropy;

    /**
     * @brief Factor to multiply randomness in the next iteration, in [0, 1]
     *
     */
    double entropy_decay;

    /**
     * @brief Minimum score requirement for additional words, in [0, 100]
     *
     */
    int score_min;

    /**
     * @brief Factor to multiply randomness by in the next iteration, in [0, 1]
     *
     */
    double score_min_decay;

    /**
     * @brief Do we undo incomplete searches?
     *
     */
    bool rollback;

    /**
     * @brief Autofill parameter construction
     *
     */
    explicit AutofillParams(WordDatabase *db) : db(db), entropy(100), entropy_decay(.9), score_min(100),
                                       score_min_decay(.9), branching_factor_limit(kNO_NUMBER),
                                       rollback(true), seconds_limit(100) {};
  };

  /**
   * @brief DFS search node.
   *
   */
  struct DFSNode {
    CrosswordAction *action;
    int depth;

    DFSNode(CrosswordAction *action_pointer, int node_depth) : action(action_pointer), depth(node_depth) {};
  };
}

#endif
