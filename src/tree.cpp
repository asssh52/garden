#include "../hpp/tree.hpp"
#define MEOW fprintf(stderr, RED "MEOW\n" RESET);

const int64_t POISON        = -52;

const int64_t COMMAND_LEN   = 200; //max system command length
const int64_t MESSAGE_LEN   = 500; //max output message length
const int64_t ANS_LEN       = 50;  //max user input length
const int64_t BUFPTR_LEN    = 20;  //max message depth in 'define' and 'compare'

enum errors{

    OK  =   0,
    ERR =   1

};

enum commands{

    AKINATOR    = 1,
    DEFINE      = 2,
    COMPARE     = 3,
    LOAD        = 4,
    SAVE        = 5,
    HELP        = 6,
    GITSAVE     = 7,
    GITLOAD     = 8,
    TREE_RESET  = 9,
    HLT         = 0

};

// MISCELANEOUS
static int TreeVerify       (tree_t* tree);
static int NodeVerify       (tree_t* tree, node_t* node, int depth);
static int NodeDel          (tree_t* tree, node_t* node);
static int AddNodeElem      (tree_t* tree, node_t* node, data_t data);
static int NodeFind         (tree_t* tree, node_t* node, char* inputValue, node_t** retNode);
// MISCELANEOUS

// SAVE/LOAD
static int LoadTree         (tree_t* tree);
static int LoadNode         (tree_t* tree, node_t* node, param_t param);
static int SaveTree         (tree_t* tree);
static int TreePrint        (tree_t* tree, FILE* file);
static int GitSave          (tree_t* tree);
static int GitLoad          (tree_t* tree);
// SAVE/LOAD

//USER RELATED
static int TreeFind         (tree_t* tree, char* inputValue, node_t** retNode);
static int NewQuestion      (tree_t* tree, node_t* node);
static int ProcessNode      (tree_t* tree, node_t* node);
static int DefineElem       (tree_t* tree);
static int CompareElems     (tree_t* tree);
static int AskQuestion      (node_t* node);
static int AskCommand       (int* retValue);
static int GetAnswer        (int* retValue);
static int GetHelp          ();
//USER RELATED

//DUMP
static int StartTreeDump    (tree_t* tree);
static int DoDot            (tree_t* tree);
static int EndTreeDump      (tree_t* tree);
static int HTMLGenerateHead (tree_t* tree);
static int HTMLGenerateBody (tree_t* tree, param_t param);
static int NodeDump         (tree_t* tree, node_t* node, int depth, param_t param);
static int NodePrint        (tree_t* tree, node_t* node, int depth, FILE* file);
//DUMP



/*=================================================================*/
//starting programm

int StartAkinator(tree_t* tree){

    int command = POISON;

    AskCommand(&command);

    while (command != HLT){
        switch(command){
            case AKINATOR:
                ProcessNode(tree, tree->root);
                break;

            case DEFINE:
                DefineElem(tree);
                break;

            case COMPARE:
                CompareElems(tree);
                break;

            case LOAD:
                LoadTree(tree);
                break;

            case SAVE:
                SaveTree(tree);
                break;

            case HELP:
                GetHelp();
                break;

            case GITSAVE:
                GitSave(tree);
                break;

            case GITLOAD:
                GitLoad(tree);
                break;

            case TREE_RESET:
                TreeDel(tree);
                break;

            case HLT:
                break;

            default:
                break;
        }

        command = POISON;
        if (command) AskCommand(&command);
    }

    return OK;
}

static int GetHelp(){
    printf(BLU "доступные команды:\n");
    printf(CYN "- \"акинатор\" \n");
    printf(CYN "- \"определить\" \n");
    printf(CYN "- \"сравнить\" \n");
    printf(CYN "- \"загрузить\" \n");
    printf(CYN "- \"сохранить\" \n");
    printf(CYN "- \"помощь\" \n");
    printf(CYN "- \"гит загрузить\" \n");
    printf(CYN "- \"гит сохранить\" \n");
    printf(CYN "- \"сбросить\" \n");
    printf(CYN "- \"закончить\" \n" RESET);

    return OK;
}

/*=================================================================*/
//loading tree from file

static int LoadTree(tree_t* tree){

    //open save
    tree->files.save = fopen(tree->files.saveName, "r");
    if (!tree->files.save){
        tree->files.save     = fopen("save.txt", "r");
        tree->files.saveName = "save.txt";
    }
    //open save
    TreeDel(tree);

    tree->numElem = 0;
    LoadNode(tree, tree->root, ROOT);

    TreeDump(tree);
    return OK;
}

