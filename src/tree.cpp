
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdbool>
#include <unordered_map> // Include for unordered_map


// Token types
enum class TokenType {
    TOKEN_IDENTIFIER,
    TOKEN_EQUALS,
    TOKEN_AMPERSAND,
    TOKEN_VERTICAL_BAR,
    TOKEN_INTEGER,
    TOKEN_END
};

// Token structure
struct Token {
    TokenType type;
    const char* value;
};

// Abstract Syntax Tree Node
struct ASTNode {
    TokenType type;
    char* value;
    ASTNode *left;
    ASTNode *right;
};



// Define cache data structure
// ASTNode* Cache[1000];
// int cacheIndex = 0;

#define MAX_PAIRS 10  // Maximum number of key-value pairs 
#define MAX_LENGTH 20 // Maximum length of key or value

// Structure to store key-value pairs
struct KeyValuePair {
    char key[MAX_LENGTH];
    char value[MAX_LENGTH];
};

// Structure to store dictionary
struct Dictionary {
    KeyValuePair pairs[MAX_PAIRS];
    int count;
};

// Structure to store the result of validation
struct ValidationResult {
    bool result;
    ASTNode* ast;
    Dictionary kv;
};

// Function prototypes
Token getNextToken(char *input, int *pos);
ASTNode* parseExpression(char *input);
ASTNode* parseEqualityExpression(Token token);
ASTNode* parseAndExpression(Token token);
ASTNode* parseOrExpression(Token token);
Dictionary parseInput(char* PropertyValueSet, Dictionary* dict);
void printAST(ASTNode *root, int indent);
bool evaluate(Dictionary* kv, ASTNode* root);
ValidationResult isValid(char* PropertyValueSet, char* PropertyFilter);
void freeAST(ASTNode *root);
ASTNode* getASTFromCache(char* PropertyFilter);
void addToCache(char* PropertyFilter, ASTNode* ast);
void freeASTCache();
void freeDict(Dictionary* dict);
ValidationResult isValidUncached(char* PropertyValueSet, char* PropertyFilter);

// Free the memory associated with the AST cache
// Define cache data structure using unordered_map
std::unordered_map<std::string, ASTNode*> Cache;

// Free the memory associated with the AST cache
void freeASTCache() {
    for (auto& pair : Cache) {
        freeAST(pair.second);
    }
    Cache.clear();
}

// Main parsing function
ASTNode* parseExpression(char *input) {
    int pos = 0;
    Token token = getNextToken(input, &pos);
    if(token.type == TokenType::TOKEN_END){
        std::cerr << "Error: empty input\n";
        exit(EXIT_FAILURE);
    }
    else if(token.type == TokenType::TOKEN_EQUALS){
        std::cerr << "Error: Expected identifier before '='\n";
        exit(EXIT_FAILURE);
    }
    else
        return parseOrExpression(token);
        // return NULL;
}

// Parses equality expressions (e.g., "a=1")
ASTNode* parseEqualityExpression(Token token) {
    printf("NEW ASTNode\n");
    ASTNode* node_id = new ASTNode();
    node_id->type = TokenType::TOKEN_IDENTIFIER;
    if(token.type == TokenType::TOKEN_END){
        std::cerr << "Error: Invalid input\n";
        exit(EXIT_FAILURE);
    }
    printf("NEW allocate memory for the identifier \n");
    node_id->value = new char[strlen(token.value) + 1]; // allocate memory for the identifier 
    strcpy(node_id->value, token.value);
    token = getNextToken(nullptr, nullptr); // Get next token
    if (token.type != TokenType::TOKEN_EQUALS) {
        std::cerr << "Error: Expected '=' after identifier\n";
        exit(EXIT_FAILURE);
    }
    printf("NEW ASTNode\n");
    ASTNode* node_eq = new ASTNode();
    node_eq->type = TokenType::TOKEN_EQUALS;
    printf("NEW // Save the '=' token\n");
    node_eq->value = new char[strlen(token.value) + 1]; // Save the '=' token
    strcpy(node_eq->value, token.value);
    token = getNextToken(nullptr, nullptr); // Get next token
    if (token.type != TokenType::TOKEN_INTEGER) {
        std::cerr << "Error: Expected integer value after '='\n";
        exit(EXIT_FAILURE);
    }
    node_eq->left = node_id;
    printf("NEW ASTNode\n");
    node_eq->right = new ASTNode();
    node_eq->right->type = TokenType::TOKEN_INTEGER;
    printf("NEW new char[strlen(token.value) + 1]\n");
    node_eq->right->value = new char[strlen(token.value) + 1]; 
    strcpy(node_eq->right->value, token.value);
    node_id->left = nullptr;
    node_id->right = nullptr;
    return node_eq;
}

