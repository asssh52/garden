#include "../hpp/tree.hpp"
#define MEOW fprintf(stderr, RED "MEOW\n" RESET);

const int64_t POISON = -52;

enum errors{

    OK  =   0,
    ERR =   1

};

static int NodeDump         (tree_t* tree, node_t* node);
static int AddNodeElem      (tree_t* tree, node_t* node, data_t data);
static int DoDot            (tree_t* tree);

static int HTMLGenerateHead (tree_t* tree);
static int HTMLGenerateBody (tree_t* tree);

/*=================================================================*/



/*=================================================================*/

int NewNode(tree_t* tree, data_t data, node_t* parentNode, param_t param, node_t** returnNode){
    //tree verify?
    node_t* newNode = (node_t*)calloc(1, sizeof(*newNode));

    newNode->data       = data;
    newNode->id         = tree->numElem;
    tree->lastModified  = tree->numElem;

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

    tree->files.html = fopen(tree->files.htmlName, "w");
    if (!tree->files.html){
        tree->files.html     = fopen("htmldump.html", "w");
        tree->files.htmlName = "htmldump.html";
    }

    HTMLGenerateHead(tree);
    //tree verify

    fprintf(tree->files.log, "tree created\n");
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
    fprintf(tree->files.log, "\ntree print:\n");

    NodePrint(tree, tree->root);
    printf("\n");

    fprintf(tree->files.log, "tree printed\n");
    return OK;
}

int NodePrint(tree_t* tree, node_t* node){
    if (!node) return OK;

    printf("(");

    if (node->left)    NodePrint(tree, node->left);

    printf("%lld", node->data);

    if (node->right)   NodePrint(tree, node->right);
    printf(")");

    return OK;
}

/*=================================================================*/

int TreeDump(tree_t* tree){
    fprintf(tree->files.log, "\ntree dump#%d started\n", tree->numDump + 1);

    StartTreeDump(tree);
    NodeDump(tree, tree->root);
    EndTreeDump(tree);

    DoDot(tree);
    HTMLGenerateBody(tree);

    fprintf(tree->files.log, "tree dumped\n");
    return OK;
}

static int NodeDump(tree_t* tree, node_t* node){
    if (!node) return OK;

    if (node->id == tree->lastModified){
        fprintf(tree->files.dot,
            "\tnode%0.3lu [fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#79FF61\";label = \" { %0.3lu } | { data = %3.0lld }\"];\n",
            node->id, node->id, node->data);
    }

    else{
        fprintf(tree->files.dot,
                "\tnode%0.3lu [fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#e6f2ff\";label = \" { %0.3lu } | { data = %3.0lld }\"];\n",
                node->id, node->id, node->data);
    }

    if (node->left){
        fprintf(tree->files.dot,
                "\tnode%0.3lu -> node%0.3lu [ weight=1; color=\"#fd4381\"; style=\"bold\"];\n\n",
                node->id, node->left->id);

        NodeDump(tree, node->left);
    }

    if (node->right){
        fprintf(tree->files.dot,
                "\tnode%0.3lu -> node%0.3lu [ weight=1; color=\"#3474f5\"; style=\"bold\"];\n\n",
                node->id, node->right->id);

        NodeDump(tree, node->right);
    }

    return OK;
}

/*=================================================================*/

int StartTreeDump(tree_t* tree){

    tree->files.dot = fopen(tree->files.dotName, "w");
    if (!tree->files.dot){
        tree->files.dot     = fopen("dotdump.dot", "w");
        tree->files.dotName = "dotdump.dot";
    }

    fprintf(tree->files.dot, "digraph G{\n");

    fprintf(tree->files.dot, "\trankdir=TB;\n");
    fprintf(tree->files.dot, "\tbgcolor=\"#f8fff8\";\n");

    return OK;
}

int EndTreeDump(tree_t* tree){

    fprintf(tree->files.dot, "}\n");

    fclose(tree->files.dot);

    return OK;
}

/*=================================================================*/

int AddTreeElem(tree_t* tree, data_t data){
    //verify
    node_t* currentNode = tree->root;

    if (!currentNode){
        NewNode(tree, data, nullptr, ROOT, &tree->root);


        return OK;
    }

    AddNodeElem(tree, currentNode, data);

    return OK;
}

int AddNodeElem(tree_t* tree, node_t* node, data_t data){

    /*if (data == node->data){
        return OK;

    }

    else*/ if (data < node->data){
        if (node->left){
            AddNodeElem(tree, node->left,  data);

            return OK;
        }

        NewNode(tree, data, node, LEFT, nullptr);

    }

    else{
        if (node->right){
            AddNodeElem(tree, node->right, data);

            return OK;
        }

        NewNode(tree, data, node, RIGHT, nullptr);
    }

    return OK;
}

/*=================================================================*/

static int DoDot(tree_t* tree){
    char command[100]   = {};
    char out[100]       = {};

    const char* startOut= "./bin/png/output";
    const char* endOut  = ".png";

    snprintf(out, 100, "%s%lu%s", startOut, tree->numDump, endOut);
    snprintf(command, 100, "dot -Tpng %s > %s", tree->files.dotName, out);
    system(command);

    tree->numDump++;
    return OK;
}

static int HTMLGenerateHead(tree_t* tree){
    fprintf(tree->files.html, "<html>\n");

    fprintf(tree->files.html, "<head>\n");
    fprintf(tree->files.html, "</head>\n");

    fprintf(tree->files.html, "<body style=\"background-color:#f8fff8;\">\n");

    return OK;
}

static int HTMLGenerateBody(tree_t* tree){
    fprintf(tree->files.html, "<div style=\"text-align: center;\">\n");

    fprintf(tree->files.html, "\t<h2 style=\"font-family: 'Haas Grot Text R Web', 'Helvetica Neue', Helvetica, Arial, sans-serif;'\"> Dump: %lu</h2>\n", tree->numDump);
    fprintf(tree->files.html, "\t<img src=\"./bin/png/output%lu.png\">\n\t<br>\n\t<br>\n\t<br>\n", tree->numDump - 1);

    fprintf(tree->files.html, "</div>\n");

    return OK;
}

int HTMLDumpGenerate(tree_t* tree){


    fprintf(tree->files.html, "</body>\n");
    fprintf(tree->files.html, "</html>\n");

    return OK;
}