/*=================================================================*/
//loading node from file

static int LoadNode(tree_t* tree, node_t* node, param_t param){
    char* newData   = (char*)calloc(ANS_LEN, sizeof(*newData));
    char* newData2  = (char*)calloc(ANS_LEN, sizeof(*newData));
    node_t* newNode = nullptr;
    char buffer[MESSAGE_LEN] = {};
    char bracket = 0;

    if (param == ROOT){
        fscanf(tree->files.save, "%[^{}]", buffer);
        bracket = getc(tree->files.save);

        if (bracket == '{'){
            NewNode(tree, newData, nullptr, ROOT, &tree->root);
        }
        else return 0;
        fscanf(tree->files.save, "\"%[^\"]\"", newData);

        fscanf(tree->files.save, "%[^{}]", buffer);
        bracket = getc(tree->files.save);

        if (bracket == '{'){
            ungetc(bracket, tree->files.save);
            LoadNode(tree, tree->root, LEFT);
        }


        LoadNode(tree, tree->root, RIGHT);
        return OK;
    }

    else if (param == LEFT){
        fscanf(tree->files.save, "%[^{}]", buffer);
        bracket = getc(tree->files.save);

        if (bracket == '{'){
            NewNode(tree, newData, node, LEFT, &newNode);
        }
        else return 0;
        fscanf(tree->files.save, "\"%[^\"]\"", newData);

        fscanf(tree->files.save, "%[^{}]", buffer);
        bracket = getc(tree->files.save);

        if (bracket == '{'){
            ungetc(bracket, tree->files.save);
            LoadNode(tree, newNode, LEFT);
        }

        LoadNode(tree, node, RIGHT);

        return OK;
    }

    else if (param == RIGHT){
        fscanf(tree->files.save, "%[^{}]", buffer);
        bracket = getc(tree->files.save);

        if (bracket == '{'){
            NewNode(tree, newData, node, RIGHT, &newNode);
        }
        else return OK;
        fscanf(tree->files.save, "\"%[^\"]\"", newData);

        fscanf(tree->files.save, "%[^{}]", buffer);
        bracket = getc(tree->files.save);

        if (bracket == '{'){
            ungetc(bracket, tree->files.save);
            LoadNode(tree, newNode, LEFT);
        }

        LoadNode(tree, newNode, RIGHT);
        return OK;
    }
}

/*=================================================================*/
//saving tree to file

static int SaveTree(tree_t* tree){
    if (TreeVerify(tree)) return ERR;

    tree->files.save = fopen(tree->files.saveName, "w");
    if (!tree->files.save){
        tree->files.save     = fopen("save.txt", "w");
        tree->files.saveName = "save.txt";
    }

    TreePrint(tree, tree->files.save);

    fclose(tree->files.save);

    return OK;
}

/*=================================================================*/
//getting user answer to node question

static int ProcessNode(tree_t* tree, node_t* node){
    int retValue = POISON;
    tree->currentNode = node->id;
    TreeDump(tree);

    AskQuestion(node);
    GetAnswer(&retValue);

    if (retValue){
        if (node->left) ProcessNode(tree, node->left);

        else printf(MAG "ура!!!\n" RESET);
    }

    else{
        if (node->right) ProcessNode(tree, node->right);

        else NewQuestion(tree, node);
    }

    return OK;
}

/*=================================================================*/
// defining elem

static int DefineElem(tree_t* tree){
    if (TreeVerify(tree)) return ERR;

    char inputValue[ANS_LEN] = {};
    node_t* retNode = nullptr;

    printf(GRN "введите вашу штуку\n" RESET);
    scanf("\n%[^\n]", inputValue);
    TreeFind(tree, inputValue, &retNode);

    while (!retNode){
        printf(RED "не могу найти, повторите запрос\n" RESET);
        scanf("\n%[^\n]", inputValue);
        TreeFind(tree, inputValue, &retNode);
    }

    //root path
    node_t* currentNode = retNode;
    char*   charPath[BUFPTR_LEN] = {};
    int      intPath[BUFPTR_LEN] = {};

    int counter = 0;
    while (currentNode != tree->root){
        if      (currentNode == currentNode->parent->left)  intPath[counter] = 1;
        else if (currentNode == currentNode->parent->right) intPath[counter] = -1;

        currentNode = currentNode->parent;
        charPath[counter] = currentNode->data;

        counter++;
    }
    //root path

    //out
    char outMessage[MESSAGE_LEN] = {};
    for (int i = counter - 1; i >= 0; i--){
        if (i == counter - 1) snprintf(outMessage, MESSAGE_LEN, "это");

        if (intPath[i] == 1){
            snprintf(outMessage, MESSAGE_LEN, "%s %s", outMessage, charPath[i]);
        }

        else{
            snprintf(outMessage, MESSAGE_LEN, "%s не %s", outMessage, charPath[i]);
        }

        if (i != 0) snprintf(outMessage, MESSAGE_LEN, "%s,", outMessage);
    }

    printf(CYN "%s\n" RESET, outMessage);
    //out

    return 0;
}

