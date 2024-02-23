#include "structint.h"

void structint_dealloc(structint_t *self) {
  dealloc_uint64list(self->value);
  Py_TYPE(self)->tp_free((PyObject*)self);
  return;
}

PyObject *structint_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  structint_t *self;
  self = (structint_t*)type->tp_alloc(type, 0);
  if (self == NULL) {
    return PyErr_NoMemory();
  }

  self->value = NULL;
  self->byte_sz = 0LL;
  self->used_value_parts = 0LL;
  self->bit_len = 0LL;
  self->sign_mask = 0LL;
  self->flags = 0;

  self->asymmetric = 0;
  self->carry = 0;
  self->overflow = 0;
  self->null = 0;

  return (PyObject*)self;
}

int structint_init(structint_t *self, PyObject *args, PyObject *kwds) {
  static char *kwlist[] = {"value", "len", "flags", NULL};
  PyObject *arg_obj = Py_None;
  size_t arg_bit_len = -1;
  uint32_t arg_flags = -1;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OKI", kwlist, 
      &arg_obj, &arg_bit_len, &arg_flags)) {
    PyErr_BadArgument();
    return -1;
  }

  if (structint_type_check(arg_obj)) {
    structint_t *src = (structint_t*)arg_obj;
    size_t res_sz, dst_sz = get_uint64list_bytesz_from_bitlen(arg_bit_len);
    uint64_t *value = copy_uint64list(NULL, dst_sz, src->value, src->byte_sz, &res_sz);
    if (value == NULL) {
      PyErr_NoMemory();
      return -1;
    }

    if (structint_unsafe_copy(self, src) == NULL) {
      return -1;
    }

    if (structint_safe_set_all(self, value, res_sz, arg_bit_len, arg_flags) == NULL) {
      return -1;
    }
  }
  else {
    self->bit_len = (arg_bit_len == -1) ? 0 : arg_bit_len;
    self->flags = (arg_flags == -1) ? 0 : arg_flags;
    if (structint_convert_obj_and_selfstore(self, arg_obj) == NULL) {
      return -1;
    }
  }

  return 0;
}


PyMODINIT_FUNC PyInit_structint(void) {
  PyObject *m;
  if (PyType_Ready(&structint_Type) < 0) {
    return NULL;
  }

  m = PyModule_Create(&module_structint);
  if (m == NULL) {
    return NULL;
  }

  Py_INCREF(&structint_Type);
  int m_err = 0;
  m_err |= PyModule_AddObject(m, "structint", (PyObject*)&structint_Type);
  
  m_err |= PyModule_AddIntConstant(m, "UNSIGNED", STRUCTINT_FLAGS_UNSIGNED);
  m_err |= PyModule_AddIntConstant(m, "ASYMMETRIC_LEN", STRUCTINT_FLAGS_ASYMMETRIC_LEN);
  m_err |= PyModule_AddIntConstant(m, "LITTLE_ENDIAN", STRUCTINT_FLAGS_LITTLE_ENDIAN);
  m_err |= PyModule_AddIntConstant(m, "CARRY_EXCEPTION", STRUCTINT_FLAGS_CARRY_EXCEPTION);
  m_err |= PyModule_AddIntConstant(m, "OVERFLOW_SATURATION", STRUCTINT_FLAGS_OVERFLOW_SATURATION);
  m_err |= PyModule_AddIntConstant(m, "OVERFLOW_EXPAND", STRUCTINT_FLAGS_OVERFLOW_EXPAND);
  m_err |= PyModule_AddIntConstant(m, "OVERFLOW_EXCEPTION", STRUCTINT_FLAGS_OVERFLOW_EXCEPTION);
  m_err |= PyModule_AddIntConstant(m, "NULL_IS_NOT_ZERO", STRUCTINT_FLAGS_NULL_IS_NOT_ZERO);

  structintExc_AsymmetricError = PyErr_NewException("structint.AsymmetricError", NULL, NULL);
  m_err |= PyModule_AddObject(m, "AsymmetricError", structintExc_AsymmetricError);
  
  structintExc_CarryError = PyErr_NewException("structint.CarryError", NULL, NULL);
  m_err |= PyModule_AddObject(m, "CarryError", structintExc_CarryError);
  
  structintExc_NullError = PyErr_NewException("structint.NullError", NULL, NULL);
  m_err |= PyModule_AddObject(m, "NullError", structintExc_NullError);

  if (m_err == -1) {
    Py_DECREF(&structint_Type);
    Py_DECREF(m);
  } 

  return m;
}
