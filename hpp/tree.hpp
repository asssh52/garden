#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "colors.hpp"

typedef int64_t data_t;

typedef struct node_t{

    node_t* left;
    node_t* right;
    node_t* parent;

    size_t id;
    data_t data;

} node_t;

typedef struct files_t{

    char*   logName;
    FILE*   log;

    char*   dotName;
    FILE*   dot;

    char*   htmlName;
    FILE*   html;

} files_t;

typedef struct tree_t{

    node_t*     root;

    size_t      numElem;
    size_t      numDump;

    files_t     files;

} tree_t;

typedef enum params{

    ROOT    =   0,
    LEFT    =   1,
    RIGHT   =   2

} param_t;

int NewNode         (tree_t* tree, data_t data, node_t* parentNode, param_t param, node_t** returnNode);
int TreeDtor        (tree_t* tree);
int TreeCtor        (tree_t* tree);
int TreePrint       (tree_t* tree);
int TreeDump        (tree_t* tree);
int NodePrint       (node_t* node);
int StartTreeDump   (tree_t* tree);
int EndTreeDump     (tree_t* tree);
int AddTreeElem     (tree_t* tree, data_t data);

int HTMLDumpGenerate(tree_t* tree);
