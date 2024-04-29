#include <iostream>
#include <cstring>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <vector>
#include <string>

//bool noop(std::string propertyValueSet, std::string propertyFilter) {
//bool noop(std::string propertyValueSet) {
//bool noop(int* array, int length) {
 bool noop(int a) {
    return true;
}

//Binding code
EMSCRIPTEN_BINDINGS(Module) {
    // Binding functions with allow_raw_pointer for raw pointers
    emscripten::function("noop", &noop);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

// write emscript code that takes in integer array and print them out 

// void noop(int* array, int length) {
//     for (int i = 0; i < length; i++) {
//         std::cout << array[i] << std::endl;
//     }
// }

// EMSCRIPTEN_BINDINGS(Module) {
//     emscripten::function("noop", emscripten::select_overload<void(int*, int)>(&noop), emscripten::allow_raw_pointer<0>());
// }
