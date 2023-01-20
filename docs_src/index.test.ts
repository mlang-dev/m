import {add} from '../docs_src/index';

describe('testing index file', () => {
    test('empty string should result in zero', () => {
      expect(add('')).toBe(0);
    });
  });