#pragma once
#include <string>
#include <iostream>

class Node
{
public:
  std::string value;
  Node *left;
  Node *right;

  Node(): left(nullptr), right(nullptr) {}
  Node(const std::string &value): value(value), left(nullptr), right(nullptr) {}
};

class BST
{
private:
  Node *root;

  bool insert(Node *&node, const std::string &value);
  bool search(Node *node, const std::string &value);
  bool remove(Node *&node, const std::string &value);

  void postOrder(Node *node, std::ostream &os);
  void dealloc(Node *&node);
  void copy(Node *node, BST *bstCp);
public:

  BST() : root(nullptr) {}
  BST( BST &old) { old.copy(this);}
  ~BST() { dealloc(root);}
  
  
  bool insert(const std::string &value) { return insert(root, value);}
  bool search(const std::string &value) { return search(root, value);}
  bool remove(const std::string &value) { return remove(root, value);}

  void postOrder(std::ostream &os){ postOrder(root, os);}

  void copy(BST *bstCp) { copy(root, bstCp);};
};
