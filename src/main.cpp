#include "../hpp/tree.hpp"

int main(int argc, char* argv[]){
    tree_t tree = {};

    TreeCtor(&tree);

    StartAkinator(&tree);

    HTMLDumpGenerate(&tree);

    TreeDtor(&tree);
    return 0;
}
