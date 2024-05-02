#include <iostream>
#include <cstring>
#include <unordered_map> // Include for unordered_map
#include <emscripten.h>
#include <emscripten/bind.h>

// Token types
enum TokenType {
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
    const char* value;
    ASTNode *left;
    ASTNode *right;
};

// // Define cache data structure
// ASTNode* Cache[1000];
// int cacheIndex = 0;

#define MAX_PAIRS 100  // Maximum number of key-value pairs 
#define MAX_LENGTH 100 // Maximum length of key or value

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
Token getNextToken(const char *input, int *pos);
Dictionary parseInput(const char* PropertyValueSet, Dictionary* dict);
ASTNode* parseEqualityExpression(Token token);
ASTNode* parseAndExpression(Token token);
ASTNode* parseOrExpression(Token token);
ASTNode* parseExpression(const char *input);
bool evaluate(Dictionary* kv, ASTNode* root);
void printAST(ASTNode *root, int indent);
//void freeAST(ASTNode *root, Dictionary* dict);
ASTNode* getASTFromCache(const std::string& PropertyFilter);
void addToCache(const std::string& PropertyFilter, ASTNode* ast);
void freeAST(ASTNode *root);


// Define cache data structure using unordered_map
std::unordered_map<std::string, ASTNode*> Cache;

// Retrieves the next token from the input string
Token getNextToken(const char *input, int *pos) {
    static char* current = nullptr;
    if (input != nullptr)
        current = (char*)input + *pos;
    if (current == nullptr || *current == '\0')
        return (Token) {TOKEN_END, nullptr};
    while (*current == ' ' || *current == '\t' || *current == ';' || *current == ':' || *current == ',')
        current++;
    if (*current == '=') {
        current++;
        return (Token) {TOKEN_EQUALS, "="};
    } else if (*current == '&') {
        current++;
        return (Token) {TOKEN_AMPERSAND, "&"};
    } else if (*current == '|') {
        current++;
        return (Token) {TOKEN_VERTICAL_BAR, "|"};
    } else if (*current >= '0' && *current <= '9') {
        char value[20];
        int i = 0;
        while (*current >= '0' && *current <= '9') {
            value[i++] = *current++;
        }
        value[i] = '\0';
        return (Token) {TOKEN_INTEGER, strdup(value)};
    } else if ((*current >= 'a' && *current <= 'z') || (*current >= 'A' && *current <= 'Z')) {
        char value[20]; 
        int i = 0;
        while((*current >= 'a' && *current  <= 'z') || (*current >= 'A' && *current  <= 'Z')) {
            value[i++] = *current++;
        }
        value[i] = '\0';
        return (Token) {TOKEN_IDENTIFIER, strdup(value)};
    }
    return (Token) {TOKEN_END, nullptr};
}

