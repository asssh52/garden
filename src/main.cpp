#include "../hpp/tree.hpp"

int main(int argc, char* argv[]){
    tree_t tree = {};

    TreeCtor(&tree);
    TreePrint(&tree);

    NewNode(&tree, 23, tree.root, LEFT, nullptr);
    NewNode(&tree, 10, tree.root->left, LEFT, nullptr);
    NewNode(&tree, 25, tree.root->left, RIGHT, nullptr);
    NewNode(&tree, 26, tree.root->left->right, RIGHT, nullptr);
    NewNode(&tree, 27, tree.root->left->right->right, RIGHT, nullptr);

    TreeDump(&tree);

    return 0;
}