/*=================================================================*/
//comparing two elems

static int CompareElems(tree_t* tree){
    char inputValueFirst[ANS_LEN] = {};
    char inputValueSecond[ANS_LEN] = {};
    node_t* retNodeFirst = nullptr;
    node_t* retNodeSecond = nullptr;

    //first item
    printf(GRN "введите вашу первую штуку\n" RESET);
    scanf("\n%[^\n]", inputValueFirst);
    TreeFind(tree, inputValueFirst, &retNodeFirst);

    while (!retNodeFirst){
        printf(RED "не могу найти, повторите запрос\n" RESET);
        scanf("\n%[^\n]", inputValueFirst);
        TreeFind(tree, inputValueFirst, &retNodeFirst);
    }
    //first item

    //second item
    printf(GRN "введите вашу вторую штуку\n" RESET);
    scanf("\n%[^\n]", inputValueSecond);
    TreeFind(tree, inputValueSecond, &retNodeSecond);

    while (!retNodeSecond){
        printf(RED "не могу найти, повторите запрос\n" RESET);
        scanf("\n%[^\n]", inputValueSecond);
        TreeFind(tree, inputValueSecond, &retNodeSecond);
    }
    //second item

    //root path 1
    node_t* currentNode = retNodeFirst;

    char*   charPathFirst[BUFPTR_LEN] = {};
    int      intPathFirst[BUFPTR_LEN] = {};

    int counterFirst = 0;
    while (currentNode != tree->root){
        if      (currentNode == currentNode->parent->left)  intPathFirst[counterFirst] = 1;
        else if (currentNode == currentNode->parent->right) intPathFirst[counterFirst] = -1;

        currentNode = currentNode->parent;
        charPathFirst[counterFirst] = currentNode->data;

        counterFirst++;
    }
    //root path 1

    //root path 2
    currentNode = retNodeSecond;
    char*   charPathSecond[BUFPTR_LEN] = {};
    int      intPathSecond[BUFPTR_LEN] = {};

    int counterSecond = 0;
    while (currentNode != tree->root){
        if      (currentNode == currentNode->parent->left)  intPathSecond[counterSecond] = 1;
        else if (currentNode == currentNode->parent->right) intPathSecond[counterSecond] = -1;

        currentNode = currentNode->parent;
        charPathSecond[counterSecond] = currentNode->data;

        counterSecond++;
    }
    //root path 2

    char outMessage[MESSAGE_LEN] = {};

    snprintf(outMessage, MESSAGE_LEN, "%s похож на %s тем что, они:\n", inputValueFirst, inputValueSecond);

    for (int ctr1 = counterFirst - 1, ctr2 = counterSecond - 1; ctr1 >= 0 || ctr2 >= 0; ctr1--, ctr2--){
        if (ctr1 >= 0 && ctr2 >= 0 && charPathFirst[ctr1] == charPathSecond[ctr2]){
            if (intPathFirst[ctr1] == intPathSecond[ctr2] && intPathSecond[ctr2] == 1){
                snprintf(outMessage, MESSAGE_LEN, "%s %s", outMessage, charPathFirst[ctr1]);
            }

            else if (intPathFirst[ctr1] == intPathSecond[ctr2] && intPathSecond[ctr2] == -1){
                snprintf(outMessage, MESSAGE_LEN, "%s не %s", outMessage, charPathFirst[ctr1]);
            }

            else if (intPathFirst[ctr1] == -1 && intPathSecond[ctr2] == 1){
                snprintf(outMessage, MESSAGE_LEN, "%s отличаются тем, что %s не %s", outMessage, inputValueFirst, charPathFirst[ctr1]);
            }

            else{
                snprintf(outMessage, MESSAGE_LEN, "%s отличаются тем, что %s не %s", outMessage, inputValueSecond, charPathFirst[ctr1]);
            }

            if(intPathFirst[ctr1] == intPathSecond[ctr2]) snprintf(outMessage, MESSAGE_LEN, "%s,", outMessage);
        }
    }

    printf("%s\n", outMessage);

    return OK;
}

