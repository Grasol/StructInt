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

#include "core.h"

uint64_t *alloc_uint64list(uint64_t *value, size_t new_sz, size_t old_sz, size_t *res_sz) {
  if (new_sz <= old_sz) {
    if (res_sz != NULL) {
      *res_sz = old_sz;
    }

    return value;
  }

  size_t alloc_sz = round_size(new_sz, 8LL);  
  uint64_t *new_value;
  if (value == NULL) {
    new_value = PyMem_RawMalloc(alloc_sz);
  }
  else {
    new_value = PyMem_RawRealloc(value, alloc_sz);
  }

  if (res_sz != NULL) {
    *res_sz = alloc_sz;
  }

  return new_value;
}

void dealloc_uint64list(uint64_t *value) {
  if (value != NULL) {
    PyMem_RawFree(value);
  }

  return;
}

uint64_t *copy_uint64list(uint64_t *dst_value, size_t dst_sz, uint64_t *src_value, size_t src_sz, size_t *res_sz) {
  size_t alloc_sz = get_true_value(dst_sz, src_sz);
  if (!res_sz) {
    return NULL;
  }

  uint64_t *res_value = alloc_uint64list(dst_value, alloc_sz, 0LL, NULL);
  if (res_value == NULL) {
    return NULL;
  } 

  for (size_t i = 0LL; (8 * i) < alloc_sz; ++i) {
    if (8 * i < src_sz) {
      res_value[i] = src_value[i];
    }
    else {
      res_value[i] = 0LL;
    }
  }

  if (res_sz != NULL) {
    *res_sz = alloc_sz;
  }

  return res_value;
}


size_t round_size(size_t value, size_t base) {
  if (value == 0) {
    return base;
  }

  size_t mask = base - 1;
  size_t res = value & ~mask;
  if (value & mask) {
    res += base;
  }

  return res;
}


structint_t *structint_safe_set_all(structint_t *self, uint64_t *new_value, size_t new_byte_sz, size_t new_bit_len, uint32_t new_flags) {
  if (self == NULL) {
    PyErr_SetString(PyExc_SystemError, INTERNAL_LIB_ERROR_STR);
    return NULL;
  } 

  // set len
  if (new_bit_len != -1) {
    self->bit_len = new_bit_len;
  }

  // set flags
  if (new_flags != -1) {
    self->flags = new_flags;

    self->asymmetric = 0;
    self->carry = 0;
    self->overflow = 0;
  }

  // set uint64list
  if (new_value != NULL) {
    self->value = new_value;
    self->byte_sz = new_byte_sz;
  }

  self->used_value_parts = get_uint64list_idx_by_bit(self->bit_len) + 1;
  if (self->bit_len == 0) {
    structint_set_null_value(self);
  }
  else {
    self->null = 0;
    self->sign_mask = get_signbit_mask(self->bit_len);
    structint_sign_smear(self);
  }

  return self;
}


structint_t *structint_set_null_value(structint_t *self) {
  if (self == NULL) {
    PyErr_SetString(PyExc_SystemError, INTERNAL_LIB_ERROR_STR);
    return NULL;
  }

  self->null = 1;
  for (size_t i = 0; i < self->used_value_parts; ++i) {
    self->value[i] = 0LL;
  }

  self->sign_mask = 0LL;
  return self;
}

structint_t *structint_unsafe_copy(structint_t *self, structint_t *src) {
  if (self == NULL) {
    PyErr_SetString(PyExc_SystemError, INTERNAL_LIB_ERROR_STR);
    return NULL;
  }

  self->bit_len = src->bit_len;
  self->sign_mask = src->sign_mask;
  self->flags = src->flags;
  self->asymmetric = src->asymmetric;  
  self->carry = src->carry;
  self->overflow = src->overflow;
  self->null = src->null;  

  return self;
}

structint_t *structint_sign_smear(structint_t *self) {
  if (self == NULL) {
    PyErr_SetString(PyExc_SystemError, INTERNAL_LIB_ERROR_STR);
    return NULL;
  }

  if (!self->sign_mask) {
    return self;
  }

  bool sign;
  size_t last_part_idx = self->used_value_parts - 1;
  if (self->flags & STRUCTINT_FLAGS_UNSIGNED) {
    sign = false;
  }
  else {
    sign = (bool)(self->value[last_part_idx] & self->sign_mask);
  }

  uint64_t part_mask = get_bit_partmask(self->sign_mask);
  if (sign) {
    self->value[last_part_idx] |= ~part_mask;
  }
  else {
    self->value[last_part_idx] &= part_mask;
  }  

  return self;
}

