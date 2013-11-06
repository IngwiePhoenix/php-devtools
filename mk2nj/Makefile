DEFS = -DPHP_ATOM_INC -I$(top_builddir)/include -I$(top_builddir)/main -I$(top_srcdir)
COMMON_FLAGS = $(DEFS) $(INCLUDES) $(EXTRA_INCLUDES) $(CPPFLAGS) $(PHP_FRAMEWORKPATH)

build-modules: $(PHP_MODULES) $(PHP_ZEND_EX)

Zend/zend_multibyte.lo: /Users/Ingwie/Work/drag0n-php/Zend/zend_multibyte.c
	$(LIBTOOL) --mode=compile $(CC)  -IZend/ -I/Users/Ingwie/Work/drag0n-php/Zend/ $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS) -c /Users/Ingwie/Work/drag0n-php/Zend/zend_multibyte.c -o Zend/zend_multibyte.lo 

Zend/zend_language_scanner.lo: /Users/Ingwie/Work/drag0n-php/Zend/zend_language_parser.h
Zend/zend_ini_scanner.lo: /Users/Ingwie/Work/drag0n-php/Zend/zend_ini_parser.h

