var Ftdi = require('./');

console.log("init: " + Ftdi.init());
console.log("findAll: ");
console.log(Ftdi.findAll());
console.log("open: " + Ftdi.open());
console.log("getPins: " + Ftdi.getPins());
console.log("setPins: " + Ftdi.setPins(7));
console.log("getPins: " + Ftdi.getPins());
console.log("close: " + Ftdi.close());
