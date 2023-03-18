"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const mtest_1 = require("./mtest");
(0, mtest_1.mtest_string)('parse char missing end quote', 'parse char missing end quote', `
c = '
`, "missing end quote for char literal. location (line, col): (2, 5)\n");
(0, mtest_1.mtest_string)('parse char too long', 'parse char too long', `
c = 'ab'
`, "character literal is found to have more than 1 character. location (line, col): (2, 5)\n");
(0, mtest_1.mtest_string)('parse string missing end quote', 'parse string missing end quote', `
c = "abc
`, "missing end quote for string literal. location (line, col): (2, 5)\n");
//# sourceMappingURL=error-lexer.test.js.map