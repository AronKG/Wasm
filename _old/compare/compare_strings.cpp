// here we will copare two stringes and return a boolean 0 0r 1, then export the functio that we will call from javascript 
#include <string>
#include <iostream> // Include for cout and endl for input and output 
#include <map>      // Include for std::map  for dictionary  data structure  
#include <sstream>  // Include for std::istringstream  for string stream  input and output 
#include <utility>  // Include for std::pair  for key value pair        
#include <emscripten.h>
#include <emscripten/bind.h>

// the following function will compare two strings
//the  isValid() function takes two string parameters: PropertyValueSet and PropertyFilter
EMSCRIPTEN_KEEPALIVE    
int isValid(std::string PropertyValueSet, std::string PropertyFilter){
//const v1 = Module.isValid("a=1;b=2", "a=1,2&b=3"); // false
//const v2 = Module.isValid("a=1;b=3", "a=1,2&b=3"); // true
//isValid("a=1;b=2", "b=9"); // false
//isValid("a=1;b=3", "a=1"); // true
//isValid("a=1;b=2", "b=2"); // true

//brek down string, build a dictionary propertyMap
    std::map<std::string, std::string> propertyMap;

    std::stringstream iss(PropertyValueSet); 
    std::string token; 
    while (std::getline(iss, token, ';')) 
    { 
        size_t pos = token.find("=");
        if (pos != std::string::npos) 
        { 
            std::string key = token.substr(0, pos); 
            std::string value = token.substr(pos + 1); 
            propertyMap[key] = value; 
        }
    }

    for ( const auto &pair : propertyMap )
    {
       // std::cout << "key: " << pair.first << " value: " << pair.second << std::endl;
    }

// parese proporty filter 

std::stringstream filterStream(PropertyFilter);
std::string condition; 
bool overallResult = true; 

    while(std:: getline(filterStream, condition, '&'))
    { 
        bool conditionResult = false; 
        std::stringstream conditionStream(condition); 
        std::string subCondition;

            while (std::getline(conditionStream, subCondition, ','))
            { 
                size_t pos = subCondition.find("=");
                if(pos != std::string::npos)
                {
                    std::string key = subCondition.substr(0, pos); // key is a 
                    std::string filterValue = subCondition.substr(pos + 1); // value is 1,2
                    if(propertyMap.find(key) != propertyMap.end() && propertyMap[key] == filterValue)
                    {
                        conditionResult = true; 
                        break;
                    }                   
                }
            }
            if (!conditionResult)
            {
                overallResult = false; 
                break;
            }
    }
    //std::cout << "overallResult is: " << overallResult << std::endl;
    return overallResult; // ? 1 : 0;
} ;   

// Binding code 
EMSCRIPTEN_BINDINGS(Module)
{ 
    emscripten::function("isValid", &isValid);
}