// Parses logical AND expressions (e.g., "a=1&b=2&c=3")
ASTNode* parseAndExpression(Token token) {
    ASTNode* left_node = parseEqualityExpression(token);
    token = getNextToken(nullptr, nullptr); // Get next token
    if (token.type == TokenType::TOKEN_AMPERSAND) {
        printf("NEW new ASTNode\n");
        ASTNode* node_and = new ASTNode();
        node_and->type = TokenType::TOKEN_AMPERSAND;
        printf("NEW new char[2]\n");
        node_and->value = new char[2]; // Allocate memory for the "&" character and null terminator
        strcpy(node_and->value, "&");
        node_and->left = left_node;
        node_and->right = parseAndExpression(getNextToken(nullptr, nullptr));
        printf("DELETE delete[] token.value\n");
        delete[] token.value;
        return node_and;
    } else if (token.type == TokenType::TOKEN_VERTICAL_BAR) {
        printf("NEW ASTNode\n");
        ASTNode* node = new ASTNode();
        node->type = TokenType::TOKEN_VERTICAL_BAR;
        printf("NEW char[2]");
        node->value = new char[2]; // Allocate memory for the "|" character and null terminator
        strcpy(node->value, "|");
        node->left = left_node;
        node->right = parseOrExpression(getNextToken(nullptr, nullptr));
        //delete[] token.value;   
        return node;
    }else if(token.type == TokenType::TOKEN_END){
        printf("DELETE delete[] token.value\n");
        delete[] token.value;
        return left_node;
    }
    else{
        std::cerr << "Error: Expected '&' or '|' or end of expression\n";
        exit(EXIT_FAILURE);
    }
}

// Parses logical OR expressions (e.g., "a=1|b=2|c=3")
ASTNode* parseOrExpression(Token token) {
    return parseAndExpression(token);
}

// Retrieves the next token from the input string
Token getNextToken(char *input, int *pos) {
   static char* current = nullptr;
    if (input != nullptr)
        current = input + *pos;
    if (current == nullptr || *current == '\0')
        return {TokenType::TOKEN_END, nullptr};
    while (*current == ' ' || *current == '\t' || *current == ';' || *current == ':' || *current == ',')
        current++;
                 

    if (*current == '=') {
        current++;
        return {TokenType::TOKEN_EQUALS, "="};
    } else if (*current == '&') {
        current++;
        return {TokenType::TOKEN_AMPERSAND, "&"};
    } else if (*current == '|') {
        current++;
        return {TokenType::TOKEN_VERTICAL_BAR, "|"};
    } else if (*current >= '0' && *current <= '9') {
        char value[20];
        int i = 0;
        while (*current >= '0' && *current <= '9') {
            value[i++] = *current++;
        }
        value[i] = '\0';
        printf("NEW new char[i + 1]\n");
        char* intValue = new char[i + 1]; // Allocate memory for the integer value
        strcpy(intValue, value);
        //delete[] value;
        return {TokenType::TOKEN_INTEGER, intValue};    
    } else if ((*current >= 'a' && *current <= 'z') || (*current >= 'A' && *current <= 'Z')) {
        char value[20]; 
        int i = 0;
        while((*current >= 'a' && *current  <= 'z') || (*current >= 'A' && *current  <= 'Z')) {
            value[i++] = *current++;
        }
        value[i] = '\0';
        printf("NEW new char[i + 1]\n");
        char* identifierValue = new char[i + 1]; // Allocate memory for the identifier value
        strcpy(identifierValue, value);
        return {TokenType::TOKEN_IDENTIFIER, identifierValue};
    }
    return {TokenType::TOKEN_END, nullptr};
}

