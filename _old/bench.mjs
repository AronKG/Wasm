import Wasm from './property-filter-wasm.js';
import { Bench } from 'tinybench';

// Function to benchmark isValid function from Wasm file for each test case
function benchmarkIsValid1() {
    const result1 =  Wasm.isValid("pp=4", "GGG=3 | pp=4");
    console.log("result1 =", result1 === true ? "true" : "false");
}

function benchmarkIsValid2() {
    const result2 =  Wasm.isValid("pp=4; GGG= 34", "GGG=3 & pp=4");
    console.log("result2 =", result2 === true ? "true" : "false");
    return result2 === 1; //false
}

function benchmarkIsValid3() {
    const result3 =  Wasm.isValid("a=1;b=2", "a=1,&b=3");
    console.log("result3 =", result3 === true ? "true" : "false");
    return result3 === 1; //false
}

function benchmarkIsValid4() {
    const result4 =  Wasm.isValid("a=1;b=3", "a=1, &b=3");
    console.log("result4 =", result4 === true ? "true" : "false");
    return result4 === 1;   //true
}

// Create a Bench instance
const bench1 = new Bench({ time: 50 });
bench1.add('isValid1 benchmarkIsValid1', benchmarkIsValid1);
const bench2 = new Bench({ time: 50 });
bench2.add('isValid2 benchmarkIsValid2', benchmarkIsValid2);
const bench3 = new Bench({ time: 50 });
bench3.add('isValid3 benchmarkIsValid3', benchmarkIsValid3);
const bench4 = new Bench({ time: 50 });
bench4.add('isValid4 benchmarkIsValid4', benchmarkIsValid4);

//run beanches concarrently 
await Promise.all([bench1.run(), bench2.run(), bench3.run(), bench4.run()]);

// Merge the tables from all benchmarks
const mergedTable = [...bench1.table(), ...bench2.table(), ...bench3.table(), ...bench4.table()];

// Output the merged table
console.table(mergedTable);
