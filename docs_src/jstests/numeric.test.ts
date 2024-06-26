/**
 * numeric operators
 */
import { mtest } from './mtest';

mtest('add expression', 'add opeartor: 10 + 20', "10 + 20", 30);

mtest('operator precendence', 'operator precendence: 10 + 20 * 3', "10 + 20 * 3", 70);

mtest('negative opearator', 'negative number: -10 + 20 * 3', "-10 + 20 * 3", 50);

mtest('float expression', 'float expression: -10.0 + 20.0 * 3.0', "-10.0 + 20.0 * 3.0", 50.0);

mtest('exponentiation operator', 'exponentiation operator: 2.0 ** 3.0', "2.0 ** 3.0", 8.0);

mtest('modulo operator zero', 'modulo operator: 8 % 2', "8 % 2", 0);
mtest('modulo operator', 'modulo operator: 8 % 5', "8 % 5", 3);