structint_t *structint_convert_obj_and_selfstore(structint_t *self, PyObject *src) {
  if (self == NULL) {
    PyErr_SetString(PyExc_SystemError, INTERNAL_LIB_ERROR_STR);
    return NULL;
  }

  structint_obj_t src_type = check_valueobj_type(src);

  size_t src_bit_len = 0, true_bit_len = 0, value_byte_sz = 0;
  uint64_t *value = NULL;
  bool null = false;
  switch (src_type) {
    case Long: {
      src_bit_len = get_bitlen_pylong(src);
      true_bit_len = get_true_value(self->bit_len, src_bit_len);
      value_byte_sz = get_uint64list_bytesz_from_bitlen(true_bit_len);

      value = alloc_uint64list(self->value, value_byte_sz, self->byte_sz, &value_byte_sz);
      if (value == NULL) {
        return (structint_t*)PyErr_NoMemory();
      }

      if (convert_pylong_to_uint64list(value, true_bit_len, src)) {
        PyErr_SetString(PyExc_TypeError, INTERNAL_LIB_ERROR_STR);
        return NULL;
      }

      break;
    }
    case ByteArray: case Bytes: {
      Py_buffer buf;
      if (convert_pybyteslike_to_pybuffer(&buf, src) == NULL) {
        PyBuffer_Release(&buf);
        PyErr_BadArgument();
        return NULL;
      }

      src_bit_len = get_bitlen_pybuffer(&buf);
      true_bit_len = get_true_value(self->bit_len, src_bit_len);
      value_byte_sz = get_uint64list_bytesz_from_bitlen(true_bit_len);

      value = alloc_uint64list(self->value, value_byte_sz, self->byte_sz, &value_byte_sz);
      if (value == NULL) {
        PyBuffer_Release(&buf);
        return (structint_t*)PyErr_NoMemory();
      }

      convert_pybuffer_to_uint64list(value, true_bit_len, &buf);

      PyBuffer_Release(&buf);
      break;
    }
    case Bool: {
      true_bit_len = get_true_value(self->bit_len, 1LL);
      value_byte_sz = get_uint64list_bytesz_from_bitlen(true_bit_len);

      value = alloc_uint64list(self->value, value_byte_sz, self->byte_sz, &value_byte_sz);
      if (value == NULL) {
        return (structint_t*)PyErr_NoMemory();
      }

      if (convert_pybool_to_uint64list(value, true_bit_len, src)) {
        PyErr_SetString(PyExc_TypeError, INTERNAL_LIB_ERROR_STR);
        return NULL;
      }

      break;
    }
    case None: {
      null = true;

      true_bit_len = get_true_value(self->bit_len, 0LL);
      value_byte_sz = get_uint64list_bytesz_from_bitlen(true_bit_len);
      value = alloc_uint64list(self->value, value_byte_sz, self->byte_sz, &value_byte_sz);
      if (value == NULL) {
        return (structint_t*)PyErr_NoMemory();
      }

      break;
    }
    case TypeError: {
      PyErr_SetString(PyExc_TypeError, TYPE_OBJ_ERROR_STR);
      return NULL;
    }
  }

  if ((true_bit_len == 0) || null) {
    self->value = value;
    self->byte_sz = value_byte_sz;
    self->used_value_parts = get_uint64list_idx_by_bit(true_bit_len) + 1;
    self->bit_len = true_bit_len;
    structint_set_null_value(self);
  }
  else {
    structint_safe_set_all(self, value, value_byte_sz, true_bit_len, self->flags);
  }

  return self;
}

PyObject *structint_print_value(structint_t *self, PyObject *Py_UNUSED(ignored)) {
  for (size_t i = 0; i < self->used_value_parts; ++i) {
    printf("%.16"PRIx64"\n", self->value[i]);
  }

  Py_INCREF(Py_None);
  return Py_None;
}

