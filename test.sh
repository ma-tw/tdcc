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
# return
assert 0 'return 0;'
assert 12 'a = 12; return a; return 0;'
assert 3 'a = 0; return a = 3;'
assert 2 'x = 1; returnx = 2; return returnx;'
# if
assert 2 'x = 0; if (1) x = 2; return x; return 99;'
assert 3 'if (0) return 2; else return 3; return 99;'
assert 3 'if (1) if (0) return 2; else return 3; return 99;'
assert 99 'if (0) if (0) return 2; else return 3; return 99;'
assert 4 'if (1) if (0) if (0) return 2; else return 3; else return 4; return 99;'
assert 99 'if (0) if (0) if (0) return 2; else return 3; else return 4; return 99;'
assert 22 'age = 17; limit = 0; if (age < 16) limit = 18; else if (age < 18) limit = 22; else limit = 24; return limit;'
# while
assert 4 'x = 10; while (x >= 5) x = x - 1; return x;'
assert 3 'x = 9; while (x >= 5) x = x - 2; return x;'

# do-while
assert 5 'x = 1; do x = x + 2; while (x <= 4); return x;'
assert 10 'x = 1; do x = 10; while (0); return x;'

# for
assert 15 'x = 0; i = 10; for (i = 1; i <= 5; i = i + 1) x = x + i; return x;'
assert 14 'x = 0; i = 2; for (; i <= 5; i = i + 1) x = x + i; return x;'
assert 50 'x = -50; i = 1000; for (i = 0; ; i = i + 1) if (i == 100) return x + i; return 99;'
assert 0 'i = 1; for (i = 0; i < 1; ) return 0; return 1;'

# block
assert 0 '{ return 0; }'
assert 5 '{{{ return 5; }}}'
assert 1 '{ 0; 0; 0; 0; 0; return 1; }'
assert 10 'x = 0; i = 4; while (i >= 0) { x = x + i; i = i - 1; } return x;'

echo OK
