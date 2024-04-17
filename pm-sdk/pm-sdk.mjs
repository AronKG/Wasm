import { PropertyValueSet, PropertyFilter } from "@promaster-sdk/property";

const pvs1valid = PropertyFilter.isValid(PropertyValueSet.fromString("a=1;b=3"), PropertyFilter.fromString("a=1,2&b=3")); // false

console.log("pvs1valid", pvs1valid);
