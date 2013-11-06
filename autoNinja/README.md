# autoNinja

This tool is currently more like a PoC.

It allows you to create build.ninja files out of some PHP code. To use it, create a `ninja.an` file and put valid PHP code into it. You can use the following functions to produce the ninja build file.


## API
- setVar(string $key, string $value)
	- This will introduce a variable to ninja.
- setDefault(string/array $def)
	- Define one or more defaults
- source(string $rule, string $src, string $out [, array $vars=array()})
	- Add a source to build. This function could also be called build, but "source" sounded yet more nature.
- string out($file [, $out="o"])
	- Changes a file shuffix. Like: main.cpp -> main.o
- rule(string $name, string $command [, $description=null [, $vars=array()]])
	- Define a rule with it's name and command - additionally, also a description or some variables
- bool check(string $tool [, version $minVersion=null [, bool $required=true [, string $extraFlag=null]]])
	- Checks for a tool's existance. If supplied a minVersion, the gained version of said tool will be compared. If it fails, an error is displayed.
- array sh(string $cmd [, $args=array()])
	- Execute a shell command. This returns: [ 'out'=>string $output, 'code'=>int $errorCode ]

In your ninja.an, you can access $opt and $cwd. $opt represents the few flags, such as verbose and print, that autoNinja uses. We suggest you to use your own getopt-call for configuration options.


## The included files

The two .an files that are included are actual little scripts, that when put and then used in the program's folder will produce a build.ninja file. That is how I built hydra and dylibbundler :)

If your project is not too big, you can use this tool to speedfuly generate a ninja-file. You can also use this on bigger projects if yo want a simpler syntax than what these build systems currently have.


## Will this stay just a PoC?

Nope. I am currently working on PCC and this tool, and maybe a third to build a whole build system based on PHP. that might then make it all into one repository. :)y

