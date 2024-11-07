#include "../hpp/tree.hpp"

int main(int argc, char* argv[]){
    tree_t tree = {};

    TreeCtor(&tree);
    AddTreeElem(&tree, 50);
    int a = 12312837;

    for (int i = 0; i < 50; i++){
        AddTreeElem(&tree, a % 100);
        TreeDump(&tree);

        a = a << 1;
        a += 23456789098;
    }

    HTMLDumpGenerate(&tree);
    TreePrint(&tree);
    return 0;
}
