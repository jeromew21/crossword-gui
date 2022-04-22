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

void WordTrie::Insert(Word const &entry) {
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

TrieNode* TrieNode::AddChild(const Atom child_value) {
  assert(FindChild(child_value) == nullptr);
  TrieNode* new_node = new TrieNode(child_value, this);
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

std::vector<Word> TrieNode::Find(const Word &partial, std::size_t substr_start) const {
  assert(substr_start < partial.size()); // OOB would be problematic...

  const Atom target_child = partial[substr_start];
  if (substr_start == partial.size() - 1) {
    if (target_child.IsEmpty()) { // If final character is wildcard, just push everything
      std::vector<Word> result{};
      for (const auto &child_leaf_ptr: children) {
        result.push_back(child_leaf_ptr->LeafToWord());
      }
      return result;
    } else {
      TrieNode *child = FindChild(target_child);
      if (child == nullptr) return {}; // not found in children
      std::vector<Word> result{};
      result.push_back(child->LeafToWord());
      return result;
    }
  }

  if (target_child.IsEmpty()) { // If current is wildcard, push everything
    std::vector<Word> result{};
    for (const auto &child_ptr: children) {
      std::vector<Word> child_solutions = child_ptr->Find(partial, substr_start + 1);
      result.insert(result.end(), child_solutions.begin(), child_solutions.end());
    }
    return result;
  } else { // Otherwise, just push subtree.
    TrieNode *child = FindChild(target_child);
    if (child == nullptr) return {};
    std::vector<Word> result{};
    return child->Find(partial, substr_start + 1);
  }
};

/**
 * @brief Go from a leaf node to entire word.
 *
 * @return Word
 */
Word TrieNode::LeafToWord() const {
  assert(IsTerminal());
  Word leaf_word;

  const TrieNode *node = this;
  while (node != nullptr && !node->value.IsEmpty()) {
    Atom atom_value = node->value;
    leaf_word.atoms_.insert(leaf_word.atoms_.begin(), atom_value);
    node = node->parent;
  }

  return leaf_word;
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
