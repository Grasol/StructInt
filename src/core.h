/*
 * This file is part of StructInt.
 *
 * StructInt is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * StructInt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with StructInt.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once 
#include <Python.h>
#include "structmember.h"

#include <stdbool.h>
#include <stdint.h>


typedef struct {
  PyObject_HEAD

  uint64_t *value;
  size_t byte_sz;
  size_t used_value_parts;
  size_t bit_len;
  uint64_t sign_mask;

  uint32_t flags;
  /*  31                          8 7       0
   *  ---- ---- ---- ---- ---- ---N -vvC L-AU
   * 0     U -> UNSIGNED
   * 1     A -> ASYMMETRIC_LEN
   * 3     L -> LITTLE_ENDIAN
   * 4     C -> CARRY_EXCEPTION
   * 6-5  vv -> overflow field:
   *      00 .. do nothing
   *      01 .. OVERFLOW_SATURATION
   *      10 .. OVERFLOW_EXPAND
   *      11 .. OVERFLOW_EXCEPTION
   * 8     N -> NULL_IS_NOT_ZERO  
   */
  #define STRUCTINT_FLAGS_UNSIGNED              0x0001
  #define STRUCTINT_FLAGS_ASYMMETRIC_LEN        0x0002
  #define STRUCTINT_FLAGS_TEST                  0x0004
  #define STRUCTINT_FLAGS_LITTLE_ENDIAN         0x0008
  #define STRUCTINT_FLAGS_CARRY_EXCEPTION       0x0010
  #define STRUCTINT_FLAGS_OVERFLOW_FIELD        0x0060
  #define STRUCTINT_FLAGS_OVERFLOW_SATURATION   0x0020
  #define STRUCTINT_FLAGS_OVERFLOW_EXPAND       0x0040
  #define STRUCTINT_FLAGS_OVERFLOW_EXCEPTION    0x0060
  #define STRUCTINT_FLAGS_NULL_IS_NOT_ZERO      0x0100

  char asymmetric;
  char carry;
  char overflow;
  char null;
} structint_t;

PyObject *structintExc_AsymmetricError;
PyObject *structintExc_CarryError;
PyObject *structintExc_NullError;
#define INTERNAL_LIB_ERROR_STR "internal structint error"
#define TYPE_OBJ_ERROR_STR "first argument must be a int, a bytes, a bytearray, a bool, a none or another structint object"
#define ASYMMETRIC_LEN_ERROR_FMT "right side has bit lentgh %z but left side requires %z"

uint64_t *alloc_uint64list(uint64_t *value, size_t new_sz, size_t old_sz, size_t *res_sz);
void dealloc_uint64list(uint64_t *value);
/* 
 * dst_value and dst_sz are optional. Default values are NULL and 0. 
 * This function doesn't lose the origin of dst_value
 */
uint64_t *copy_uint64list(uint64_t *dst_value, size_t dst_sz, uint64_t *src_value, size_t src_sz, size_t *res_sz);
size_t round_size(size_t value, size_t base);
#define get_uint64list_idx_by_bit(bit) ((round_size(bit, 64LL) / 64) - 1)
#define get_uint64list_bytesz_from_bitlen(bit) (round_size(bit, 8LL))

PyObject *structint_new(PyTypeObject *type, PyObject *args, PyObject *kwds);
int structint_init(structint_t *self, PyObject *args, PyObject *kwds);
void structint_dealloc(structint_t *self);
/* 
 * In structint_t_safe_set_all() all parameters are optional
 *  - defualt pointers are NULL
 *  - defualt values are -1
 *
 * Warning 1: 
 *  - If 'new_value' isn't null, 'new_byte_sz' must be given
 * Warning 2: 
 *  - The origin of 'new_value' should be from the same structint object which it was originally taken.
 *    alloc_uint64list() doesn't lose the origin
 */
structint_t *structint_safe_set_all(structint_t *self, uint64_t *new_value, size_t new_byte_sz, size_t new_bit_len, uint32_t new_flags);
structint_t *structint_set_null_value(structint_t *self);
structint_t *structint_sign_smear(structint_t *self);
/*
 * unsafe_copy() doesn't modify value and byte_sz
 */
structint_t *structint_unsafe_copy(structint_t *self, structint_t *src);
/*
 * structint_t_convert_obj_and_selfstore() require additional parameters stored in 
 * structint object: 
 *  - value (with keep the origin. If value haven't the origin, it can be NULL)
 *  - byte_sz (can be 0)
 *  - bit_len (prioritize bit length of value)
 *  - flags (destination flags in object)
 * These parameters can be transferred unsafe (without using safe_set_all...)
 */
structint_t *structint_convert_obj_and_selfstore(structint_t *self, PyObject *src);
PyObject *structint_print_value(structint_t *self, PyObject *Py_UNUSED(ignored));

size_t structint_asymmetric_len_check(structint_t *a, structint_t *b);
#define structint_type_check(obj) (PyObject_TypeCheck(arg_obj, &structint_Type))
structint_t *structint_overflow(structint_t *self);

typedef enum  {
  TypeError,
  Long,
  StructInt,
  ByteArray,
  Bytes,
  Bool,
  None
} structint_obj_t;

structint_obj_t check_valueobj_type(PyObject *src);
Py_buffer *convert_pybyteslike_to_pybuffer(Py_buffer *buf, PyObject *src);

/* 
 * convert to uint64list functions: 
 * do not check flags and compatibilities with other structint parameters, 
 * bit_len precision is rounded to 64bit part
 */
int convert_pybool_to_uint64list(uint64_t *value, size_t bit_len, PyObject *src);
int convert_pybuffer_to_uint64list(uint64_t *value, size_t bit_len, Py_buffer *src);
int convert_pylong_to_uint64list(uint64_t *value, size_t bit_len, PyObject *src);

size_t get_bitlen_pybuffer(Py_buffer *src);
size_t get_bitlen_pylong(PyObject *src);
#define get_signbit_mask(bit) (1LL << ((bit - 1) & 0x3fLL))
#define get_bit_partmask(bit_mask) (bit_mask | (bit_mask - 1))
#define get_true_value(first_len, second_len) ((first_len == 0LL) ? second_len : first_len)

