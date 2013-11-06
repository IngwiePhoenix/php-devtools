<?php

// This tool converts all build statements and alike found in a makefile to a dummy rule, preserving original commands etc.

$file = getcwd()."/Makefile";
$mk = file_get_contents($file);
$mk_out = str_replace("\\\n"," ",$mk);
$mk_out = explode("\n",$mk_out);

$builds=[];
$lastBuild=null;

ob_start();
foreach($mk_out as $i=>$v) {
	$v2 = str_split($v);
	foreach($v2 as $vp) {
		if($vp == "@" || $vp == "(") {
			if(!is_null($lastBuild)) {
				$builds[$lastBuild]['command'][]=$v;
			}
			break;
		}
		if($vp == ":") {
			// A build statement
			list($what2build, $that2build) = explode(":",$v,2);
			$builds[$what2build]=['in'=>$that2build, 'command'=>[]];
			$i2 = $i+1;
			do {
				$v3 = $mk_out[$i2];
				if(substr($v3,0,1)=="\t") $builds[$what2build]['command'][]=ltrim($v3);
				else break;
			} while($i2++);
			$lastBuild=$what2build;
			break;
		}
		if($vp == "=") {
			echo $v."\n";
			break;
		}
	}
}

echo "\n\n";
echo "rule MakefileBuildCommand\n";
echo "  command = \$command\n";
echo "  description = BUILDING: \$out\n\n";

foreach($builds as $out=>$build) {
	$rule = 'MakefileBuildCommand';
	if(!file_exists(getcwd().'/'.$out)) $rule = 'phony';
	if(empty($build['command'])) $build['command']=["exit 0;"];
	echo "build {$out}: $rule {$build['in']}\n";
	echo "  command = ".implode("; ",$build['command'])."\n\n";
}

$res = ob_get_clean();
$res = str_replace("\t", " ", $res);
$res = str_replace('$@', " ", $res);
$res = str_replace( ":.lo=.o", "", $res );
$res = preg_replace('/\$\((.+?)\)/','\${$1}', $res);
echo $res;