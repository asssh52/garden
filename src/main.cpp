#include "../hpp/tree.hpp"

int main(int argc, char* argv[]){
    tree_t tree = {};

    TreeCtor(&tree);
    AddTreeElem(&tree, "животное");
    TreeDump(&tree);
    NewNode(&tree, "полторашка", tree.root, LEFT, nullptr);
    TreeDump(&tree);
    NewNode(&tree, "препод", tree.root, RIGHT, nullptr);
    TreeDump(&tree);
    NewNode(&tree, "дед", tree.root->right, LEFT, nullptr);
    TreeDump(&tree);
    NewNode(&tree, "хз", tree.root->right, RIGHT, nullptr);
    TreeDump(&tree);

    StartAkinator(&tree);
    TreeDump(&tree);

    HTMLDumpGenerate(&tree);
    return 0;
}
