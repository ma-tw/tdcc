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
assert 3 'foo = 3; foo;'
assert 20 'hoge = 2; fuga = hoge + 8; 2 * fuga;'
assert 2 'num = 0; num = num + 1; num = num + 1;'
assert 8 'a_ = _b = _0 = 2; d0 = (a_ + _b) * _0;'
assert 1 'sum = 1 + 2 + 3 + 4 + 5; fm = (1 + 5) * 5 / 2; sum == fm;'
assert 1 'a = 5; b = a + 1; c = a - 1; gtres = b > a; ltres = c < a; gtres * ltres;'

echo OK
