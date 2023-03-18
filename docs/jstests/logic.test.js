"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * logic operators
 */
const mtest_1 = require("./mtest");
(0, mtest_1.mtest)('true', 'true is evaluated as integer 1.', "true", 1);
(0, mtest_1.mtest)('false', 'false is evaluated as integer 0.', "false", 0);
(0, mtest_1.mtest)('true and true', 'true && true should be true.', "true && true", 1);
(0, mtest_1.mtest)('true and false', 'true && false should be false.', "true && false", 0);
(0, mtest_1.mtest)('false and true', 'false && true should be false.', "false && true", 0, false);
(0, mtest_1.mtest)('false and false', 'false && false should be false.', "false && false", 0);
(0, mtest_1.mtest)('false or false', 'false || false should be false.', "false || false", 0);
(0, mtest_1.mtest)('true or false', 'true || false should be true.', "true || false", 1);
(0, mtest_1.mtest)('false or true', 'false || true should be true.', "true || false", 1, false);
(0, mtest_1.mtest)('true or true', 'true || true should be true.', "true || true", 1, false);
(0, mtest_1.mtest)('not true', 'not true should be false.', "!true", 0);
(0, mtest_1.mtest)('not false', 'not false is true.', "!false", 1);
//# sourceMappingURL=logic.test.js.map