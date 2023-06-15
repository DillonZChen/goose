#include <Python.h>

#include "bliss-0.73/digraph_wrapper.h"

using namespace std;

static void _destroy(void *g)
{
  if(g)
  {
    delete (DigraphWrapper *)g;
  }
}

static PyObject *
create(PyObject *self, PyObject *args)
{
  DigraphWrapper *g = new DigraphWrapper();
  if(!g)
    Py_RETURN_NONE;

  PyObject *py_g = PyCObject_FromVoidPtr(g, &_destroy);
  if(!py_g)
    Py_RETURN_NONE;
  return py_g;
}


static PyObject *
add_vertex(PyObject *self, PyObject *args)
{
  PyObject *py_g = NULL;
  unsigned int color;

  // "OI": O for object, I for int
  if(!PyArg_ParseTuple(args, "OI", &py_g, &color))
    Py_RETURN_NONE;
  if(!PyCObject_Check(py_g))
    Py_RETURN_NONE;

  DigraphWrapper *g = (DigraphWrapper *)PyCObject_AsVoidPtr(py_g);
  assert(g);

  g->add_vertex(color);
  Py_RETURN_NONE;
}


static PyObject *
add_edge(PyObject *self, PyObject *args)
{
  PyObject *py_g = NULL;
  unsigned int v1;
  unsigned int v2;

  if(!PyArg_ParseTuple(args, "OII", &py_g, &v1, &v2))
    Py_RETURN_NONE;
  if(!PyCObject_Check(py_g))
    Py_RETURN_NONE;

  DigraphWrapper* g = (DigraphWrapper *)PyCObject_AsVoidPtr(py_g);
  assert(g);

  g->add_edge(v1, v2);
  Py_RETURN_NONE;
}


static PyObject *
find_automorphisms(PyObject *self, PyObject *args)
{
  PyObject *py_g = NULL;

  if(!PyArg_ParseTuple(args, "O", &py_g))
    Py_RETURN_NONE;
  if(!PyCObject_Check(py_g))
    Py_RETURN_NONE;

  DigraphWrapper *g = (DigraphWrapper *)PyCObject_AsVoidPtr(py_g);
  assert(g);

  // TODO: add support for time_limit parameter for find_automorphisms

  vector<vector<int> > automorphisms = g->find_automorphisms();

  // Map the automorphisms to a python list
  PyObject* py_outer = PyList_New(0);
  if(!py_outer)
    Py_RETURN_NONE;

  for (size_t aut_index = 0; aut_index < automorphisms.size(); ++aut_index)
  {
    const vector<int> &automorphism = automorphisms[aut_index];
    PyObject* py_inner = PyList_New(0);
    if(!py_inner)
      Py_RETURN_NONE;
    for(size_t from = 0; from < automorphism.size(); ++from)
    {
      if (PyList_Append(py_inner, PyInt_FromLong((long)automorphism[from])) != 0)
        Py_RETURN_NONE;
    }
    if (PyList_Append(py_outer, py_inner) != 0)
      Py_RETURN_NONE;
  }

  return py_outer;
}


static PyMethodDef Methods[] = {
    {"create", create, METH_VARARGS, ""},
    {"add_vertex", add_vertex, METH_VARARGS, ""},
    {"add_edge", add_edge, METH_VARARGS, ""},
    {"find_automorphisms",  find_automorphisms, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};


PyMODINIT_FUNC
initpyext_blissmodule(void)
{
  (void)Py_InitModule("pyext_blissmodule", Methods);
}

