#include "../hpp/tree.hpp"
#define MEOW fprintf(stderr, RED "MEOW\n" RESET);

const int64_t POISON = -52;

enum errors{

    OK  =   0,
    ERR =   1

};

static  int EndTreeDump     (tree_t* tree);
static  int StartTreeDump   (tree_t* tree);
        int NodeDump        (tree_t* tree, node_t* node);

/*=================================================================*/

int NewNode(tree_t* tree, data_t data, node_t* parentNode, param_t param, node_t** returnNode){
    //tree verify?
    node_t* newNode = (node_t*)calloc(1, sizeof(*newNode));

    newNode->data = data;
    newNode->id   = tree->numElem;

    switch(param){
        case ROOT:{
            if (returnNode) *returnNode = newNode;

            break;
        }

        case LEFT:{
            parentNode->left    = newNode;
            newNode->parent     = parentNode;

            if (returnNode) *returnNode = newNode;
            break;
        }

        case RIGHT:{
            parentNode->right   = newNode;
            newNode->parent     = parentNode;

            if (returnNode) *returnNode = newNode;
            break;
        }

        default:{
            return ERR;

            break;
        }
    }

    tree->numElem++;
    return OK;
}

/*=================================================================*/

int TreeCtor(tree_t* tree){
    if (!tree) return ERR;

    tree->files.log = fopen(tree->files.logName, "w");
    if (!tree->files.log) tree->files.log = stdout;

    tree->files.dot = fopen(tree->files.dotName, "w");
    if (!tree->files.dot) tree->files.dot = fopen("dotdump.dot", "w");

    NewNode(tree, POISON, nullptr, ROOT, &(tree->root));

    //tree verify
    return OK;
}

/*=================================================================*/

int TreeDtor(tree_t* tree){
    //tree verify


    return OK;
}

/*=================================================================*/

int TreePrint(tree_t* tree){
    //tree verify
    NodePrint(tree->root);
    printf("\n");

    return OK;
}

int NodePrint(node_t* node){
    if (!node) return OK;

    printf("(");

    if (node->left)    NodePrint(node->left);

    printf("%lld", node->data);

    if (node->right)   NodePrint(node->right);
    printf(")");

    return OK;
}

/*=================================================================*/

int TreeDump(tree_t* tree){

    StartTreeDump(tree);

    NodeDump(tree, tree->root);

    EndTreeDump(tree);

    return OK;
}

int NodeDump(tree_t* tree, node_t* node){
    if (!node) return OK;

    fprintf(tree->files.dot,
            "\tnode%0.3d [fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#e6f2ff\";label = \" { %0.3d } | { data = %3.0lld }\"];\n",
            node->id, node->id, node->data);

    if (node->left){
        fprintf(tree->files.dot,
                "\tnode%0.3d -> node%0.3lld [ weight=1; color=\"#fd4381\"; style=\"bold\"];\n\n",
                node->id, node->left->id);

        NodeDump(tree, node->left);
    }

    if (node->right){
        fprintf(tree->files.dot,
                "\tnode%0.3d -> node%0.3lld [ weight=1; color=\"#3474f5\"; style=\"bold\"];\n\n",
                node->id, node->right->id);

        NodeDump(tree, node->right);
    }

    return OK;
}

/*=================================================================*/

static int StartTreeDump(tree_t* tree){

    fprintf(tree->files.dot, "digraph G{\n");

    fprintf(tree->files.dot, "\trankdir=TB;\n");
    fprintf(tree->files.dot, "\tbgcolor=\"#f8fff8\";\n");

    return OK;
}

static int EndTreeDump(tree_t* tree){

    fprintf(tree->files.dot, "}\n");

    return OK;
}
