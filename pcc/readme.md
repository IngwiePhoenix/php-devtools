# Php Code Compiler -> PCC

The command `pcc` was previously used by the roadsend PHP compiler. But sadly, all these gorgerous projects stopped - and facebook's hiphop only works on a **VERY** limited amount of platforms. So basically I am screwed =). But to get around this, I decided to code a little bit of own code to "compile" things - in fact, pcc aims to put PHP code, its libraries, and the PHP binary into a sort of bundle. It does not sound easy, but challenging!


## Test invocation
```bash
php pcc.php -ffoo.php -f bar.php -i baz.php -o a.out
```


## Concept

1. PCC will first make note of the files, and then iterate over them to detect include(_once) or require(_once) calls, and add the files into the list and comparing them to avoid duplicates.
2. Then it will start to bring the files into order in which they need to be loaded/executed. In other words, it builds a big file structure and lets it end up as a sort-of PHAR.
3. Now we have the file, probably quite large =). We now grab the PHP binary. We also compress our single PHP file.
4. Now we also need to grab the libraries...how I am gonna do that - eighter write a parser for `otool -L` or `ld`...and we of course copy them into our folder too. We will use our self-copied binary to detect libraries. We also will have to change the linker-arguments a little so we can create a more porbable structure.
5. We now should have:
	- php <- Executable
	- script.php <- Our script
	- library.... <- our libs. like, libcurl and such.
6. If above is right, we now need to compress it all into a rather big executable. here on, we may make use of C. We first convert the php script into base64, same with a compressed version of the binary and the libraries. C now needs to be auto-generated, to have all the lists and such. And voila, large, large file...
7. use gcc to compile the now-created file. The file now is not the smallest, but well...
8. The output binary now needs to do:
	1. Find a temporary folder.
	2. Extract binary, library and script.
	3. Run the script

This is not a "real" compression, and not the most elegant method, but we are able to close down our source code and execute it on different plattforms...at least, we dont need PHP everywhere.

## Notice...
When we grab the binary, you WILL loose all your extensions, if they arent compiled into PHP itself. Therefore, use a configuration like this:

```bash
./configure \
	--enable-embed=static \
	--enable-static=yes
```

Now you just add --with-MODULE or --enable-SOMETHING to add it into your binary. See the PHP manual for staticaly compiling PHP.


# This is just a concept.

It may work, it may not work. Whatever, this is basically what I have imagined for a little work-around for having no real compiler =)

If you want more information, read pcc.php :)

Of course, you are free to support me! ^.^
