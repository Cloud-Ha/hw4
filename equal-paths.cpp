#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
static bool checkDepth(Node* node, int depth, int& firstLeafDepth)
{
  if (node == nullptr){
    return true;
  }
  if (node->left == nullptr && node->right == nullptr){
    if (firstLeafDepth == -1){
      firstLeafDepth = depth;
    }
    return depth == firstLeafDepth;
  }

  return checkDepth(node->left, depth+1, firstLeafDepth) && checkDepth(node->right, depth+1, firstLeafDepth);
}

bool equalPaths(Node * root)
{
    // Add your code below
    int firstLeafDepth = -1;
    return checkDepth(root, 0, firstLeafDepth);
}

