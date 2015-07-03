#!/usr/bin/env python3

import sys
import math
import random

def sign():
    return random.choice(('', '+', '-'))

def zeros(mean):
    n = random.expovariate(1.0 / float(mean))
    return '0' * int(n)

def significant_digis(mean):
    n = random.lognormvariate(math.log(mean), math.log(mean) / 2)
    res = []
    for i in range(int(n)):
        res.append(random.choice('0123456789'))
    return ''.join(res)

def decimal_mark(digits):
    n = len(digits)
    if random.random() > 0.25:
        u = random.triangular(0, n + 1)
        i = int(u)
        return ''.join((digits[:i], '.', digits[i:]))
    else:
        return digits

def exponent():
    if random.random() > 0.25:
        return ''.join((random.choice('eE'),
                        sign(),
                        zeros(1),
                        str(int(random.uniform(0, 250)))))
    else:
        return ''

def number():
    while True:
        digits = ''.join((zeros(1.5),
                          significant_digis(5),
                          zeros(2)))
        digits = decimal_mark(digits)
        if digits:
            return ''.join((sign(), digits, exponent()))

def main(args):
    for i in range(int(args[0])):

        print(number())

if __name__  == '__main__':
    main(sys.argv[1:])
