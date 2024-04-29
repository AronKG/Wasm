import { Bench } from 'tinybench';
import { PropertyValueSet, PropertyFilter } from "@promaster-sdk/property";
import PfWasm from './pf-wasm.js';
import NoopWasm from './noop.js';

// Call web assembly version of isVvalid()
function wasmIsValid() {
    const result =  PfWasm.isValid("a=1;b=3", "a=12&b=3");
    // console.log("wasmIsValid", result);
}

function wasmNoop() {
    
    const result =  NoopWasm.noop(1);
    // console.log("wasmIsValid", result);
}

// function wasmNoop() {
//     const array = [1, 2, 3, 4, 5]; // Example array of integers
//     const result = NoopWasm.noop(array); // Pass the array to the noop function
//     // console.log("wasmNoop", result);
// }

// Call pure javascript version of isVvalid()
function jsIsValid() {
    const result = PropertyFilter.isValid(PropertyValueSet.fromString("a=1;b=3"), PropertyFilter.fromString("a=12&b=3")); // false
    // console.log("jsIsValid", result);
}


// Create a Bench instance
const bench = new Bench({ time: 1000 });
bench.add('wasmIsValid', wasmIsValid);
bench.add('wasmNoop', wasmNoop);
bench.add('jsIsValid', jsIsValid);

//run beanches concarrently 
await bench.run();

// Output the merged table
console.table(bench.table());
