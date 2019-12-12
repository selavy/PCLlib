#!/usr/bin/env python

if __name__ == '__main__':
    base1 = 'vector'
    base2 = 'table'
    for c in range(ord('a'), ord('z') + 1):
        cc = chr(c)
        print(f'{cc}{base1}_t')
        print(f'{cc}{base2}_t')
        print(f'{cc}vector_init()')
        print(f'{cc}table_init()')
        print()

