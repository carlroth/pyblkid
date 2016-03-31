##############################
#
# simple makefile
#
##############################

default:
	:

ifndef ONL
ONL		= $(HOME)/work/switch/onl
endif

dist-buildroot:
	python setup.py clean
	python setup.py sdist --formats=bztar
	cp dist/pyblkid*.tar.bz2 $(ONL)/packages/base/any/initrds/buildroot/builds/patches/.
	rm $(ONL)/packages/base/any/initrds/buildroot/builds/.setup.done
	rm -fr $(ONL)/packages/base/any/initrds/buildroot/builds/buildroot-x86_64/build/python-pyblkid*
	rm -fr $(ONL)/packages/base/any/initrds/buildroot/builds/buildroot-powerpc/build/python-pyblkid*
	rm -fr $(ONL)/packages/base/any/initrds/buildroot/builds/buildroot-mirror/package/python-pyblkid
