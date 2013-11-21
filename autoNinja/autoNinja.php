<?php

// This is autoNinja, a tool to generate ninja files from source, using PHP expressions.
// This tool is not intended for websites or anything, its a pure php-cli script.

$opt=getopt("f:o:pv",array("file:","out:","print","verbose"));

# configuration array
$_AN=array(
	'libraryFlag'=>'-l',
);

// Save our path
define('__AN__',__FILE__);

// Version in array form
function version() { return array('major'=>0, 'minor'=>1, 'state'=>'alpha'); }

function sh($cmd, $args=array()) {
	$cmd = escapeshellarg($cmd);
	foreach($args as $ai=>$av) $args[$ai]=escapeshellarg($av);
	$args = implode(' ',$args);
	$descSpec=array( 0=>array('pipe','r'), 1=>array('pipe','w'), 2=>array('pipe','w') );
	$pipes=array();
	$proc = proc_open("$cmd $args 2>&1", $descSpec, $pipes);
	if(is_resource($proc)) {
		$out=trim(stream_get_contents($pipes[1]));
		foreach($pipes as $i=>$p) fclose($pipes[$i]);
		return array('exitCode'=>proc_close($proc), 'out'=>$out);
	} else return false;
}

// Defaults
$AN__defs = array();
function setDefault() {
	global $AN__defs;
	$AN__defs[]=implode(" ", func_get_args());
}

// Variables
$AN__variables = array(
	// E.x.: "key"=>"value",
	"autoNinja"=>"0.1"
);
function setVar($key,$val) {
	global $AN__variables;
	$AN__variables[$key]=$val;
}

// Sources
$AN__sources = array(
	// Ex.: 'main.o'=>array( 'gcc', array('main.c'] ]
	// Or:  'prog'=>array( 'link', array('main.o', 'funcs.o'] ]
);
function source($rule, $src, $out, $vars=array()) {
	#import
	global $AN__sources, $AN__rules;
	
	# Sanity check
	if(!isset($AN__rules[$rule]) && $rule != 'phony') die("Unable to associate rule '".$rule."' with source '".$src."': Rule not found.\n");
	
	# Add the stuff
	$AN__sources[$out]=array($rule, $src, $vars);
}

// subninja
$AN__subninja = array(
	// Ex: 'php.ninja', 'upx.ninja', ...
);
function subninja($file) {
	global $AN__subninja;
	if(file_exists($file))
		$AN__subninja[]=$file;
	else
		die("-> Can not subninja '$file', not found.\n");
}

function out($in, $out="o") {
	$file = explode(".", $in);
	array_pop($file);
	$file[]=$out;
	return implode(".",$file);
}

// Rules
$AN__rules = array(
	// Ex.: 'gcc'=>array( 'gcc -Wall $in -o $out', 'desc' ]
);
function rule($name, $command, $description=null, $vars=array()) {
	global $AN__rules;
	$AN__rules[$name]=array($command, $description, $vars);
}

function check($tool, $minVersion=null, $required=true, $extraFlag=null) {
	echo "-> Checking for tool: $tool ... ";
	$found=false; $res=null;
	$flags=array('-v','-V','--version','-dumpversion');
	if(!is_null($extraFlag)) $flags[]=$extraFlag;
	foreach($flags as $vflag) {
		$res=sh($tool, array($vflag));
		   # Exitcode is zero      # The output of the command HAS 1 line
		if($res['exitCode']===0 && count( explode("\n",$res['out']) )===1) {
			$found=true;
		}
		if($found) break;
	}
	if(!$found) die("ERROR: Couldn't find tool: $tool\n");
	echo $res['out']."\n";
	if(!is_null($minVersion)) {
		if( ($res2 = version_compare( $minVersion, $res['out'], 'lt' ) ) == true ) {
			return $res2;
		} else die("!> Version missmatch! Found {$res['out']} -- Specified minimal version: $minVersion\n");
	} else return $found;
}

function checkLib($name, $compiler, $shuffix="c", $testInclude=false) { 
	global $_AN;
	echo "-> Checking for $name with $compiler {$_AN['libraryFlag']}{$name}... ";
	$tf=sys_get_temp_dir().DIRECTORY_SEPARATOR.uniqid().".$shuffix";
	file_put_contents(
		$tf,
		($testInclude != false ? "#include $testInclude\n":"")
		."int main() { return 0; }"
	);
	$sho=sh($compiler, array($_AN['libraryFlag'].$name, $tf));

	if($sho['exitCode']!=0) {
		echo "Not found! Compile $name first.\n";
		exit($sho['exitCode']);
	} else echo "OK\n";
}

function ninjaize() {
	# Import to local scope
	global $AN__defs, $AN__variables, $AN__sources, $AN__rules, $AN__subninja;

	$ninja=array(
		"# Ninja file generated using autoNinja\n"
	);

	foreach($AN__variables as $name => $value) {
		$ninja[] = "$name = $value";
	}
	$ninja[] = "";
	foreach($AN__rules as $name => $detail) {
		$txt = "rule $name\n"
		    ."  command = {$detail[0]}\n"
		    ."  description = ".(!is_null($detail[1]) ? $detail[1] : strtoupper($name).': $out');
		foreach($detail[2] as $vn=>$vk) $txt .= "\n  $vn = $vk";
		$ninja[]=$txt;
	}
	$ninja[] = "";
	foreach($AN__sources as $name => $detail) {
		$txt = "build $name: {$detail[0]} $detail[1]";
		foreach($detail[2] as $vn=>$vk) $txt .= "\n  $vn = $vk";
		$ninja[] = $txt;
	}
	$ninja[] = "";
	foreach($AN__subninja as $sj) {
		$ninja[] = "include $sj";
	}
	$ninja[] = "";
	foreach($AN__defs as $def) {
		$ninja[] = "default $def";
	}
	
	return implode("\n",$ninja)."\n";
}

$cwd=getcwd();
# decide input
if(file_exists("$cwd/ninja.an")) include "$cwd/ninja.an";
elseif(isset($opt['f'])) include($opt['f']);
elseif(isset($opt['file'])) include($opt['file']);
else { echo implode("\n",array(
"autoNinja 0.1 by Ingwie Phoenix",
" ",
"autoNinja will attempt to use ninja.an in the current working directory. Alternatively, you can use -f or --file to select a specific .an file.",
"Note: ninja.an files can accept command line args via getopt() too, or use the \$opt variable generated by autoNinja itself.",
" ",
"Usage: autoNinja [-f|--file|-o|--out] [switches needed by ninja.an...]",
"Switches:",
"    -f | --file=FILE :: Select other .an file. Default is: ninja.an",
"    -o | --out=FILE  :: Specify the name as which the resulting .ninja should be saved. Default is: build.ninja",
))."\n";
exit(1); }

$AN__out = "$cwd/build.ninja";
if(isset($opt['o'])) $AN__out = $opt['o'];
elseif(isset($opt['out'])) $AN__out = $opt['out'];
file_put_contents($AN__out, ninjaize());
