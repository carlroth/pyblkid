/*
 * python-blkid
 * ---------------
 * A Python extension for blkid
 *
 * Copyright (c) 2009 Kel Modderman <kel@otaku42.de>
 * License: GPL v2
 *
 */

#include <Python.h>
#include <structmember.h>
#include <osdefs.h>
#include <stdio.h>

#include <fcntl.h>
#include <sys/ioctl.h>

#include <blkid/blkid.h>

#define BLKGETSIZE64 _IOR(0x12,114,size_t)

#define UNUSED __attribute__ (( __unused__ ))

typedef struct {
	PyObject_HEAD
	blkid_probe pr;
	PyObject* file;
} BlkId;

static int       BlkId_init(BlkId*, PyObject*, PyObject*);
static PyObject* BlkId_probe_results(BlkId*);
static PyObject* BlkId_lookup_value(BlkId*, PyObject*, PyObject*);
static void      BlkId_dealloc(BlkId*);
static PyObject* BlkId_error;

static char* BLKID_DOC_STRING =
	"blkid defines a single class, BlkId, that must be instantiated\n"
	"with the pathname of a block device.\n"
	"\n"
	"Once a BlkId object has been instantiated, it may call several\n"
	"helper methods to query volume properties of the block device.\n"
	"If an error occurs, a blkid.error exception is raised.\n"
;

static int BlkId_init(BlkId* self, PyObject* args, PyObject* kwargs)
{
	int fd = -1;
	int ret = 0;

    static char *kwlist[] = {"file", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &self->file))
        return NULL;

    if (self->file && PyFile_CheckExact(self->file)) {    	
    	FILE *fp = PyFile_AsFile(self->file);   
    	//PyFile_IncUseCount(self->file);

		fd = fileno(fp);

		if (fd < 0) {
			PyErr_SetString(BlkId_error, "open() failed");
			return -1;
		}

		self->pr = blkid_new_probe();

		if (!self->pr) {
			PyErr_SetString(BlkId_error, "blkid_new_probe() failed");
			return -1;
		}
		/*
		blkid_probe_set_request(self->pr,
					BLKID_PROBREQ_LABEL | BLKID_PROBREQ_UUID |
					BLKID_PROBREQ_TYPE | BLKID_PROBREQ_SECTYPE |
					BLKID_PROBREQ_USAGE | BLKID_PROBREQ_VERSION);
		if (ioctl(self->fd, BLKGETSIZE64, &size) != 0)
			size = 0;
		
		*/
		if (blkid_probe_set_device(self->pr, fd, 0, 0) != 0) {
			PyErr_SetString(BlkId_error, "blkid_probe_set_device() failed");
			return -1;
		}
	} else {		
		PyErr_SetString(BlkId_error, "init failed");
		return -1;
	}
	/*
	blkid_probe_enable_partitions(self->pr, 1);
	blkid_probe_enable_superblocks(self->pr, 1);
	//blkid_probe_enable_topology(self->pr, 1);

	ret = blkid_do_safeprobe(self->pr);

	if (ret == -1) {
		PyErr_SetString(BlkId_error, "blkid_do_safeprobe() failed");
		return -1;
	}
	*/

	/*
	if (ret == 1) {
		PyErr_SetString(BlkId_error, "blkid_do_safeprobe() failed. nothing detected");
		return -1;
	}
	if (ret == -2) {
		PyErr_SetString(BlkId_error, "blkid_do_safeprobe() failed. ambivalen result is detected");
		return -1;
	}
	*/

	return 0;
}

static PyObject* BlkId_do_probe(BlkId* self)
{
    int ret;

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }

	ret = blkid_do_probe(self->pr);

	if (ret == -1) {
		PyErr_SetString(BlkId_error, "blkid_do_probe() failed");
		return NULL;
	}

	Py_RETURN_NONE;

}

static PyObject* BlkId_do_safeprobe(BlkId* self)
{
    int ret;

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }
	
	ret = blkid_do_safeprobe(self->pr);

	if (ret == -1) {
		PyErr_SetString(BlkId_error, "blkid_do_safeprobe() failed");
		return NULL;
	}

	Py_RETURN_NONE;

}


