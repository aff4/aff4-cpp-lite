AFF4 CPP Light v2.0.3
-------------

Copyright Schatz Forensic Pty Ltd, 2017-2019

Released under the LGPL v3.0+


Requirements:
-------------

* C++11 compiler (gcc/clang/msvc++)
* zlib
* snappy
* liblz4
* libraptor2
* cppunit (tests only)
* openssl (tests/examples only)
* doxygen (documentation).

General Installation (*nix):
----------------------------------

With all dependencies installed, typically, a user can:

```
$ ./configure
$ make
# make install
```

And libaff4 will be installed into /usr/local/

To override installation location, use the --prefix option, and compiler
selection may be made using environment variables as passed to ./configure.

General Installation (macOS):
----------------------------------

With all dependencies installed, typically, a user can:

```
$ ./configure CC=clang CXX=clang++ CXXFLAGS="-std=c++11 -stdlib=libc++ -O2 -g0" LDFLAGS="-stdlib=libc++ -L/opt/local/lib" --prefix=/opt/local
$ make
# make install
```

And libaff4 will be installed into /opt/local/

To override installation location, use the --prefix option, and compiler
selection may be made using environment variables as passed to ./configure.

General Installation (Windows):
-------------------------------

Solution files for Visual Studio 2015 are located in /win32.

See /win32/INSTALL.txt for full details if needing to rebuild dependencies.

## Notes:

This source repository includes the following projects
for building on Windows (via Visual Studio 2015):

* snappy 1.1.7   - http://google.github.io/snappy/
* zlib 1.2.11    - http://zlib.net
* raptor2 2.0.15 - http://librdf.org/

These projects are distributed in accordance with their relevant license.

Unit Tests:
-----------

Unit tests can be built and run with:

```
$ make check
$ make test
```

Note: Typically, ``make check`` will build and run tests, however running of 
separated out to assist with integration with IDEs.

Example Applications:
---------------------

Three example applications are contained in ./examples. These are:

* aff4-info - Display the Image metadata cotained in the container to console.
* aff4-digest - Open the given container, and create a SHA1 digest of the first 
in the container.
* aff4-extract - Open the given container, and extract the first image as a RAW/dd.

Notes:
------

v2.0.3 is a near complete rewrite of the v1.0.0 of the library, and exhibits
a completely new API model. We did this so we have a simple, easy to read C++ implementation.
 
