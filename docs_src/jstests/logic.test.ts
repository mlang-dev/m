/**
 * logic operators
 */
import { mtest } from './mtest';

mtest('true', 'true is evaluated as integer 1.', "true", 1);

mtest('false', 'false is evaluated as integer 0.', "false", 0);

mtest('true and true', 'true && true should be true.', "true and true", 1);

mtest('true and false', 'true && false should be false.', "true and false", 0);

mtest('false and true', 'false && true should be false.', "false and true", 0, false);

mtest('false and false', 'false && false should be false.', "false and false", 0);

mtest('false or false', 'false || false should be false.', "false or false", 0);

mtest('true or false', 'true || false should be true.', "true or false", 1);

mtest('false or true', 'false || true should be true.', "true or false", 1, false);

mtest('true or true', 'true || true should be true.', "true or true", 1, false);

mtest('not true', 'not true should be false.', "not true", 0);

mtest('not false', 'not false is true.', "not false", 1);
