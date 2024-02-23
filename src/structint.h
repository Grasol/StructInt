#pragma once
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"

#include "core.h"

#include <stdbool.h>
#include <stdint.h>

#define STRUCTINT_DOCSTR "structint(value=None, len=0, flags=0)\n\nfixed length flexible int with multiple function like carry service, unsigned or saturation arithmetic"


static PyMemberDef structint_members[] = {
  {"len", T_ULONGLONG, offsetof(structint_t, bit_len), READONLY},
  {"flags", T_UINT, offsetof(structint_t, flags), READONLY},
  {"asymmetric", T_BOOL, offsetof(structint_t, asymmetric), READONLY},
  {"carry", T_BOOL, offsetof(structint_t, carry), 0},
  {"overflow", T_BOOL, offsetof(structint_t, overflow), READONLY},
  {"null", T_BOOL, offsetof(structint_t, null), READONLY},
  {"test_byte_sz", T_ULONGLONG, offsetof(structint_t, byte_sz), READONLY},
  {"test_used_value_parts", T_ULONGLONG, offsetof(structint_t, used_value_parts), READONLY},
  {"test_sign_mask", T_ULONGLONG, offsetof(structint_t, sign_mask), READONLY},
  {NULL}
};

static PyMethodDef structint_methods[] = {
  {"print_value", (PyCFunction)structint_print_value, METH_NOARGS},
  {NULL}
};

static PyTypeObject structint_Type = {
  PyVarObject_HEAD_INIT(NULL, 0)
  .tp_name = "structint.structint",
  .tp_doc = PyDoc_STR(STRUCTINT_DOCSTR),
  .tp_basicsize = sizeof(structint_t),
  .tp_itemsize = 0,
  .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
  .tp_new = structint_new,
  .tp_init = (initproc)structint_init,
  .tp_dealloc = (destructor)structint_dealloc,
  .tp_members = structint_members,
  .tp_methods = structint_methods,
};

static struct PyModuleDef module_structint = {
  PyModuleDef_HEAD_INIT,
  .m_name = "structint",
  .m_doc = STRUCTINT_DOCSTR,
  .m_size = -1,
};


PyMODINIT_FUNC PyInit_structint(void);