static PyObject* BlkId_do_fullprobe(BlkId* self)
{
    int ret;

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }
	
	ret = blkid_do_fullprobe(self->pr);

	if (ret == -1) {
		PyErr_SetString(BlkId_error, "blkid_do_safeprobe() failed");
		return NULL;
	}

	Py_RETURN_NONE;

}

static PyObject* BlkId_do_wipe(BlkId* self, PyObject* args, PyObject* kwargs)
{
    int ret;
    PyObject* dryrun = NULL;

    static char *kwlist[] = {"dryrun", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|O", kwlist, &dryrun))
        return NULL;

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }

    if(!dryrun) {
		PyErr_SetString(BlkId_error, "dryrun must provided");
		return NULL;
    }

    if (PyObject_IsTrue(dryrun)) {    	
		ret = blkid_do_wipe(self->pr, 1);

		if (ret == -1) {
			PyErr_SetString(BlkId_error, "blkid_do_wipe() failed");
			return NULL;
		}
    }
    else {
		ret = blkid_do_wipe(self->pr, 0);

		if (ret == -1) {
			PyErr_SetString(BlkId_error, "blkid_do_wipe() failed");
			return NULL;
		}

    }

	Py_RETURN_NONE;

}

static PyObject* BlkId_enable_superblocks(BlkId* self)
{
    int ret;

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }

	ret = blkid_probe_enable_superblocks(self->pr, 1);

	if (ret == -1) {
		PyErr_SetString(BlkId_error, "blkid_probe_enable_superblocks() failed");
		return NULL;
	}

	Py_RETURN_NONE;

}

static PyObject* BlkId_disable_superblocks(BlkId* self)
{
    int ret;

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }

	ret = blkid_probe_enable_superblocks(self->pr, 0);

	if (ret == -1) {
		PyErr_SetString(BlkId_error, "blkid_probe_enable_superblocks() failed");
		return NULL;
	}

	Py_RETURN_NONE;

}
static PyObject* BlkId_set_superblocks_flags(BlkId* self, PyObject* args, PyObject* kwargs)
{
    int ret;
    int flags = -1;

    static char *kwlist[] = {"flags", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "I", kwlist, &flags))
    {
		PyErr_SetString(BlkId_error, "invalid handle");
        return NULL;
    }

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }

	if(flags == -1) {
		PyErr_SetString(BlkId_error, "invalied flags for BlkId_set_partitions_flags()");
		return NULL;
	}

	ret = blkid_probe_set_superblocks_flags(self->pr, flags);

	if (ret == -1) {
		PyErr_SetString(BlkId_error, "blkid_do_safeprobe() failed");
		return NULL;
	}

	Py_RETURN_NONE;

}



static PyObject* BlkId_set_partitions_flags(BlkId* self, PyObject* args, PyObject* kwargs)
{
    int ret;
    int flags = -1;

    static char *kwlist[] = {"flags", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "I", kwlist, &flags))
        return NULL;

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }
	
	if(flags == -1) {
		PyErr_SetString(BlkId_error, "invalied flags for BlkId_set_partitions_flags()");
		return NULL;
	}

	ret = blkid_probe_set_partitions_flags(self->pr, flags);

	if (ret == -1) {
		PyErr_SetString(BlkId_error, "blkid_do_safeprobe() failed");
		return NULL;
	}

	Py_RETURN_NONE;

}

static PyObject* BlkId_enable_partitions(BlkId* self)
{
    int ret;

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }

	ret = blkid_probe_enable_partitions(self->pr, 1);

	if (ret == -1) {
		PyErr_SetString(BlkId_error, "blkid_probe_enable_partitions() failed");
		return NULL;
	}

	Py_RETURN_NONE;

}

static PyObject* BlkId_disable_partitions(BlkId* self)
{
    int ret;

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }

	ret = blkid_probe_enable_partitions(self->pr, 0);

	if (ret == -1) {
		PyErr_SetString(BlkId_error, "blkid_probe_enable_partitions() failed");
		return NULL;
	}

	Py_RETURN_NONE;

}

static PyObject* BlkId_reset_probe(BlkId* self)
{
    int ret;

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }

	blkid_reset_probe(self->pr);

	Py_RETURN_NONE;

}

