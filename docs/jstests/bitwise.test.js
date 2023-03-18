"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * bitwise operators
 */
const mtest_1 = require("./mtest");
(0, mtest_1.mtest)('bitwise not zero', 'bitwise not 0', "~0", -1);
(0, mtest_1.mtest)('bitwise not neg one', 'bitwise not -1', "~-1", 0);
(0, mtest_1.mtest)('bitwise not one', 'bitwise not 1', "~1", -2);
(0, mtest_1.mtest)('bitwise shift left', 'bitwise shift left', "1 << 1", 2);
(0, mtest_1.mtest)('bitwise shift right', 'bitwise shift right', "2 >> 1", 1);
(0, mtest_1.mtest)('bitwise and', 'bitwise and', "2 & 1", 0);
(0, mtest_1.mtest)('bitwise or', 'bitwise or', "2 | 1", 3);
(0, mtest_1.mtest)('bitwise xor', 'bitwise xor', "8 ^ 15", 7);
//# sourceMappingURL=bitwise.test.js.map