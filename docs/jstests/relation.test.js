"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * relational operators
 */
const mtest_1 = require("./mtest");
(0, mtest_1.mtest)('less than', '10 < 20 is evaluated as true (1).', "10 < 20", 1);
(0, mtest_1.mtest)('less than float', '10.0 < 20.0 is evaluated as true (1).', "10.0 < 20.0", 1);
(0, mtest_1.mtest)('less and equal than float', '10.0 <= 20.0 is evaluated as true (1).', "10.0 <= 20.0", 1);
(0, mtest_1.mtest)('less and equal than int', '10 <= 20 is evaluated as true (1).', "10 <= 20", 1);
(0, mtest_1.mtest)('less and equal than int equal', '20 <= 20 is evaluated as true (1).', "20 <= 20", 1);
(0, mtest_1.mtest)('less and equal than int false', '21 <= 20 is evaluated as false (0).', "21 <= 20", 0);
(0, mtest_1.mtest)('greater and equal than int', '21 >= 20 is evaluated as true (1).', "21 >= 20", 1);
(0, mtest_1.mtest)('greater than int', '21 > 20 is evaluated as true (1).', "21 > 20", 1);
(0, mtest_1.mtest)('equal int false', '21 == 20 is evaluated as false (0).', "21 == 20", 0);
(0, mtest_1.mtest)('equal int', '20 == 20 is evaluated as true (1).', "20 == 20", 1);
(0, mtest_1.mtest)('not equal int', '21 != 20 is evaluated as true (1).', "21 != 20", 1);
(0, mtest_1.mtest)('not equal int false', '20 != 20 is evaluated as false (0).', "20 != 20", 0);
//# sourceMappingURL=relation.test.js.map