"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const mtest_1 = require("./mtest");
(0, mtest_1.mtest_string)('analyzer type mismatch', 'analyzer type mismatch', `
let i:int = "string"
`, "variable type not matched with literal constant. loc (line, col): (2, 5)\n");
//# sourceMappingURL=error-analyzer.test.js.map