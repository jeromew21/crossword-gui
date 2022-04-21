/**
 * @file logging.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Logging implementation
 * @version 0.1
 * @date 2022-04-11
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "logging.hpp"

using namespace crossword_backend;

/**
 * @brief Default constructor for Logger 
 * 
 */
Logger::Logger() : silent(false), start_time_{std::chrono::high_resolution_clock::now()} {}

/**
 * @brief Returns a reference to all the logs from start of program execution.
 *
 * @return std::vector<std::string> const& 
 */
std::vector<std::string> const &Logger::GetLogs() {
  return history_;
}

/**
 * @brief Returns time offset string
 *
 * @return std::string
 */
std::string Logger::time_offset() {
  auto stop = std::chrono::high_resolution_clock::now();
  int secs = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(stop - start_time_).count());
  return "+" + std::to_string(secs) + "s";
}

/**
 * @brief Writes a string to stdout and add it to the running log.
 *
 * @param val
 */
void Logger::Log(std::string const &val) {
  std::string log_message = "[" + time_offset() + "] " + val;
  std::lock_guard<std::mutex> lock(mutex_);
  if (!silent) {
    std::cout << log_message << std::endl;
  }
  history_.push_back(log_message);
}

/**
 * @brief Write out log to stdout but don't make it visible to the user.
 *
 * @param val
 */
void Logger::DebugLog(std::string const &val) {
  std::string log_message = "[DEBUG " + time_offset() + "] " + val;
  if (!silent) {
    const std::lock_guard<std::mutex> lock(mutex_);
    std::cout << log_message << std::endl;
  }
}
