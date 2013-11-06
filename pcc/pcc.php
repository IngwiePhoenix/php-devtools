<?php class Compiler {

	public function main($argc, $argv) {
		print_r( $this->getopt($argv) );
	}
	
	public function getopt($argv) {
		$args = array("f"=>array(), "i"=>array(), "o"=>"p.out", "e"=>PHP_BINARY);
		$opts = array(
			"-f",  // = File
			"-o",  // = Output file
			"-i",  // = Included file (Will be written in preprocessing)
			"-e"   // = PHP binary
		);
		for($i=1; $i <= count($argv); null) {
			$i==10 && print_r($argv) && die();
			$v=$argv[$i];
			#echo "Index: $i, Value: $v, Count: ".count($argv)."\n";
			if(strlen($v)!=2 && in_array(substr($v,0,2), $opts)) {
				$argv[$i] = substr($v,0,2);					// insert arg back to place
				array_splice($argv, $i+1, 0, substr($v,2)); // add val after
				$v=$argv[$i]; 								// Re-register $v
			}
			switch($v) {
				case "-f":
					$v2=$argv[$i+1]; // Our file is the next arg...i think.
					if(file_exists($v2)) {
						$args["f"][]=$v2;
					} else echo "!(-f)> Omitting $v2 - file not found!\n";
					$i=$i+2;
				break;
				case "-i":
					$v2=$argv[$i+1]; // Our file is the next arg...i think.
					if(file_exists($v2)) {
						$args["i"][]=$v2;
					} else echo "!(-i)> Omitting $v2 - file not found!\n";
					$i=$i+2;
				break;
				case "-o":
					$args["o"]=$argv[$i+1];
					$i=$i+2;
				break;
				case "-e":
					$args["e"]=$argv[$i+1];
					$i=$i+2;
				break;
				default:
					if(file_exists($v)) { $args["f"][]=$v; }
					$i++;
				break;
			}
		}
		return $args;
	}
	
	public function compileObject($in, $out) {}
	public function compileExecurable(array $ins, $out) {}
	public function findLibs() {}

} return (new Compiler)->main($argc, $argv); ?>