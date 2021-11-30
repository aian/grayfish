Grayfish
========

**THIS MODULE IS NOT AVAILABLE. FOR NOW, IT IS UNDER DEVELOPMENT.**


Grayfish is a static website generator for DocBook XML documents.


Overview
--------

Grayfish is a website generator, which generats static HTML files from
each DocBook XML documents as an integrated content.

[DocBook](https://docbook.org/) is a large XML schema for technical
documentation. The documents written in DocBook is machine readable
and you can apply reliable XML tools and technologies to them.

On the other hand, to organize or integrate these documents as a
integrated content was so hard. For example, publishing RSS/Atom feeds
or displaying `rescently update articles' on your site needs the other
tools outside the DocBook technology.

Grayfish helps you to build an integrated website from separated
documents statically.

Prerequisites
-------------

### To Run

- Microsoft Windows 10 >= v1903 build 18362.657
- [LibXML](http://xmlsoft.org/) >= 2.9.9
- [LibXSLT](http://xmlsoft.org/libxslt/) >= 1.1.33

### To Build

- [MSYS2](https://www.msys2.org/)
- [MinGW64 GCC](http://mingw-w64.org/) >= 9.1.0
- [CMake](https://cmake.org/) >= 3.14
- [CUnit](http://cunit.sourceforge.net/) >= 2.1.3

Building Modules
----------------

**THIS BUILD PROCESS WILL BE CHANGED IN FUTURE.**

The file `configure` is a CMake script to manage simple build system.
To build modules, just do as below:


    % cd grayfish
    % ./configure init
    % cd .build && make

To clean intermediate files, use `clean` sub-command.

    % ./configure clean
