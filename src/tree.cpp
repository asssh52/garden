#include "../hpp/tree.hpp"
#define MEOW fprintf(stderr, RED "MEOW\n" RESET);

const int64_t POISON        = -52;
const int64_t MESSAGE_LEN   = 100;
const int64_t ANS_LEN       = 20;

enum errors{

    OK  =   0,
    ERR =   1

};

static int NodeDump         (tree_t* tree, node_t* node, int depth, param_t param);
static int NodeDel          (tree_t* tree, node_t* node);
static int NodePrint        (tree_t* tree, node_t* node, int depth, FILE* file);
static int AddNodeElem      (tree_t* tree, node_t* node, data_t data);
static int DoDot            (tree_t* tree);

static int HTMLGenerateHead (tree_t* tree);
static int HTMLGenerateBody (tree_t* tree, param_t param);

static int LoadTree         (tree_t* tree);
static int LoadNode         (tree_t* tree, node_t* node);

static int NewQuestion      (tree_t* tree, node_t* node);
static int ProcessNode      (tree_t* tree, node_t* node);
static int SaveTree         (tree_t* tree);
static int StartTreeDump    (tree_t* tree);
static int EndTreeDump      (tree_t* tree);
static int AskQuestion      (node_t* node);
static int AskContinue      (int* retValue);
static int GetAnswer        (int* retValue);

/*=================================================================*/

int StartAkinator(tree_t* tree){
    LoadTree(tree);

    ProcessNode(tree, tree->root);

    int doContinue = POISON;
    AskContinue(&doContinue);

    if (doContinue) StartAkinator(tree);
    else SaveTree(tree);

    return OK;
}

static int LoadTree(tree_t* tree){

    //open save
    tree->files.save = fopen(tree->files.saveName, "r");
    if (!tree->files.save){
        tree->files.save     = fopen("save.txt", "r");
        tree->files.saveName = "save.txt";
    }
    //open save
    TreeDel(tree);

    LoadNode(tree, tree->root);


    return OK;
}

static int LoadNode(tree_t* tree, node_t* node){
    char* newData        = (char*)calloc(ANS_LEN, sizeof(char));
    char* newQuestion    = (char*)calloc(ANS_LEN, sizeof(char));
    fscanf("{%[]",);


    return OK;
}

static int SaveTree(tree_t* tree){

    tree->files.save = fopen(tree->files.saveName, "w");
    if (!tree->files.save){
        tree->files.save     = fopen("save.txt", "w");
        tree->files.saveName = "save.txt";
    }

    TreePrint(tree, tree->files.save);

    fclose(tree->files.save);

    return OK;
}

static int ProcessNode(tree_t* tree, node_t* node){
    int retValue = POISON;
    tree->currentNode = node->id;
    TreeDump(tree);

    AskQuestion(node);
    GetAnswer(&retValue);

    if (retValue){
        if (node->left){
            ProcessNode(tree, node->left);
        }

        else{
            printf("ура!!!\n");
        }
    }

    else{
        if (node->right){
            ProcessNode(tree, node->right);
        }

        else{
            NewQuestion(tree, node);
        }
    }

    return OK;
}

static int NewQuestion(tree_t* tree, node_t* node){
    char* newData        = (char*)calloc(ANS_LEN, sizeof(char));
    char* newQuestion    = (char*)calloc(ANS_LEN, sizeof(char));

    printf("кто это был?\n");
    scanf("\n%[^\n]", newData);

    printf("чем отличается %s от %s?\n", newData, node->data);
    scanf("\n%[^\n]", newQuestion);

    NewNode(tree, node->data, node, RIGHT, nullptr);
    NewNode(tree, newData, node, LEFT, nullptr);

    node->data = newQuestion;

    return OK;
}

static int AskContinue(int* doContinue){
    printf("продолжить?\n");
    GetAnswer(doContinue);

    return OK;
}

static int AskQuestion(node_t* node){
    char message[MESSAGE_LEN]= {};
    snprintf(message, MESSAGE_LEN, "это %s?\n", node->data);

    printf("%s", message);
    return OK;
}

static int GetAnswer(int* retValue){
    char buffer[ANS_LEN] = {};

    while (*retValue == POISON){
        scanf("%s", buffer);

        if      (!strcmp(buffer, "да"))     *retValue = 1;
        else if (!strcmp(buffer, "нет"))    *retValue = 0;
        else{
            printf("неверный ввод, введите \"да\" или \"нет\"\n");
        }
    }

    return OK;
}

/*=================================================================*/

