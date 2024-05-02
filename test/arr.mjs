// // Import the generated Emscripten module
// import MyModule from './my_array.js';


// // Function to call the WebAssembly function 'noop'
// function wasmNoop() {
//     try {
//         // Call the exported WebAssembly function 'noop' with an argument
//         const result = MyModule.noop(1);
//         console.log("WebAssembly function called successfully.");
//         console.log("Result:", result);
//     } catch (error) {
//         console.error("An error occurred while calling the WebAssembly function:", error);
//     }
// }

// // Call the 'wasmNoop' function
// wasmNoop();


// // Function to call the WebAssembly function 'passTwoIntArrays' with two integer arrays
// function wasmPassTwoIntArrays(jsIntArray1, jsIntArray2) {
//     try {
//         // Convert JavaScript arrays to typed arrays
//         const typedIntArray1 = new Int32Array(jsIntArray1);
//         const typedIntArray2 = new Int32Array(jsIntArray2);

//         // Call the exported WebAssembly function 'passTwoIntArrays' with the typed arrays
//         MyModule.passTwoIntArrays(typedIntArray1, typedIntArray2);

//         console.log("WebAssembly function 'passTwoIntArrays' called successfully.");
//     } catch (error) {
//         console.error("An error occurred while calling the WebAssembly function 'passTwoIntArrays':", error);
//     }
// }

// // Define two JavaScript arrays containing integers
// const jsIntArray1 = [1, 2, 3, 4, 5];
// const jsIntArray2 = [2, 3, 4, 5, 6];

// // Call the 'wasmPassTwoIntArrays' function with the JavaScript integer arrays
// wasmPassTwoIntArrays(jsIntArray1, jsIntArray2);


import Module from './my_array.js';


// Import function from Emscripten generated file
var float_multiply_array = Module.cwrap(
    'float_multiply_array', 'number', ['number', 'number', 'number']
);


// Create example data to test float_multiply_array
var data = new Float32Array([1, 2, 3, 4, 5]);

// Get data byte size, allocate memory on Emscripten heap, and get pointer
var nDataBytes = data.length * data.BYTES_PER_ELEMENT;
var dataPtr = Module._malloc(nDataBytes);

// Copy data to Emscripten heap (directly accessed from Module.HEAPU8)
var dataHeap = new Uint8Array(Module.HEAPU8.buffer, dataPtr, nDataBytes);
dataHeap.set(new Uint8Array(data.buffer));

// Call function and get result
float_multiply_array(2, dataHeap.byteOffset, data.length);
var result = new Float32Array(dataHeap.buffer, dataHeap.byteOffset, data.length);
// Print result
console.log(result);

// Free memory
Module._free(dataHeap.byteOffset);
