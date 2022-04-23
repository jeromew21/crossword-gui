/**
 * @file trie.cpp
 * @author Jerome Wei (jeromejwei@gmail.com)
 * @brief Simple trie implementation
 * @version 0.1
 * @date 2022-04-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "crossword/database.hpp"

using namespace crossword_backend;

/**
 * Insert a word into the trie.
 *
 * @param entry
 */
void WordTrie::Insert(Word const &entry) const {
  TrieNode *node = root.get();
  for (auto const letter: entry.atoms_) {
    TrieNode *child = node->FindChild(letter);
    if (child != nullptr) {
      node = child;
    } else {
      node = node->AddChild(letter);
    }
  }
}

TrieNode *TrieNode::AddChild(const Atom child_value) {
  assert(FindChild(child_value) == nullptr);
  auto *new_node = new TrieNode(child_value, this);
  children.push_back(std::unique_ptr<TrieNode>(new_node));
  return new_node;
}

TrieNode *TrieNode::FindChild(const Atom queried_child) const {
  if (IsTerminal()) return nullptr;
  for (auto const &child: children) {
    if (queried_child == child->value) {
      return child.get();
    }
  }
  return nullptr;
}

/**
 * @brief Recursively check whether a word is contained in the trie.
 *
 * @param partial
 * @param substr_start
 * @return
 */
bool TrieNode::Contains(const Word &partial, std::size_t substr_start) const {
  assert(substr_start < partial.size()); // OOB would be problematic...

  const Atom target_child = partial[substr_start];
  if (substr_start == partial.size() - 1) {
    if (target_child.IsEmpty()) { // If final character is wildcard, true iff there is anything remaining.
      return !IsTerminal();
    } else {
      TrieNode *child = FindChild(target_child);
      return child != nullptr; // not found in children
    }
  }

  if (target_child.IsEmpty()) { // If current is wildcard, true iff any sub-problems true
    for (const auto &child_ptr: children) {
      bool sub_value = child_ptr->Contains(partial, substr_start + 1);
      if (sub_value) return true;
    }
    return false;
  } else {
    TrieNode *child = FindChild(target_child);
    if (child == nullptr) return false;
    return child->Contains(partial, substr_start + 1);
  }
}

/**
 * @brief Recursively returns all words in the trie that fit a particular
 * partial query.
 *
 * @param partial
 * @param substr_start
 * @return std::vector<Word>
 */
std::vector<Word> TrieNode::Find(Word const &partial, std::size_t substr_start) const {
  assert(substr_start < partial.size()); // OOB would be problematic...

  const Atom target_child = partial[substr_start];
  if (substr_start == partial.size() - 1) {
    if (target_child.IsEmpty()) { // If final character is wildcard, just push everything
      std::vector<Word> result;
      result.reserve(children.size());
      for (const auto &child_leaf_ptr: children) {
        result.push_back(child_leaf_ptr->LeafToWord());
      }
      return result;
    } else {
      TrieNode *child = FindChild(target_child);
      if (child == nullptr) return {}; // not found in children
      return {child->LeafToWord()};
    }
  }

  if (target_child.IsEmpty()) { // If current is wildcard, push everything
    std::vector<Word> result;
    for (const auto &child_ptr: children) {
      std::vector<Word> const &child_solutions = child_ptr->Find(partial, substr_start + 1);
      result.insert(result.end(), child_solutions.begin(), child_solutions.end());
    }
    return result;
  } else { // Otherwise, just push subtree.
    TrieNode *child = FindChild(target_child);
    if (child == nullptr) return {};
    return child->Find(partial, substr_start + 1);
  }
}

/**
 * @brief Go from a leaf node to entire word.
 *
 * Hotspot: 23 percent. We could try caching these but then every node gets an extra field. But that makes sense
 * in all honesty so
 *
 * @return Word
 */
Word &TrieNode::LeafToWord() const {
  assert(IsTerminal() && leaf_word != nullptr);
  return *leaf_word;
}

std::string TrieNode::ReprString() const {
  if (IsTerminal()) return LeafToWord().ToString();
  std::string result;
  result += "{" + value.ToString() + " children=";
  for (auto const &child: children) {
    result += child->ReprString() + ", ";
  }
  return result + "}";
}
