#include <string>
#include <iostream> // Include for cout and endl
#include <emscripten.h>
#include <emscripten/bind.h>

// Function to return the length of a string
size_t stringlen(const std::string& p, const std::string& q)
{
    if (p.compare(q) == 0) {
        return 1; // Strings are equal
    } else {
        return 0; // Strings are not equal
    }
}


// Function to add two integers
int add_integers(int a, int b)
{

    return a + b;
}

// Function to multiply two integers
int multiply_integers(int a, int b)
{
   int sum = add_integers(a, b);
    return sum * b;
}

// Binding code   
EMSCRIPTEN_BINDINGS(Module)
{ 
    // Bind the stringlen function to the name 'stringlen'
    emscripten::function("stringlen", &stringlen);

    // Bind the add_integers function to the name 'add_integers'
    //emscripten::function("add_integers", &add_integers);

    // Bind the multiply_integers function to the name 'multiply_integers'
    emscripten::function("multiply_integers", &multiply_integers);
}

