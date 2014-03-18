#!/usr/bin/python2

import distutils
from distutils.core import setup
from distutils.extension import Extension
import glob
import os

blkid_ext = Extension(name = '_blkid',
				sources = glob.glob(os.path.join('src', '*.c')),
                libraries = ['blkid'])

setup(
    name = 'pyblkid',
    version = '0.0.1',
    ext_modules = [blkid_ext],
    license='GPLv2+',
    packages=['blkid'],
    package_dir={'blkid': 'blkid'},
    scripts = glob.glob(os.path.join('scripts', '*'))
 )