static PyObject* BlkId_probe_results(BlkId* self)
{
	const char *data, *name;
	int nvals = 0, n;
	blkid_loff_t size;
	PyObject *probe_results = PyDict_New();

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }

	nvals = blkid_probe_numof_values(self->pr);

	for (n = 0; n < nvals; n++) {
		if (blkid_probe_get_value(self->pr, n, &name, &data, NULL))
			continue;

		PyDict_SetItem(probe_results,
			       PyString_FromString(name),
			       PyString_FromString(data));
	}

	size = blkid_probe_get_size(self->pr);

	PyDict_SetItem(probe_results,
		       PyString_FromString("SIZE"),
		       PyLong_FromLongLong(size));

	return probe_results;
}

static PyObject* BlkId_lookup_value(BlkId* self, PyObject* args, UNUSED PyObject* kargs)
{
	const char *keyword, *value;

    if(!self->pr) {
		PyErr_SetString(BlkId_error, "invalid handle");
		return NULL;
    }

	if (!PyArg_ParseTuple(args, "s", &keyword)) {
		PyErr_SetString(BlkId_error, "failed to parse query string");
		return NULL;
	}

	if (blkid_probe_lookup_value(self->pr, keyword, &value, NULL) == 0)
		return Py_BuildValue("s", value);

	Py_RETURN_NONE;
}


static PyObject* BlkId_close(BlkId* self)
{
	if(self->pr) {
		fsync(blkid_probe_get_fd(self->pr));
		blkid_free_probe(self->pr);
		//PyFile_DecUseCount(self->file);
		self->pr = NULL;
	}

	Py_RETURN_NONE;

}

static void BlkId_dealloc(BlkId* self)
{
	if (self->pr) {
		fsync(blkid_probe_get_fd(self->pr));
		blkid_free_probe(self->pr);
		//PyFile_DecUseCount(self->file);
		self->pr = NULL;
	}
	/*
	if (self->ob_type)
		self->ob_type->tp_free((PyObject*)(self));
	e*/
}

static PyMethodDef BlkId_methods[] = {
	{
		"do_probe", (PyCFunction)(BlkId_do_probe), METH_NOARGS,
		"Return a dict of probe data."
	},
	{
		"do_safeprobe", (PyCFunction)(BlkId_do_safeprobe), METH_NOARGS,
		"Return a dict of probe data."
	},
	{
		"do_fullprobe", (PyCFunction)(BlkId_do_fullprobe), METH_NOARGS,
		"Return a dict of probe data."
	},
	{
		"probe_results", (PyCFunction)(BlkId_probe_results), METH_NOARGS,
		"Return a dict of probe data."
	},
	{
		"reset_probe", (PyCFunction)(BlkId_reset_probe), METH_NOARGS,
		"Return value for a specific keyword from probe data."
	},
	{
		"disable_partitions", (PyCFunction)(BlkId_disable_partitions), METH_NOARGS,
		"Return value for a specific keyword from probe data."
	},
	{
		"enable_partitions", (PyCFunction)(BlkId_enable_partitions), METH_NOARGS,
		"Return value for a specific keyword from probe data."
	},
	{
		"set_partitions_flags", (PyCFunction)(BlkId_set_partitions_flags), METH_VARARGS,
		"Return value for a specific keyword from probe data."
	},
	{
		"set_superblocks_flags", (PyCFunction)(BlkId_set_superblocks_flags), METH_VARARGS,
		"Return value for a specific keyword from probe data."
	},
	{
		"disable_superblocks", (PyCFunction)(BlkId_disable_superblocks), METH_NOARGS,
		"Return value for a specific keyword from probe data."
	},
	{
		"enable_superblocks", (PyCFunction)(BlkId_enable_superblocks), METH_NOARGS,
		"Return value for a specific keyword from probe data."
	},
	{
		"do_wipe", (PyCFunction)(BlkId_do_wipe), METH_VARARGS,
		"Return value for a specific keyword from probe data."
	},
	{
		"close", (PyCFunction)(BlkId_close), METH_NOARGS,
		"Return value for a specific keyword from probe data."
	},
	{ NULL, NULL, 0, NULL }
};