size_t structint_asymmetric_len_check(structint_t *a, structint_t *b) {
  if (a->flags & STRUCTINT_FLAGS_ASYMMETRIC_LEN) {
    return 0;
  }

  size_t left_len = a->bit_len;
  size_t right_len = b->bit_len;
  if (left_len != right_len) {
    PyErr_Format(structintExc_AsymmetricError, ASYMMETRIC_LEN_ERROR_FMT, right_len, left_len);
    return -1;
  }

  return 0;
}

structint_t *structint_overflow(structint_t *self) {

}


structint_obj_t check_valueobj_type(PyObject *src) {
  if (PyLong_CheckExact(src)) {
    return Long;
  }
  else if (PyByteArray_CheckExact(src)) {
    return ByteArray;
  }
  else if (PyBytes_CheckExact(src)) {
    return Bytes;
  }
  else if (PyBool_Check(src)) {
    return Bool;
  }
  else if (src == Py_None) {
    return None;
  }

  return TypeError;
}

int convert_pybool_to_uint64list(uint64_t *value, size_t bit_len, PyObject *src) {
  uint64_t v;
  if (src == Py_True) {
    v = 1LL;
  }
  else if (src == Py_False) {
    v = 0LL;
  }
  else {
    return -1;
  }

  for (size_t i = 0LL; (i * 64) < bit_len; ++i) {
    value[i] = v;
    v = 0LL;
  }

  return 0;
}

Py_buffer *convert_pybyteslike_to_pybuffer(Py_buffer *buf, PyObject *src) {
  if (!PyArg_Parse(src, "y*", buf)) {
    return NULL;
  }

  return buf;
}

int convert_pybuffer_to_uint64list(uint64_t *value, size_t bit_len, Py_buffer *src) {
  size_t byte_sz = src->len;
  union V {
    uint64_t u64;
    uint8_t u8[8];
  } v;

  size_t value_i = 0LL;
  for (size_t i = 0LL; (i * 8) < bit_len; ++i) {
    size_t u8_i = i % 8; 
    if (i < byte_sz) {
      v.u8[u8_i] = ((uint8_t*)src->buf)[byte_sz - i - 1];
    }
    else {
      v.u8[u8_i] = 0;
    }

    if (u8_i == 7) {
      value[value_i] = v.u64;
      v.u64 = 0LL;
      ++value_i;
    }
  }

  value[value_i] = v.u64;
  return 0;
}

int convert_pylong_to_uint64list(uint64_t *value, size_t bit_len, PyObject *src) {
  uint64_t mask = 0xffffffffffffffffLL;
  PyObject *pylong_mask = PyLong_FromUnsignedLongLong(mask);

  if (bit_len <= 64) {
    PyObject *pylong_value = PyNumber_And(src, pylong_mask);        
    if (pylong_value == NULL) {
      return -1;
    }

    value[0] = PyLong_AsUnsignedLongLong(pylong_value);

    Py_XDECREF(pylong_value);
    Py_XDECREF(pylong_mask);
    return 0;
  }

  for (size_t i = 0LL; (i * 64) < bit_len; ++i) {
    uint64_t shift = i * 64;
    PyObject *pylong_shift = PyLong_FromUnsignedLongLong(shift);
    PyObject *pylong_shifted_src = PyNumber_Rshift(src, pylong_shift);
    PyObject *pylong_value_part = PyNumber_And(pylong_shifted_src, pylong_mask);
    if (pylong_value_part == NULL) {
      return -1;
    }

    value[i] = PyLong_AsUnsignedLongLong(pylong_value_part);

    Py_XDECREF(pylong_shift);
    Py_XDECREF(pylong_shifted_src);
    Py_XDECREF(pylong_value_part);
  }

  Py_XDECREF(pylong_mask);
  return 0;
}

size_t get_bitlen_pybuffer(Py_buffer *src) {
  if (src->ndim != 1) {
    return 0LL;
  }

  if (src->itemsize != 1LL) {
    return 0LL;
  }

  return src->len * 8;
}



size_t get_bitlen_pylong(PyObject *src) {
  PyObject *len_obj = PyObject_CallMethod(src, "bit_length", NULL);
  if (len_obj == NULL) {
    return 0LL;
  }

  size_t bit_len = PyLong_AsSize_t(len_obj);
  Py_XDECREF(len_obj);
  if (bit_len == -1LL) {
    return 0LL;
  }

  return ++bit_len;
}

