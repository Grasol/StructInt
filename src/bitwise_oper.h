#pragma once



PyObject *structint_oper_negative(PyObject *self);
PyObject *structint_oper_positive(PyObject *self);
PyObject *structint_oper_absolute(PyObject *self);
PyObject *structint_oper_invert(PyObject *self;

PyObject *structint_oper_and(PyObject *self, PyObject *b);
PyObject *structint_oper_iand(PyObject *self, PyObject *b);
PyObject *structint_oper_xor(PyObject *self, PyObject *b);
PyObject *structint_oper_ixor(PyObject *self, PyObject *b);
PyObject *structint_oper_or(PyObject *self, PyObject *b);
PyObject *structint_oper_ior(PyObject *self, PyObject *b);

