import { Bench } from 'tinybench';
import { PropertyValueSet, PropertyFilter } from "@promaster-sdk/property";
import PfWasm from './pf-wasm.js';

// Call web assembly version of isVvalid()
function wasmIsValid() {
    const result =  PfWasm.isValid("a=1;b=3", "a=12&b=3");
    // console.log("wasmIsValid", result);
}

function wasmIsValidUncached() {
    const result =  PfWasm.isValidUncached("a=1;b=3", "a=12&b=3");
    // console.log("wasmIsValid", result);
}

// Call pure javascript version of isVvalid()
function jsIsValid() {
    const result = PropertyFilter.isValid(PropertyValueSet.fromString("a=1;b=3"), PropertyFilter.fromString("a=12&b=3")); // false
    // console.log("jsIsValid", result);
}


// Create a Bench instance
const bench = new Bench({ time: 50 });
bench.add('wasmIsValid', wasmIsValid);
bench.add('wasmIsValidUncached', wasmIsValidUncached);
bench.add('jsIsValid', jsIsValid);

//run beanches concarrently 
await bench.run();

// Output the merged table
console.table(bench.table());
