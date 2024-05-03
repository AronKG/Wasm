import { Bench } from 'tinybench';
import { PropertyValueSet, PropertyFilter } from "@promaster-sdk/property";
import PfWasm from './pf-wasm.js';
import NoopWasm from './noop.js';

let dataPtr = null; // Global variable to store the pointer to the allocated memory
let dataHeap = null; // Global variable to store the Uint8Array representing the allocated memory

// Import function from Emscripten generated file
const float_multiply_array = NoopWasm.cwrap(
    'float_multiply_array', 'number', ['number', 'number', 'number']
);

// Function to allocate memory and fill it with data
function allocateMemory(data) {
    if (!dataPtr) {
        // Get data byte size, allocate memory on Emscripten heap, and get pointer
        const nDataBytes = data.length * data.BYTES_PER_ELEMENT;
        dataPtr = NoopWasm._malloc(nDataBytes);
         dataHeap = new Uint8Array(NoopWasm.HEAPU8.buffer, dataPtr, nDataBytes);// Copy data to Emscripten heap (directly accessed from Module.HEAPU8)

    }
}

// Function to perform multiplication using WebAssembly
function pointer_to_array() {
    const data = new Uint8Array([1, 2, 3, 4, 5]);
    
    allocateMemory(data); // Ensure memory is allocated

  // Get data byte size, allocate memory on Emscripten heap, and get pointer
  const nDataBytes = data.length * data.BYTES_PER_ELEMENT; 
  //dataHeap.set(new Uint8Array(data.buffer));

    // Call function and get result
    float_multiply_array(2, dataHeap.byteOffset, 5); // Assuming 5 elements in the array
   // const result = new Uint8Array(dataHeap.buffer, dataHeap.byteOffset, 5); // Assuming 5 elements in the array

    return 0;
}

// Call web assembly version of isVvalid()
function wasmIsValid() {
    const result = PfWasm.isValid("a=1;b=3", "a=12&b=3");
    // console.log("wasmIsValid", result);
}

// Call web assembly version of wasmPassTwoIntArrays from noop.js
function wasmPassTwoIntArrays() {
    const jsIntArray1 = [1, 2, 3, 4, 5];
    const jsIntArray2 = [2, 3, 4, 5, 6];
    const typedIntArray1 = new Int32Array(jsIntArray1);
    const typedIntArray2 = new Int32Array(jsIntArray2);
    NoopWasm.passTwoIntArrays(typedIntArray1, typedIntArray2);
    // console.log("wasmPassTwoIntArrays", result);
}

// Call pure JavaScript version of isVvalid()
function jsIsValid() {
    const result = PropertyFilter.isValid(PropertyValueSet.fromString("a=1;b=3"), PropertyFilter.fromString("a=12&b=3")); // false
    // console.log("jsIsValid", result);
}

// Create a Bench instance
const bench = new Bench({ time: 1000 });
bench.add('wasmIsValid', wasmIsValid);
bench.add('jsIsValid', jsIsValid);
bench.add('wasmPassTwoIntArrays', wasmPassTwoIntArrays);
bench.add('pointer_to_array', pointer_to_array);

// Run benches concurrently 
await bench.run();

// Output the merged table
console.table(bench.table());

// Free the memory at the end
if (dataPtr) {
    NoopWasm._free(dataPtr);
}
