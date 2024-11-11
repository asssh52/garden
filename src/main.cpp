#include "../hpp/tree.hpp"

int main(int argc, char* argv[]){
    tree_t tree = {};

    TreeCtor(&tree);


    StartAkinator(&tree);
    TreeDump(&tree);

    HTMLDumpGenerate(&tree);
    return 0;
}
