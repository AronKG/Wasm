import Module from './example.js';
import { Bench } from 'tinybench';

function benchmarkStringlen() {
    const result = Module.stringlen('Aron', 'Aron');
    if (result === 1) {
        console.log('true');
    } else {
        console.log('false');
    }
}

function benchmarkMultiplyIntegers() {
    const result = Module.multiply_integers(9, 2);
    console.log(result);
}

const bench1 = new Bench({ time: 2 });
bench1.add('stringlen Benchmark', benchmarkStringlen);
const bench2 = new Bench({ time: 2 });
bench2.add('multiply_integers Benchmark', benchmarkMultiplyIntegers);

// Run both benchmarks concurrently
await Promise.all([bench1.run(), bench2.run()]);

// Merge the tables from both benchmarks
const mergedTable = [...bench1.table(), ...bench2.table()];

// Output the merged table
console.table(mergedTable);