int NewNode(tree_t* tree, data_t data, node_t* parentNode, param_t param, node_t** returnNode){
    //tree verify?
    node_t* newNode = (node_t*)calloc(1, sizeof(*newNode));
    if (newNode == nullptr) abort();

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

int TreeDel(tree_t* tree){
    NodeDel(tree, tree->root);
    tree->numElem = 0;

    NewNode(tree, "никто", nullptr, ROOT, &tree->root);

    return OK;
}

static int NodeDel(tree_t* tree, node_t* node){

    if (node->left)  NodeDel(tree, node->left);
    if (node->right) NodeDel(tree, node->right);

    TreeDump(tree);

    return OK;
}
/*=================================================================*/

int TreePrint(tree_t* tree, FILE* file){
    //tree verify
    fprintf(tree->files.log, "\ntree print:\n");

    NodePrint(tree, tree->root, 0, file);
    printf("\n");

    fprintf(tree->files.log, "tree printed\n");
    return OK;
}

static int NodePrint(tree_t* tree, node_t* node, int depth, FILE* file){
    if (!node) return OK;
    if (depth >= tree->numElem) return ERR;

    fprintf(file, "{");

    fprintf(file, "\"%s\"", node->data);

    if(node->left || node->right) fprintf(file, "\n");

    if (node->left){
        for (int i = 0; i < depth; i++) fprintf(file, "\t");
        fprintf(file, "yes:");
        NodePrint(tree, node->left, depth + 1, file);
    }

    if (node->right){
        for (int i = 0; i < depth; i++) fprintf(file, "\t");
        fprintf(file, "no: ");
        NodePrint(tree, node->right, depth + 1, file);
    }

    if(node->left || node->right) for (int i = 0; i < depth; i++) fprintf(file, "\t");
    fprintf(file, "}\n");

    return OK;
}

/*=================================================================*/

int TreeDump(tree_t* tree){
    fprintf(tree->files.log, "\ntree dump#%lu started\n", tree->numDump + 1);

    StartTreeDump(tree);
    NodeDump(tree, tree->root, 0, SIMPLE);
    EndTreeDump(tree);

    DoDot(tree);
    HTMLGenerateBody(tree, SIMPLE);

    fprintf(tree->files.log, "tree dumped\n");
    return OK;
}

static int NodeDump(tree_t* tree, node_t* node, int depth, param_t param){
    if (!node) return OK;
    if (depth > tree->numElem) return ERR;

    // detailed dump
    if (param == DETAILED){
        if (node->id == tree->currentNode){
            fprintf(tree->files.dot,
                "\tnode%0.3lu [rankdir=LR; fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#FCFF61\";label = \" { %0.3lu } | { data = %s } | { pointer = %p} | { parent = %p} | { left = %p} | { right = %p}\"];\n",
                node->id, node->id, node->data, node, node->parent, node->left, node->right);
        }

        else if (node->id == tree->lastModified){
            fprintf(tree->files.dot,
                "\tnode%0.3lu [rankdir=LR; fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#79FF61\";label = \" { %0.3lu } | { data = %s } | { pointer = %p} | { parent = %p} | { left = %p} | { right = %p}\"];\n",
                node->id, node->id, node->data, node, node->parent, node->left, node->right);
        }

        else{
            fprintf(tree->files.dot,
                    "\tnode%0.3lu [rankdir=LR; fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#e6f2ff\";label = \" { %0.3lu } | { data = %s } | { pointer = %p} | { parent = %p} | { left = %p} | { right = %p}\"];\n",
                    node->id, node->id, node->data, node, node->parent, node->left, node->right);
        }
    }
    // detailed dump

    //simple dump
    else{
        if (node->id == tree->currentNode){
            fprintf(tree->files.dot,
                "\tnode%0.3lu [rankdir=LR; fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#FCFF61\";label = \" { %0.3lu } | { data = %s }\"];\n",
                node->id, node->id, node->data);
        }

        else if (node->id == tree->lastModified){
            fprintf(tree->files.dot,
                "\tnode%0.3lu [rankdir=LR; fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#79FF61\";label = \" { %0.3lu } | { data = %s }\"];\n",
                node->id, node->id, node->data);
        }

        else{
            fprintf(tree->files.dot,
                    "\tnode%0.3lu [rankdir=LR; fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#e6f2ff\";label = \" { %0.3lu } | { data = %s }\"];\n",
                    node->id, node->id, node->data);
        }
    }
    //simple dump

    if (node->left){
        fprintf(tree->files.dot,
                "\tnode%0.3lu -> node%0.3lu [ fontname=\"SF Pro\"; label=\"да\"; weight=1; color=\"#04BF00\"; style=\"bold\"];\n\n",
                node->id, node->left->id);

        NodeDump(tree, node->left, depth + 1, param);
    }

    if (node->right){
        fprintf(tree->files.dot,
                "\tnode%0.3lu -> node%0.3lu [ fontname=\"SF Pro\"; label=\"нет\"; weight=1; color=\"#fd4381\"; style=\"bold\"];\n\n",
                node->id, node->right->id);

        NodeDump(tree, node->right, depth + 1, param);
    }

    return OK;
}

/*=================================================================*/

static int StartTreeDump(tree_t* tree){

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

static int EndTreeDump(tree_t* tree){

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

    if (data < node->data){
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

static int HTMLGenerateBody(tree_t* tree, param_t param){
    fprintf(tree->files.html, "<div style=\"text-align: center;\">\n");

    fprintf(tree->files.html, "\t<h2 style=\"font-family: 'Haas Grot Text R Web', 'Helvetica Neue', Helvetica, Arial, sans-serif;'\"> Dump: %lu</h2>\n", tree->numDump);
    fprintf(tree->files.html, "\t<h3 style=\"font-family: 'Haas Grot Text R Web', 'Helvetica Neue', Helvetica, Arial, sans-serif;'\"> Num elems: %lu</h3>\n", tree->numElem);

    if (param == DETAILED)fprintf(tree->files.html, "\t<h3 style=\"font-family: 'Haas Grot Text R Web', 'Helvetica Neue', Helvetica, Arial, sans-serif;'\"> Root: %p</h3>\n", tree->root);

    fprintf(tree->files.html, "\t<img src=\"./bin/png/output%lu.png\">\n\t<br>\n\t<br>\n\t<br>\n", tree->numDump - 1);

    fprintf(tree->files.html, "</div>\n");

    return OK;
}

int HTMLDumpGenerate(tree_t* tree){


    fprintf(tree->files.html, "</body>\n");
    fprintf(tree->files.html, "</html>\n");

    return OK;
}