// Function to parse input and extract key-value pairs
Dictionary parseInput(const char* PropertyValueSet, Dictionary* dict) {
    if(dict == nullptr || PropertyValueSet == nullptr){
        std::cerr << "Error: either tree or property filter is empty\n";
        exit(EXIT_FAILURE);
    }
    int pos = 0;
    Token token = getNextToken(PropertyValueSet, &pos);

    while (token.type != TOKEN_END) {
        if (token.type != TOKEN_IDENTIFIER) {
            std::cerr << "Error: Expected identifier first\n";
            exit(EXIT_FAILURE);
        }
        const char* key = token.value;

        token = getNextToken(nullptr, &pos);
        if (token.type != TOKEN_EQUALS) {
            std::cerr << "Error: Expected '=' token\n";
            exit(EXIT_FAILURE);
        }

        token = getNextToken(nullptr, &pos);
        if ( token.type != TOKEN_INTEGER) {
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
        //free memory allocated for the token value 
        delete[] token.value;
        delete[] key;
        token = getNextToken(nullptr, &pos);
    }

    return *dict;
}

// Parses equality expressions (e.g., "a=1")
ASTNode* parseEqualityExpression(Token token) {
    ASTNode* node_id = new ASTNode;
    node_id->type = TOKEN_IDENTIFIER;
    if(token.type == TOKEN_END){
        std::cerr << "Error: Invalid input\n";
        exit(EXIT_FAILURE);
    }
    node_id->value = new char[strlen(token.value) + 1]; // Save the variable name
    token = getNextToken(NULL, NULL); // Get next token
    if (token.type != TOKEN_EQUALS) {
        std::cerr << "Error: Expected '=' after identifier\n";
        exit(EXIT_FAILURE);
    }
    ASTNode* node_eq = new ASTNode;
    node_eq->type = TOKEN_EQUALS;
    node_eq->value = new char[strlen(token.value) + 1]; // Save the '=' token
    token = getNextToken(NULL, NULL); // Get next token
    if (token.type != TOKEN_INTEGER) {
        std::cerr << "Error: Expected integer value after '='\n";
        exit(EXIT_FAILURE);
    }
    node_eq->left = node_id;
    node_eq->right = new ASTNode;
    node_eq->right->type = TOKEN_INTEGER;
    node_eq->right->value = new char[strlen(token.value) + 1];
    node_id->left = nullptr;
    node_id->right = nullptr;
    return node_eq;
}

// Parses logical AND expressions (e.g., "a=1&b=2&c=3")
ASTNode* parseAndExpression(Token token) {
    ASTNode* left_node = parseEqualityExpression(token);
    token = getNextToken(NULL, NULL); // Get next token
    if (token.type == TOKEN_AMPERSAND) {
        ASTNode* node_and = new ASTNode;
        node_and->type = TOKEN_AMPERSAND;
        node_and->value = strdup("&");
        node_and->left = left_node;
        node_and->right = parseAndExpression(getNextToken(NULL, NULL));
        return node_and;
    } else if (token.type == TOKEN_VERTICAL_BAR) {
        ASTNode* node = new ASTNode;
        node->type = TOKEN_VERTICAL_BAR;
        node->value = strdup("|");
        node->left = left_node;
        node->right = parseOrExpression(getNextToken(NULL, NULL));
        return node;
    }else if(token.type == TOKEN_END){
        return left_node;
    }
    else{
        std::cerr << "Error: Expected '&' or '|' or end of expression\n";
        exit(EXIT_FAILURE);
    }
}

// Parses logical OR expressions (e.g., "a=1|b=2|c=3")
ASTNode* parseOrExpression(Token token) {
    ASTNode* left = parseAndExpression(token);
   return left;
}

// Main parsing function
ASTNode* parseExpression(const char *input) {
    int pos = 0;
    Token token = getNextToken(input, &pos);
    if(token.type == TOKEN_END){
        std::cerr << "Error: empty input\n";
        exit(EXIT_FAILURE);
    }
    else if(token.type == TOKEN_EQUALS){
        std::cerr << "Error: Expected identifier before '='\n";
        exit(EXIT_FAILURE);
    }
    else{
        ASTNode* result = parseOrExpression(token);
        return result;
    }
}

// Function to print AST (for debugging)
void printAST(ASTNode *root, int level) {
    if (root == nullptr) {
        for (int i = 0; i < level; i++){
            std::cout << "\t";
        }
        std::cout << "-----empty----\n";
        return;
    }

    std::cout << std::string(level * 4, ' '); // Print indentation

    switch (root->type) {
        case TOKEN_INTEGER:
        case TOKEN_IDENTIFIER:
            std::cout << root->value << "\n";
            break;
        case TOKEN_EQUALS:
            std::cout << "=\n";
            break;
        case TOKEN_AMPERSAND:
            std::cout << "&\n";
            break;
        case TOKEN_VERTICAL_BAR:
            std::cout << "|\n";
            break;
        case TOKEN_END:
            std::cout << "-----end----\n";
            break;
        default:
            std::cout << "-----unknown----\n";
            break;
    }

    if (root->left != nullptr || root->right != nullptr) {
        std::cout << std::string((level + 1) * 4, ' ') << "left\n";
        printAST(root->left, level + 1);
        std::cout << std::string((level + 1) * 4, ' ') << "right\n";
        printAST(root->right, level + 1);
    } else {
        std::cout << std::string((level + 1) * 4, ' ') << "-----empty----\n";
    }

    std::cout << std::string(level * 4, ' ') << "done\n";
}

// Function to check if conditions are met 
bool evaluate(Dictionary* kv, ASTNode* root) {
    if (root == nullptr)
        return false;

    switch (root->type) {
        case TOKEN_EQUALS: {
            const char* rootIdnti = root->left->value;
            const char* rootValue = root->right->value;
            for (int i = 0; i < kv->count; i++) {
                if (strcmp(kv->pairs[i].key, rootIdnti) == 0 && strcmp(kv->pairs[i].value, rootValue) == 0)
                    return true;
            }
            break;
        }
        case TOKEN_AMPERSAND:
            if (evaluate(kv, root->left) && evaluate(kv, root->right))
                return true;
            break;
        case TOKEN_VERTICAL_BAR:
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





void freeAST(ASTNode *root) {
    if (root == nullptr) {
        return;
    }

     // Check if the node has already been deleted
    if (root->value == nullptr) {
        return;
    }
    // Free the left and right children first
    freeAST(root->left);
    freeAST(root->right);

    // Delete the value associated with the current node
    if (root->type != TokenType::TOKEN_EQUALS && root->type != TokenType::TOKEN_AMPERSAND && root->type != TokenType::TOKEN_VERTICAL_BAR && root->value != nullptr) {
    
            delete[] root->value;
            root->value = nullptr; // Optional: Set pointer to null after deallocation
    }

 
    // Finally, delete the current node itself
    delete root;
}


// Function to validate
bool isValid(std::string propertyValueSet, std::string propertyFilter) {
//     const char *pvs = propertyValueSet.c_str();
//     const char *pf = propertyFilter.c_str();
//         ValidationResult result; 

//     // // Check if this property filter is already in cache and skip the parsing
//     ASTNode* cachedAST = getASTFromCache(pf);

//     if (cachedAST != nullptr) {
//         // std::cout << "AST found in cache\n";
//          result.ast = cachedAST;
//     }
//     else {
//         //std::cout << "AST not found in cache\n";
//         ASTNode* ast = parseExpression(pf);
//          result.ast = ast;
//         addToCache(pf, ast);
//     }

//     Dictionary dict = {0}; // Initialize dictionary
//     Dictionary kv = parseInput(pvs, &dict);
//      result.result = evaluate(&kv, result.ast);

//     //Free memory allocated for AST and dictionary
//     freeAST(result.ast);

    
//    return result.result;
   return 1;

}


bool isValidUncached(std::string propertyValueSet, std::string propertyFilter) {
    const char *pvs = propertyValueSet.c_str();
    const char *pf = propertyFilter.c_str();
        ValidationResult result;

     ASTNode* ast = parseExpression(pf);
     Dictionary dict = {}; // Initialize dictionary
     Dictionary kv = parseInput(pvs, &dict);

      result.result = evaluate(&kv, ast);
      result.ast = ast;
      result.kv = kv;
//        //Free memory allocated for AST and dictionary
//    // freeAST(ast, nullptr); // Uncomment if you implement a function to free AST nodes
//    // freeDictionary(&dict); // Uncomment if you implement a function to free the dictionary

//     // Alternatively, you can manually free memory for AST nodes
    freeAST(ast);
    return result.result;
    //return 1;

}




// Binding code
EMSCRIPTEN_BINDINGS(Module) {
    // Binding functions with allow_raw_pointer for raw pointers
    emscripten::function("isValid", &isValid);
    emscripten::function("isValidUncached", &isValidUncached);
}
