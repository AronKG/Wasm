//is valid function will call here 
//const isValid = require('./isValide'); 


//the testing examples are here
  
//const v1 = Module.isValid("a=1;b=2", "a=1,2&b=3"); // false
//const v2 = Module.isValid("a=1;b=3", "a=1,2&b=3"); // true
//isValid("a=1;b=2", "b=9"); // false
//isValid("a=1;b=3", "a=1"); // true
//isValid("a=1;b=2", "b=2"); // true


const Module = require('./compare_strings.js');

const filter1="a=1,2&b=3"

const v1= Module.isValid("a=1;b=2", filter1); // false
if (v1 === 1) {
    console.log('true');
} else {
    console.log('false');
}


const v2 = Module.isValid("a=1;b=3", filter1); // true
if (v2 === 1) {
    console.log('true');
}
else {
    console.log('false');
}

const v3 = Module.isValid("a=1;b=2", "b=9"); // false
if (v3 === 1) {
    console.log('true');
}
else {
    console.log('false');
}

const v4 =  Module.isValid("a=1;b=3", "a=1"); // true
if (v4 === 1) {
    console.log('true');
}
else {  
    console.log('false');
}

const v5 = Module.isValid("a=1;b=2", "b=2"); // true
if (v5 === 1) {
    console.log('true');
}
else {
    console.log('false');
}