static PyTypeObject PyType_BlkId = {
	PyObject_HEAD_INIT(NULL)
	0,                                              /* ob_size */
	"_blkid.BlkId",                                  /* tp_name */
	sizeof(BlkId),                                  /* tp_basicsize */
	0,                                              /* tp_itemsize */
	(destructor)(BlkId_dealloc),                    /* tp_dealloc */
	0,                                              /* tp_print */
	0,                                              /* tp_getattr */
	0,                                              /* tp_setattr */
	0,                                              /* tp_compare */
	0,                                              /* tp_repr */
	0,                                              /* tp_as_number */
	0,                                              /* tp_as_sequence */
	0,                                              /* tp_as_mapping */
	0,                                              /* tp_hash */
	0,                                              /* tp_call */
	0,                                              /* tp_str */
	PyObject_GenericGetAttr,                        /* tp_getattro */
	PyObject_GenericSetAttr,                        /* tp_setattro */
	0,                                              /* tp_as_buffer */
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,       /* tp_flags */
	0,                                              /* tp_doc */
	0,                                              /* tp_traverse */
	0,                                              /* tp_clear */
	0,                                              /* tp_richcompare */
	0,                                              /* tp_weaklistoffset */
	0,                                              /* tp_iter */
	0,                                              /* tp_iternext */
	BlkId_methods,                                  /* tp_methods */
	0,                                              /* tp_members */
	0,                                              /* tp_getset */
	0,                                              /* tp_base */
	0,                                              /* tp_dict */
	0,                                              /* tp_descr_get */
	0,                                              /* tp_descr_set */
	0,                                              /* tp_dictoffset */
	(initproc)(BlkId_init),                         /* tp_init */
	0,                                              /* tp_alloc */
	PyType_GenericNew,                              /* tp_new */
	0,                                              /* tp_free */
	0,                                              /* tp_is_gc */
	0,                                              /* tp_bases */
	0,                                              /* tp_mro */
	0,                                              /* tp_cache */
	0,                                              /* tp_subclasses */
	0,                                              /* tp_weaklist */
	0                                               /* tp_del */
};

PyMODINIT_FUNC init_blkid(void)
{
	PyObject* module;

	PyType_Ready(&PyType_BlkId);

	module = Py_InitModule3("_blkid", NULL, BLKID_DOC_STRING);
	BlkId_error = PyErr_NewException("_blkid.error", NULL, NULL);

    PyModule_AddIntConstant(module, "PARTS_FORCE_GPT", BLKID_PARTS_FORCE_GPT);
    PyModule_AddIntConstant(module, "PARTS_ENTRY_DETAILS", BLKID_PARTS_ENTRY_DETAILS);
#if 0
    /* buildroot v2.20.0 does not include this */
    PyModule_AddIntConstant(module, "PARTS_MAGIC", BLKID_PARTS_MAGIC);
#endif

    PyModule_AddIntConstant(module, "SUBLKS_LABEL", BLKID_SUBLKS_LABEL);
    PyModule_AddIntConstant(module, "SUBLKS_LABELRAW", BLKID_SUBLKS_LABELRAW);
    PyModule_AddIntConstant(module, "SUBLKS_UUID", BLKID_SUBLKS_UUID);
    PyModule_AddIntConstant(module, "SUBLKS_UUIDRAW", BLKID_SUBLKS_UUIDRAW);
    PyModule_AddIntConstant(module, "SUBLKS_TYPE", BLKID_SUBLKS_TYPE);
    PyModule_AddIntConstant(module, "SUBLKS_SECTYPE", BLKID_SUBLKS_SECTYPE);
    PyModule_AddIntConstant(module, "SUBLKS_USAGE", BLKID_SUBLKS_USAGE);
    PyModule_AddIntConstant(module, "SUBLKS_VERSION", BLKID_SUBLKS_VERSION);
    PyModule_AddIntConstant(module, "SUBLKS_MAGIC", BLKID_SUBLKS_MAGIC);

	Py_INCREF(&PyType_BlkId);
	Py_INCREF(BlkId_error);

	PyModule_AddObject(module, "BlkId", (PyObject*)(&PyType_BlkId));
	PyModule_AddObject(module, "error", BlkId_error);
}
