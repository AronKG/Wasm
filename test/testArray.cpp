// #include <emscripten/bind.h>
// #include <emscripten/val.h>
// #include <iostream>
// #include <vector>

// void passTwoIntArrays(const emscripten::val &intArray1Object, const emscripten::val &intArray2Object) { 
//     unsigned int length1 = intArray1Object["length"].as<unsigned int>(); // Get the length of the first array
//     unsigned int length2 = intArray2Object["length"].as<unsigned int>();  

//     std::vector<int> intArray1(length1); // Create a vector to store the first array
//     std::vector<int> intArray2(length2);

//     auto memory = emscripten::val::module_property("HEAPU8")["buffer"]; // Get the memory buffer

//     auto memoryView1 = intArray1Object["constructor"].new_(memory, reinterpret_cast<uintptr_t>(intArray1.data()), length1);  // Create a memory view for the first array
//     memoryView1.call<void>("set", intArray1Object); // Copy the first array to the memory view

//     auto memoryView2 = intArray2Object["constructor"].new_(memory, reinterpret_cast<uintptr_t>(intArray2.data()), length2);
//     memoryView2.call<void>("set", intArray2Object);

//     // Assuming both arrays are of the same length
//     unsigned int length = std::min(length1, length2); // Get the minimum length of the two arrays

//     // Multiply corresponding elements from both arrays
//     for (unsigned int i = 0; i < length; ++i) { 
//         intArray1[i] *= intArray2[i];
//     }

//     // Print the modified array
//     for (unsigned int i = 0; i < length; ++i) {
//         std::cout << intArray1[i] << ", ";
//     }
//     std::cout << std::endl;
// }

// int noop(int a) {
//     return a + 1;
// }

// EMSCRIPTEN_BINDINGS(my_module) {
//    emscripten::function("noop", &noop);
//    emscripten::function("passTwoIntArrays", &passTwoIntArrays);
// }





#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <vector>

using namespace emscripten;

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int float_multiply_array(float factor, float *arr, int length) { // Multiply each element of the array by a factor
        for (int i = 0; i < length; i++) {
            arr[i] = factor * arr[i]; // Multiply each element of the array by the factor
            // Print the factor
            printf("Factor: %f\n", factor);
        }
        return 0;
    }
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("float_multiply_array", &float_multiply_array, allow_raw_pointer<float>());
}