// Print AST (for debugging)
void printAST(ASTNode *root, int level) {
    if (root == nullptr) {
        return;
    }
    int i;
    for ( i = 0; i < level; i++) {
        std::cout << "\t";
    }

    switch (root->type) {
        case TokenType::TOKEN_INTEGER:
            std::cout << root->value << std::endl;
            break;
        case TokenType::TOKEN_IDENTIFIER:
            std::cout << root->value << std::endl;
            break;
        case TokenType::TOKEN_EQUALS:
            std::cout << "=" << std::endl;
            break;
        case TokenType::TOKEN_AMPERSAND:
            std::cout << "&" << std::endl;
            break;
        case TokenType::TOKEN_VERTICAL_BAR:
            std::cout << "|" << std::endl;
            break;
        case TokenType::TOKEN_END:
            std::cout << "-----end----" << std::endl;
            break;
        default:
            std::cout << "-----unknown----" << std::endl;
            break;
    }

    if (root->left != nullptr || root->right != nullptr) {
        for (int i = 0; i <= level; i++) {
            std::cout << "\t";
        }
        std::cout << "left" << std::endl;
        printAST(root->left, level + 1);
        for (int i = 0; i <= level; i++) {
            std::cout << "\t";
        }
        std::cout << "right" << std::endl;
        printAST(root->right, level + 1);
    } else {
        for (int i = 0; i <= level; i++) {
            std::cout << "\t";
        }
        std::cout << "-----empty----" << std::endl;
    }

    for (int i = 0; i < level; i++) {
        std::cout << "\t";
    }
    std::cout << "done" << std::endl;
}

// Function to parse input and extract key-value pairs
Dictionary parseInput(char* PropertyValueSet, Dictionary* dict) {
    if(dict == nullptr || PropertyValueSet == nullptr){
        std::cerr << "Error: either tree or property filter is empty\n";
        exit(EXIT_FAILURE);
    }
    int pos = 0;
    Token token = getNextToken(PropertyValueSet, &pos);

    while (token.type != TokenType::TOKEN_END) {
        if (token.type != TokenType::TOKEN_IDENTIFIER) {
            std::cerr << "Error: Expected identifier first\n";
            exit(EXIT_FAILURE);
        }
        const char* key = token.value;

        token = getNextToken(nullptr, &pos);
        if (token.type != TokenType::TOKEN_EQUALS) {
            std::cerr << "Error: Expected '=' token\n";
            exit(EXIT_FAILURE);
        }

        token = getNextToken(nullptr, &pos);
        if ( token.type != TokenType::TOKEN_INTEGER) {
            std::cerr << "Error: Expected value\n";
            exit(EXIT_FAILURE);
        }
        const char* value = token.value;

        if (dict->count < MAX_PAIRS) {
            strcpy(dict->pairs[dict->count].key, key);
            strcpy(dict->pairs[dict->count].value, value);
            dict->count++;
        } else {
            std::cerr << "Error: Maximum number of key-value pairs exceeded\n";
            exit(EXIT_FAILURE);
        }

        token = getNextToken(nullptr, &pos);
    }

    return *dict;
}

// Function to check if conditions are met 
bool evaluate(Dictionary* kv, ASTNode* root) {
    if (root == nullptr)
        return false;

    switch (root->type) {
        case TokenType::TOKEN_EQUALS: {
            char* rootIdnti = root->left->value;
            char* rootValue = root->right->value;
            for (int i = 0; i < kv->count; i++) {
                if (strcmp(kv->pairs[i].key, rootIdnti) == 0 && strcmp(kv->pairs[i].value, rootValue) == 0)
                    return true;
            }
            break;
        }
        case TokenType::TOKEN_AMPERSAND:
            if (evaluate(kv, root->left) && evaluate(kv, root->right))
                return true;
            break;
        case TokenType::TOKEN_VERTICAL_BAR:
            if (evaluate(kv, root->left) || evaluate(kv, root->right))
                return true;
            break;
        default:
            std::cerr << "Error: Invalid node type\n";
            exit(EXIT_FAILURE);
    }
    
    return false;
}

// Function to get AST from cache
ASTNode* getASTFromCache(const std::string& PropertyFilter) {
    auto it = Cache.find(PropertyFilter);
    if (it != Cache.end()) {
        return it->second;
    }
    return nullptr;
}

// Function to add AST to cache
void addToCache(const std::string& PropertyFilter, ASTNode* ast) {
    Cache[PropertyFilter] = ast;
}

// Free the memory associated with the dictionary
void freeDict(Dictionary* dict) {
    // for (int i = 0; i < dict->count; ++i) {
    //     delete[] dict->pairs[i].key;
    //     delete[] dict->pairs[i].value;
    // }
    // Reset the count to 0
    dict->count = 0;
}



