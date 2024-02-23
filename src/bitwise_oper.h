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