/*=================================================================*/
// finding elem in tree

static int TreeFind(tree_t* tree, char* inputValue, node_t** retNode){
    NodeFind(tree, tree->root, inputValue, retNode);

    return OK;
}

static int NodeFind(tree_t* tree, node_t* node, char* inputValue, node_t** retNode){
    if (!strcmp(node->data, inputValue)){
        *retNode = node;
        return OK;
    }
    if(node->left)  NodeFind(tree, node->left,  inputValue, retNode);
    if(node->right) NodeFind(tree, node->right, inputValue, retNode);

    return OK;
}

/*=================================================================*/
// creating new question

static int NewQuestion(tree_t* tree, node_t* node){
    char* newData        = (char*)calloc(ANS_LEN, sizeof(char));
    char* newQuestion    = (char*)calloc(ANS_LEN, sizeof(char));

    printf(GRN "кто это был?\n" RESET);
    scanf("\n%[^\n]", newData);

    printf(GRN "чем отличается " CYN "%s" GRN " от " CYN "%s" GRN "?\n" RESET, newData, node->data);
    scanf("\n%[^\n]", newQuestion);

    NewNode(tree, node->data, node, RIGHT, nullptr);
    NewNode(tree, newData, node, LEFT, nullptr);

    node->data = newQuestion;

    return OK;
}

/*=================================================================*/
//user input/answers

static int AskCommand(int* retValue){
    char buffer[ANS_LEN] = {};

    while (*retValue == POISON){
        scanf("\n%[^\n]", buffer);

        if      (!strcmp(buffer, "акинатор"))       *retValue = AKINATOR;
        else if (!strcmp(buffer, "определить"))     *retValue = DEFINE;
        else if (!strcmp(buffer, "сравнить"))       *retValue = COMPARE;
        else if (!strcmp(buffer, "загрузить"))      *retValue = LOAD;
        else if (!strcmp(buffer, "сохранить"))      *retValue = SAVE;
        else if (!strcmp(buffer, "помощь"))         *retValue = HELP;
        else if (!strcmp(buffer, "гит сохранить"))  *retValue = GITSAVE;
        else if (!strcmp(buffer, "гит загрузить"))  *retValue = GITLOAD;
        else if (!strcmp(buffer, "сбросить"))       *retValue = TREE_RESET;
        else if (!strcmp(buffer, "закончить"))      *retValue = HLT;
        else{
            printf(RED "неверный ввод, доступные команды: " CYN "\"помощь\"\n" RESET);
        }
    }

    return OK;
}

static int AskQuestion(node_t* node){
    char message[MESSAGE_LEN]= {};
    snprintf(message, MESSAGE_LEN, GRN "это " BLU "%s" GRN "?\n" RESET, node->data);

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
            printf(RED "неверный ввод, введите \"да\" или \"нет\"\n" RESET);
        }
    }

    return OK;
}

/*=================================================================*/
//creating new node

