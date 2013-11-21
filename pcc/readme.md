# Php Code Compiler -> PCC

The command `pcc` was previously used by the roadsend PHP compiler. But sadly, all these gorgerous projects stopped - and facebook's hiphop only works on a **VERY** limited amount of platforms. So basically I am screwed =). But to get around this, I decided to code a little bit of own code to "compile" things - in fact, pcc aims to put PHP code, its libraries, and the PHP binary into a sort of bundle. It does not sound easy, but challenging!


# Requirements
- ANSI-C Compiler (tested on Clang -dumpversion => 4.2.1)
- OR PHP 5.x


# Usage
You can use this script as it is, modify it or whatever - even run it under a regular PHP binary. But you can also compile it into a stand-alone binary which does not even require PHP. To do so, there is a pcc.c file - which is ap re-generated source file. To compile pcc now run:

    gcc -O3 ph7.c pcc.c -o pcc
    
and you can call ./pcc just like a regular binary.

**NOTE**: The `__FILE__` constant is currently only working correctly on Mac OS X. Further support is planned.

After you compiled, you can look up the options by just running the binary without any option.

You can use it to re-compile itself if you want too:

    ./pcc ./pcc.php -o pcc
    
This will re-generate pcc.c and produce a new pcc binary.

To change the used binary, change it in the source code...there will be a switch for changing the compiler and flags ASAP. But it works as long as you can access GCC - you can even symlink clang or MS' compiler and do so. MinGW should work OOB.

To run this properly, you need pcc.php or the pcc binary together in a folder with ph7.c and ph7.h.


# Resources
The real ph7 project: http://ph7.symisc.net/

License is included as license.txt - pcc follows the same license, although I haven't typed it out yet. At least, the same scheme.
