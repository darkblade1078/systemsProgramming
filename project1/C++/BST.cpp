#include "BST.hpp"


bool BST::insert(Node *&node, const std::string &value)
{
  if (node == nullptr)
    {
      node = new Node(value);
      return true;
    }
  if (value < node->value) { return insert(node->left, value);}
  if (value > node->value) { return insert(node->right,value);}
  return false;
}


bool BST::search(Node *node, const std::string &value)
{
  if (node == nullptr)
    {
      return false;
    }
  if (value < node->value) { return search(node->left,value);}
  if (value > node->value) { return search(node->right,value);}
  return true;
}
bool BST::remove(Node *&node, const std::string &value)
{
  return true;
}

void BST::postOrder(Node *node, std::ostream &os)
{
  if (node == nullptr) return;
  postOrder(node->left,os);
  postOrder(node->right,os);
  os << node->value << " ";
}
void BST::dealloc(Node *&node)
{
  if (node == nullptr) return;
  dealloc(node->left);
  dealloc(node->right);
  delete node;
  node = nullptr;
}

void BST::copy(Node *node, BST *bstCp)
{
  if (node == nullptr) return;
  bstCp->insert(node->value);
  copy(node->left,bstCp);
  copy(node->right,bstCp);
}
