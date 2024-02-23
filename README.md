# StructInt

Python C lib to simplifying operations on fixed bit length integers.

e.g.
```structint(len=123, flags=UNSIGNED)``` 
creates 123 bit length unsigned integer
```
    \/ priority number
fundamentals:
    0   a = structint(value=, len=, flags=)
    0   a.set(value=, len=, flags=)
    0   a.get_flags(unsigned=, saturation=, high_bit_order=, little_endian=, 
          carry_exception=, overflow_exception=, expand=, expand_exception=, null_is_zero=)
    1   a.merge(b, ..., tflags=)
    1   a = merge(b, ..., flags=)
    0   y = a.to_bytes(value=, len=, tflags=)
    0   i = a.to_int(value=, len=, tflags=)
    2   f = a.to_float(float_fmt, value=, tflags=)
    2   a.from_float(float_fmt, value, flags=)
    2   a.convert_float(float_fmt_d, float_fmt_s, flags=)
    1   s = a.to_string(base=, value=, len=, tflags=)
    1   a = from_string(value, base=, len=, flags=)

additions:
    0   a = b + c
    0   a += b
    0   a.add(b, tflags=)
    0   a = add(b, ..., len=, flags=)

subtractions:
    0   a = b - c
    0   a -= b
    0   a.sub(b, tflags=)
    0   a = sub(b, ..., len=, flags=)

multiplications:
    4   a = b * c
    4   a *= b
    4   a.mul(b, tflags=)
    7   a.clmul(b, tflags=)
    4   (a, b) = mulhl(c, d, len=, flags=)

divisions:
    4   a = b / c
    4   a /= b
    4   a = b % c
    4   a %= b
    4   a.div(b, tflags=)
    4   a.rem(b, tflags=)
    4   a.mod(b, tflags=)
    4   (a, r) = divrem(b, c, len=, flags=)
    4   (a, r) = divmod(b, c, len=, flags=)

simple bits operations:
    0   i = a[bit]
    0   a[bit] = b
    0   a = -a
    0   a = ~a
    0   a = b & c
    0   a &= b
    0   a = b | c
    0   a |= b
    0   a = b ^ c
    0   a ^= b

shifts:
    3   a = b >> c
    3   a >>= b
    3   a = b << c
    3   a <<= b
    3   a.shlr(b, tflags=)
    3   a.shar(b, tflags=)
    3   a.shll(b, tflags=)
    3   a.rotr(b, tflags=)
    3   a.rotl(b, tflags=)
    3   a.rotcr(b, tflags=)
    3   a.rotcl(b, tflags=)

bits fields:
    0   a.bitextend(bit_pos, value=, extend_len=, tflags=)
    1   a = b[X:Y:a_len]
    1   a[X:Y:b_len] = b
    1   a = b[Y:X:a_len]
    1   a[Y:X:b_len] = b
    1   a = fill(value, dst_len=, v_len=, flags=)
    5   a.bitzhi(b)
    5   a.bitformat(bit_fmt)
    6   a.bitscatter(b)
    6   a.bitgather(b)
    6   a.bitreverse()

bit comparisons:
    0   i = a ? b
    0   i = a.any()
    0   i = a.all()
    5   i = a.popcount()
    5   i = a.tzcount()
    5   i = a.t1count()
    5   i = a.lzcount()
    5   i = a.l1count()
    6   i = a.parity()
    6   i = a.crc(b)


NEW CONTROL FLAGS:
31                          8 7       0
---- ---- ---- ---- ---- ---N -vvC L-AU
    U -> UNSIGNED
    A -> ASYMMETRIC_LEN
    L -> LITTLE_ENDIAN
    C -> CARRY_EXCEPTION
   vv -> overflow field:
   00 .. do nothing
   01 .. OVERFLOW_SATURATION
   10 .. OVERFLOW_EXPAND
   11 .. OVERFLOW_EXCEPTION
    N -> NULL_IS_NOT_ZERO  

STATUS FLAGS:
asymmetric, carry, overflow, null

EXCEPTIONS:
    MemoryError
    TypeError
    ValueError
    IndexError
    structint.AsymmetricError
    structint.CarryError
    structint.NullError
    OverflowError
    ZeroDivisionError


a = structint.structint(int(input()), 4)
print(a.to_string(2))

b = structint.from_string(input(), 4, )
```