int NewNode(tree_t* tree, data_t data, node_t* parentNode, param_t param, node_t** returnNode){
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
// creating tree struct

int TreeCtor(tree_t* tree){
    if (!tree) return ERR;

    tree->files.log = fopen(tree->files.logName, "w");
    if (!tree->files.log) tree->files.log = stdout;

    tree->files.html = fopen(tree->files.htmlName, "w");
    if (!tree->files.html){
        tree->files.html     = fopen("htmldump.html", "w");
        tree->files.htmlName = "htmldump.html";
    }

    NewNode(tree, "никто", nullptr, ROOT, &tree->root);

    HTMLGenerateHead(tree);

    if (TreeVerify(tree)) return ERR;
    fprintf(tree->files.log, "tree created\n");

    return OK;
}

/*=================================================================*/
//deleting tree/nodes

int TreeDtor(tree_t* tree){
    if (TreeVerify(tree)) return ERR;

    TreeDel(tree);
    NodeDel(tree, tree->root);

    if(tree->files.dot) fclose(tree->files.dot);
    if(tree->files.log && tree->files.log != stdout) fclose(tree->files.log);
    if(tree->files.html) fclose(tree->files.html);

    return OK;
}

int TreeDel(tree_t* tree){
    if (TreeVerify(tree)) return ERR;

    NodeDel(tree, tree->root);

    NewNode(tree, "никто", nullptr, ROOT, &tree->root);

    return OK;
}

static int NodeDel(tree_t* tree, node_t* node){

    if (node->left)  NodeDel(tree, node->left);

    if (node->right) NodeDel(tree, node->right);

    free(node);
    tree->numElem--;

    TreeDump(tree);

    return OK;
}

/*=================================================================*/
// verifycator for loops

static int TreeVerify(tree_t* tree){
    if (NodeVerify(tree, tree->root, 0)) return ERR;

    return OK;
}

static int NodeVerify(tree_t* tree, node_t* node, int depth){
    if (depth > tree->numElem) return ERR;

    if (node->left)     NodeVerify(tree, node->left, depth + 1);
    if (node->right)    NodeVerify(tree, node->right, depth + 1);

    return OK;
}

/*=================================================================*/
// output tree to file

static int TreePrint(tree_t* tree, FILE* file){
    if (TreeVerify(tree)) return ERR;

    fprintf(tree->files.log, "\ntree print:\n");

    fprintf(file, "\n");
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
// dump

int TreeDump(tree_t* tree){
    //fprintf(tree->files.log, "\ntree dump#%lu started\n", tree->numDump + 1);

    StartTreeDump(tree);
    NodeDump(tree, tree->root, 0, SIMPLE);
    EndTreeDump(tree);

    DoDot(tree);
    HTMLGenerateBody(tree, SIMPLE);

    //fprintf(tree->files.log, "tree dumped\n");
    return OK;
}

static int NodeDump(tree_t* tree, node_t* node, int depth, param_t param){
    if (!node) return OK;
    if (depth > tree->numElem) return ERR;

    // detailed dump
    if (param == DETAILED){
        if (node->id == tree->currentNode){
            fprintf(tree->files.dot,
                "\tnode%0.3lu [rankdir=LR; fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#FCFF61\";label = \"{{ %0.3lu } | { data = %s } | { pointer = %p} | { parent = %p} | { left = %p} | { right = %p}}\"];\n",
                node->id, node->id, node->data, node, node->parent, node->left, node->right);
        }

        else if (node->id == tree->lastModified){
            fprintf(tree->files.dot,
                "\tnode%0.3lu [rankdir=LR; fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#79FF61\";label = \"{{ %0.3lu } | { data = %s } | { pointer = %p} | { parent = %p} | { left = %p} | { right = %p}}\"];\n",
                node->id, node->id, node->data, node, node->parent, node->left, node->right);
        }

        else{
            fprintf(tree->files.dot,
                    "\tnode%0.3lu [rankdir=LR; fontname=\"SF Pro\"; shape=Mrecord; style=filled; color=\"#e6f2ff\";label = \"{{ %0.3lu } | { data = %s } | { pointer = %p} | { parent = %p} | { left = %p} | { right = %p}}\"];\n",
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
// part of tree dump

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
// adding elements

int AddTreeElem(tree_t* tree, data_t data){
    if (TreeVerify(tree)) return ERR;

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
// working with git

static int GitSave(tree_t* tree){
    if (TreeVerify(tree)) return ERR;

    char systemCommand[COMMAND_LEN] = {};

    tree->files.save = fopen("../database/akinator_db/save.txt", "w");
    TreePrint(tree, tree->files.save);
    fclose(tree->files.save);

    //uploading
    snprintf(systemCommand, COMMAND_LEN, "cd ../database/akinator_db && git add save.txt && git commit -C main && git push");
    system(systemCommand);

    return OK;
}


static int GitLoad(tree_t* tree){
    TreeVerify(tree);

    char systemCommand[COMMAND_LEN] = {};

    //remove old dirs
    snprintf(systemCommand, COMMAND_LEN, "cd ../database && rm -rf akinator_db");
    system(systemCommand);

    //load from git
    snprintf(systemCommand, COMMAND_LEN, "cd ../database && git clone https://github.com/asssh52/akinator_db");
    system(systemCommand);

    char saveName[COMMAND_LEN] = "../database/akinator_db/save.txt";
    tree->files.saveName = saveName;

    LoadTree(tree);

    return OK;
}
/*=================================================================*/
//generating html/png

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
