# gcc `__attribute__((...))` demo #

`plugin/attribute_demo_plugin.c` implements a gcc plugin that handels
`__attribute__((ATTRIBUTE__DEMO))` annotations in C source code. It
dumps the elements that had the attribute set to a file whose name is
specified by `-fplugin-arg-attribute_demo_plugin-out=OUTPUT_FILE`.

`example/mandel.c` is an annotated C file that demonstrates the capabilities
of the plugin.

## Prerequisites ##

* GCC plugin development headers. Install `gcc-X-plugin-dev` under Debian/Ubuntu.

## Getting Started ##

* Executing `make` in `example` should build the plugin and pass the
  example code to it. It generates the file `out.xml` that contains a
  dump of the objects in the C source that had the attributes set.

## Known Issues ##

* When parsing attributes set for function parameters, the plugin does not
  dump the function name

## License ##

GNU GPLv3+, see LICENSE.md
