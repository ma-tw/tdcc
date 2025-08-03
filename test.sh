#!/bin/bash

assert() {
    expected="$1"
    input="$2"

    ./tdcc "$input" > tmp.s
    cc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3 + 5) / 2;'
assert 10 '-10+20;'
assert 0 '15+ +(-3*+5);'
assert 1 '-(+(-(+1)));'
assert 1 '123 == 100 + 20 + 3;'
assert 0 '42 == 123;'
assert 0 '-123 == -123 == -123;'
assert 1 '10 + 5 > 10 + 4;'
assert 1 '1 < 2 > 0;'
assert 1 '1 >= 1 != 1 > 1;'
assert 2 '(12 > 10) + (12 <= 10) + (0 > -10);'
# vars
assert 3 'a = 3; a;'
assert 20 'a = 2; b = a + 8; 2 * b;'
assert 2 'a = 0; a = a + 1; a = a + 1;'
assert 8 'a = b = c = 2; d = (a + b) * c;'

echo OK
