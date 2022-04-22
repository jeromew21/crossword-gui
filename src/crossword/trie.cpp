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
  TrieNode* node = root.get();
  for (auto const letter : entry.atoms_) {
    TrieNode* child = node->FindChild(letter);
    if (child != nullptr)
    {
      node = child;
    } else {
      node->AddChild(letter);
    }
  }
}

void TrieNode::AddChild(const Atom child_value) {
  assert(FindChild(child_value) == nullptr);
  children.push_back(std::make_unique<TrieNode>(child_value, this));
}

TrieNode* TrieNode::FindChild(const Atom queried_child) const {
  if (IsTerminal()) return nullptr;
  for (auto const &child : children) {
    if (queried_child == child->value) {
      return child.get();
    }
  }
  return nullptr;
}

std::vector<Word> TrieNode::Find(const Word &partial, std::size_t substr_start) const {
  std::vector<Word> result{};
  if (IsTerminal()) return result;
  if (substr_start == partial.size() - 1) {
      for (const auto &child_leaf_ptr : children)
      {
        result.push_back(child_leaf_ptr->LeafToWord());
      }
  }

  const Atom current_atom = partial[substr_start];
  if (current_atom.IsEmpty())
  {
    for (const auto &child_ptr : children)
    {
      std::vector<Word> child_solutions = child_ptr->Find(partial, substr_start + 1);
      result.insert(result.end(), child_solutions.begin(), child_solutions.end());
    }
    return result;
  } else {
    TrieNode *child = FindChild(current_atom);
    if (child == nullptr) return result;
    return child->Find(partial, substr_start + 1);
  }
};

Word TrieNode::LeafToWord() const {
  assert(IsTerminal());
  Word leaf_word;

  const TrieNode* node = this;
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
  for (auto const &child: children)
  {
    result += child->ReprString() + ", ";
  }
  return result + "}";
}