// Validate input
ValidationResult isValid(char* PropertyValueSet, char* PropertyFilter) {
    ValidationResult result; 
    std::string filterKey(PropertyFilter); // Convert char* to std::string
    // ASTNode* cachedAST = getASTFromCache(filterKey);

    // if (cachedAST != nullptr) {
    //     std::cout << "AST found in cache\n";
    //     result.ast = cachedAST;
    // }
    // else {
    //     std::cout << "AST not found in cache\n";
        ASTNode* ast = parseExpression(PropertyFilter);
        result.ast = ast;
    //     addToCache(filterKey, ast);
    // }
    // printAST(result.ast, 0);
    // Dictionary dict = {};
    // Dictionary kv = parseInput(PropertyValueSet, &dict);

    // result.result = evaluate(&kv, result.ast);
    // result.kv = kv;

    // free memmory 
    freeAST(result.ast);
    //printAST(result.ast, 0);
//    freeDict(&kv);
   //delete[] result.ast->value;

   
    return result;
}

ValidationResult isValidUncached(char* PropertyValueSet, char* PropertyFilter) {
    ValidationResult result;
    ASTNode* ast = parseExpression(PropertyFilter);
    Dictionary dict = {}; // Initialize dictionary
    Dictionary kv = parseInput(PropertyValueSet, &dict);

    result.result = evaluate(&kv, ast);
    result.ast = ast;
    result.kv = kv;

    // free memmory 
     freeAST(result.ast);
     freeDict(&dict);

    printf("DELETE delete[] result.ast->value\n");
    delete[] result.ast->value;

    

    return result;
}


void freeAST(ASTNode *root) {
    if (root == nullptr) {
        return;
    }

    // Print the current node before deleting it
    // switch (root->type) {
    //     case TokenType::TOKEN_INTEGER:
    //         std::cout << "Root value before deleting Integer node: " << root->value << std::endl;
    //         break;
    //     case TokenType::TOKEN_IDENTIFIER:
    //         std::cout << "Root value before deleting Identifier node: " << root->value << std::endl;
    //         break;
    //     case TokenType::TOKEN_EQUALS:
    //         std::cout << "Root value before deleting Equals node:" << root->value<< std::endl;
    //         break;
    //     case TokenType::TOKEN_AMPERSAND:
    //         std::cout << "Root value before deleting Ampersand node:" << root->value<<std::endl;
    //         break;
    //     case TokenType::TOKEN_VERTICAL_BAR:
    //         std::cout << "Root value before deleting Vertical Bar node:" << root->value<< std::endl;
    //         break;
    //     default:
    //        // std::cout << "Root value before deleting Unknown node:" << root->value << std::endl;
    //         break;
    // }

    freeAST(root->left);
    freeAST(root->right);
    
    // Print the current node before deleting it
    // switch (root->type) {
    //     case TokenType::TOKEN_INTEGER:
    //         std::cout << "Deleting Integer node: " << root->value << std::endl;
    //         break;
    //     case TokenType::TOKEN_IDENTIFIER:
    //         std::cout << "Deleting Identifier node: " << root->value << std::endl;
    //         break;
    //     case TokenType::TOKEN_EQUALS:
    //         std::cout << "Deleting Equals node: " << root->value << std::endl;
    //         break;
    //     case TokenType::TOKEN_AMPERSAND:
    //         std::cout << "Deleting Ampersand node: " << root->value  << std::endl;
    //         break;
    //     case TokenType::TOKEN_VERTICAL_BAR:
    //         std::cout << "Deleting Vertical Bar node: " << root->value  << std::endl;
    //         break;
    //     default:
    //         std::cout << "Deleting Unknown node: " << root->value  << std::endl;
    //         break;
    // }

 // Deallocate memory for value field in Token struct if it's not null
    if (root->value != nullptr) {
        printf("DELETE delete[] root->value\n");
        delete[] root->value;
        root->value = nullptr; // Optional: Set pointer to null after deallocation
    }

    // delete[] root->value; // Delete value memory
    printf("DELETE delete root\n");
     delete root;
}




int main() {
    // char PropertyFilter[] = "GGG=3 | pp=4";
    char PropertyFilter[] = "G=1";
    ASTNode* ast = parseExpression(PropertyFilter);
    freeAST(ast);
    return 0;

/*
    char PropertyValueSet[] = "GGG=9 : abc=4 : pp=4 : xyz=5";
    int i;
    for ( i = 0; i < 1; ++i) {
        ValidationResult VlidResult = isValid(PropertyValueSet, PropertyFilter);
        std::cout << "Run " << i + 1 << " - Result is: " << VlidResult.result << std::endl;
    }

    // for (int i = 0; i < 1; ++i) {
    //     ValidationResult VlidResult = isValidUncached(PropertyValueSet, PropertyFilter);
    //     std::cout << "Run " << i + 1 << " - Result uncached is: " << VlidResult.result << std::endl;
    // }

    freeASTCache(); 

    return 0;
    */
}
