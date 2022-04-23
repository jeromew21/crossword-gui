/**
 * @file logging.hpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Header file for debug logging methods.
 * @version 0.1
 * @date 2022-04-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <mutex>

namespace crossword_backend {
  /**
   * @brief Logging methods for backend.
   *
   */
  class Logger {
  public:
    /**
     * @brief Set logger to silent mode.
     *
     */
    void Silence() { silent = true; };

    void Log(std::string const &val);

    [[maybe_unused]] void DebugLog(std::string const &val);

    [[maybe_unused]] std::vector<std::string> const &GetLogs();

    Logger();

  private:
    /**
     * @brief True iff logging is done in silent mode.
     *
     */
    bool silent;

    std::string time_offset();

    /**
     * @brief Time when InitLogs() is called.
     *
     */
#if defined(__gnu_linux__)
    std::chrono::time_point<std::chrono::system_clock> start_time_;
#else
    std::chrono::steady_clock::time_point start_time_;
#endif
    /**
     * @brief Holds entire log from beginning of execution.
     *
     */
    std::vector<std::string> history_;

    /**
     * @brief Ensures thread safety of logging operations.
     *
     * Protects history and stdout.
     */
    std::mutex mutex_;
  };
}

#endif
