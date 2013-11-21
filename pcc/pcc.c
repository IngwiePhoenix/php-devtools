// Generated using PCC
			#include "ph7.h"
			#include <stdio.h>
			#include <stdlib.h>
			#ifdef __WINNT__
			#include <Windows.h>
			#else
			/* Assume UNIX */
			#include <unistd.h>
			#endif
			#ifndef STDOUT_FILENO
			#define STDOUT_FILENO	1
			#endif

			static void Fatal(const char *zMsg) { puts(zMsg); ph7_lib_shutdown(); exit(0); }
			
			#ifdef __APPLE__
				#define HAS_getMe
				#include <stdint.h>
				#include <mach-o/dyld.h>
				char *getMe() {
					char path[1024];
					char *empty=" ";
					uint32_t *size = (uint32_t)sizeof(path);
					if( _NSGetExecutablePath(path, &size) == 0 ) {
						return path;
					} else {
						printf("Error with exe\n");
						return empty;
					}
				}
			#endif
			
			int ph7_fnc_system(ph7_context *pCtx, int argc, ph7_value **argv) {
				const char *cmd = ph7_value_to_string(argv[0], NULL);
				int res = system(cmd);
				ph7_result_int(pCtx, res);
				return PH7_OK;
			}
			#ifdef HAS_getMe
			void ph7_getMe(ph7_value *pVal, void *userData) {
				char *me = getMe();
				int *nlen=(int*)sizeof(me);
				ph7_value_string(pVal, me, -1);
			}
			#endif

			static int Output_Consumer(const void *pOutput, unsigned int nOutputLen, void *pUserData/* Unused */) {
			#ifdef __WINNT__
				BOOL rc;
				rc = WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), pOutput, (DWORD)nOutputLen, 0, 0);
				if( !rc ){
					/* Abort processing */
					return PH7_ABORT;
				}
			#else
				ssize_t nWr;
				nWr = write(STDOUT_FILENO, pOutput, nOutputLen);
				if( nWr < 0 ){
					/* Abort processing */
					return PH7_ABORT;
				}
			#endif /* __WINT__ */
			return PH7_OK;
		}

		int main(int argc, char *argv[]) {
			ph7 *pEngine; /* PH7 engine */
			ph7_vm *pVm;  /* Compiled PHP program */
			int rc;
			rc = ph7_init(&pEngine);
			if( rc != PH7_OK ){ Fatal("Error while allocating a new PH7 engine instance"); }

			rc = ph7_compile_v2( pEngine, "<?php class Compiler {"
""
"	public function main($argc, $argv) {"
"		$os=$this->getopt($argv);"
"		if(!empty($os[\'f\'])) {"
"			// We have something to compile."
"			$objs = array();"
"			foreach($os[\'f\'] as $src) {"
"				if(isset($os[\'v\'])) echo \"-> Working: $src\\n\";"
"				if(is_readable($src)) {"
"					$objs[] = $this->createObject(file_get_contents($src));"
"				} else {"
"					die(\"!> Could not work <{$src}> - file not readable.\\n\");"
"				}"
"			}"
"			// Build the source code"
"			if(isset($os[\'v\'])) echo \"-> Creating source off \".count($objs).\" objects\\n\";"
"			$source = $this->createSource($objs);"
"			$fname = null;"
"			if(isset($os[\'p\'])) {"
"				// The user wants to print the C files - so save them to the output path"
"				$fname = $os[\'o\'].\".c\";"
"			} else {"
"				$fname = sys_get_temp_dir().DIRECTORY_SEPARATOR.uniqid().\"-pcc.c\";"
"			}"
"			if(isset($os[\'v\'])) echo \"-> Source code saved to: {$fname}\\n\";"
"			// output"
"			file_put_contents($fname, $source);				"
"			$me = dirname(__FILE__);"
"			// compile"
"			$cmd = \"gcc -w -I$me $me/ph7.c {$os[\'o\']}.c -o {$os[\'o\']}\";"
"			if(isset($os[\'v\'])) echo \"-> Compiling: {$cmd}\\n\";"
"			system($cmd);"
"		} else {"
"			echo implode(\"\\n\", array("
"				\"PCC by Ingwie Phoenix\","
"				\" \","
"				\"Php Code Compiler lets you compile your PHP source into a C file, using the ph7 embedded engine.\","
"				\" \","
"				\" \","
"				\"Usage: pcc [-i file] [-f file] [-e php_binary] [-o output] file...\","
"				\" \","
"				\"   -f file :: Add this file to the compilation.\","
"				\"   -i file :: Include this file even if it\'s not found in the source.\","
"				\"   -e bin  :: Specify a different PHP binary to operate with (not required if using the compiled version of PCC).\","
"				\"   -o out  :: Name of the output binary.\","
"				\"   file    :: Same as -f.\","
"				\" \","
"				\"Example: pcc ./foo.php -o foo\""
"			)).\"\\n\";"
"			return 0;"
"		}"
"	}"
"	"
"	public function getopt($argv) {"
"		$args = array(\"f\"=>array(), \"i\"=>array(), \"o\"=>\"p.out\", \"e\"=>PHP_BINARY);"
"		$opts = array("
"			\"-f\",  // = File"
"			\"-o\",  // = Output file"
"			\"-i\",  // = Included file (Will be written in preprocessing)"
"			\"-p\",   // = Print the source to where the -o utput is."
"			\"-v\"    // Be verbose"
"		);"
"		for($i=1; $i <= count($argv); null) {"
"			$i==10 && print_r($argv) && die();"
"			$v=$argv[$i];"
"			if(strlen($v)!=2 && in_array(substr($v,0,2), $opts)) {"
"				$argv[$i] = substr($v,0,2);					// insert arg back to place"
"				array_splice($argv, $i+1, 0, substr($v,2)); // add val after"
"				$v=$argv[$i]; 								// Re-register $v"
"			}"
"			switch($v) {"
"				case \"-f\":"
"					$v2=$argv[$i+1]; // Our file is the next arg...i think."
"					if(file_exists($v2)) {"
"						$args[\"f\"][]=$v2;"
"					} else echo \"!(-f)> Omitting $v2 - file not found!\\n\";"
"					$i=$i+2;"
"				break;"
"				case \"-i\":"
"					$v2=$argv[$i+1]; // Our file is the next arg...i think."
"					if(file_exists($v2)) {"
"						$args[\"i\"][]=$v2;"
"					} else echo \"!(-i)> Omitting $v2 - file not found!\\n\";"
"					$i=$i+2;"
"				break;"
"				case \"-o\":"
"					$args[\"o\"]=$argv[$i+1];"
"					$i=$i+2;"
"				break;"
"				case \"-p\":"
"					$args[\"p\"]=true;"
"					$i++;"
"				break;"
"				case \"-v\":"
"					$args[\"v\"]=true;"
"					$i++;"
"				break;"
"				default:"
"					if(file_exists($v)) { $args[\"f\"][]=$v; }"
"					$i++;"
"				break;"
"			}"
"		}"
"		return $args;"
"	}"
"	"
"	public function createObject($phpString) {"
"		// Edit the source"
"		$phpString = trim(addslashes($phpString));"
"		$phpStringArray = explode(\"\\n\",$phpString);"
"		$count=count($phpStringArray);"
"		foreach($phpStringArray as $index=>$string) {"
"			if($count <= $index) {"
"				$nl = \'\\\\n\';"
"			} else {"
"				$nl = null;"
"			}"
"			$phpStringArray[$index] = \'\"\'.$string.$nl.\'\"\';"
"		}"
"		return $phpStringArray; #Array"
"	}"
"	public function createSource($objs) {"
"		// Joined array of everything."
"		$phpStringArray=array();"
"		foreach($objs as $obj) {"
"			// Each object is one exploded script"
"			$phpStringArray = array_merge($phpStringArray, $obj);"
"		}"
"		$phpString = implode(\"\\n\",$phpStringArray);"
"		"
"		// Source generator"
"		$c = \'// Generated using PCC"
"			#include \"ph7.h\""
"			#include <stdio.h>"
"			#include <stdlib.h>"
"			#ifdef __WINNT__"
"			#include <Windows.h>"
"			#else"
"			/* Assume UNIX */"
"			#include <unistd.h>"
"			#endif"
"			#ifndef STDOUT_FILENO"
"			#define STDOUT_FILENO	1"
"			#endif"
""
"			static void Fatal(const char *zMsg) { puts(zMsg); ph7_lib_shutdown(); exit(0); }"
"			"
"			#ifdef __APPLE__"
"				#define HAS_getMe"
"				#include <stdint.h>"
"				#include <mach-o/dyld.h>"
"				char *getMe() {"
"					char path[1024];"
"					char *empty=\" \";"
"					uint32_t *size = (uint32_t)sizeof(path);"
"					if( _NSGetExecutablePath(path, &size) == 0 ) {"
"						return path;"
"					} else {"
"						printf(\"Error with exe\\n\");"
"						return empty;"
"					}"
"				}"
"			#endif"
"			"
"			int ph7_fnc_system(ph7_context *pCtx, int argc, ph7_value **argv) {"
"				const char *cmd = ph7_value_to_string(argv[0], NULL);"
"				int res = system(cmd);"
"				ph7_result_int(pCtx, res);"
"				return PH7_OK;"
"			}"
"			#ifdef HAS_getMe"
"			void ph7_getMe(ph7_value *pVal, void *userData) {"
"				char *me = getMe();"
"				int *nlen=(int*)sizeof(me);"
"				ph7_value_string(pVal, me, -1);"
"			}"
"			#endif"
""
"			static int Output_Consumer(const void *pOutput, unsigned int nOutputLen, void *pUserData/* Unused */) {"
"			#ifdef __WINNT__"
"				BOOL rc;"
"				rc = WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), pOutput, (DWORD)nOutputLen, 0, 0);"
"				if( !rc ){"
"					/* Abort processing */"
"					return PH7_ABORT;"
"				}"
"			#else"
"				ssize_t nWr;"
"				nWr = write(STDOUT_FILENO, pOutput, nOutputLen);"
"				if( nWr < 0 ){"
"					/* Abort processing */"
"					return PH7_ABORT;"
"				}"
"			#endif /* __WINT__ */"
"			return PH7_OK;"
"		}"
""
"		int main(int argc, char *argv[]) {"
"			ph7 *pEngine; /* PH7 engine */"
"			ph7_vm *pVm;  /* Compiled PHP program */"
"			int rc;"
"			rc = ph7_init(&pEngine);"
"			if( rc != PH7_OK ){ Fatal(\"Error while allocating a new PH7 engine instance\"); }"
""
"			rc = ph7_compile_v2( pEngine, \'.$phpString.\', -1, &pVm, 0);"
"			if( rc != PH7_OK ){"
"				if( rc == PH7_COMPILE_ERR ){"
"					const char *zErrLog;"
"					int nLen;"
"					ph7_config( pEngine, PH7_CONFIG_ERR_LOG, &zErrLog, &nLen );"
"					if( nLen > 0 ){ puts(zErrLog); }"
"				} Fatal(\"Compile error\");"
"			}"
""
"			rc = ph7_vm_config( pVm, PH7_VM_CONFIG_OUTPUT, Output_Consumer, 0 );"
"			if( rc != PH7_OK ){ Fatal(\"Error while installing the VM output consumer callback\"); }"
"			"
"			// funcs"
"			ph7_create_function("
"				pVm,"
"				\"system\","
"				ph7_fnc_system,"
"				0"
"			);"
"			"
"			#ifdef HAS_getMe"
"			// fix __FILE__"
"			ph7_create_constant(pVm, \"__FILE__\", ph7_getMe, 0);"
"			#endif"
"			"
"			// argc, argv"
"			ph7_value *pArgc = ph7_new_scalar(pVm);"
"			ph7_value_int(pArgc, argc);"
"			rc = ph7_vm_config( pVm, PH7_VM_CONFIG_CREATE_VAR, \"argc\", pArgc );"
"			if( rc != PH7_OK ){ Fatal(\"Error while installing argc\\n\"); }"
"			for(int i=0; i<=argc-1; i++) {"
"				// PH7_VM_CONFIG_ARGV_ENTRY"
"				rc = ph7_vm_config( pVm, PH7_VM_CONFIG_ARGV_ENTRY, argv[i] );"
"				if( rc != PH7_OK ){ Fatal(\"Error while installing argv\\n\"); }"
"			}"
""
""
"			ph7_vm_config(pVm, PH7_VM_CONFIG_ERR_REPORT);"
"		"
"			ph7_vm_exec(pVm, 0);"
""
"			ph7_vm_release(pVm);"
"			ph7_release(pEngine);"
"			return 0;"
"		}\';"
"		return $c;"
"	}"
""
"} $cmp=new Compiler; $cmp->main($argc, $argv);", -1, &pVm, 0);
			if( rc != PH7_OK ){
				if( rc == PH7_COMPILE_ERR ){
					const char *zErrLog;
					int nLen;
					ph7_config( pEngine, PH7_CONFIG_ERR_LOG, &zErrLog, &nLen );
					if( nLen > 0 ){ puts(zErrLog); }
				} Fatal("Compile error");
			}

			rc = ph7_vm_config( pVm, PH7_VM_CONFIG_OUTPUT, Output_Consumer, 0 );
			if( rc != PH7_OK ){ Fatal("Error while installing the VM output consumer callback"); }
			
			// funcs
			ph7_create_function(
				pVm,
				"system",
				ph7_fnc_system,
				0
			);
			
			#ifdef HAS_getMe
			// fix __FILE__
			ph7_create_constant(pVm, "__FILE__", ph7_getMe, 0);
			#endif
			
			// argc, argv
			ph7_value *pArgc = ph7_new_scalar(pVm);
			ph7_value_int(pArgc, argc);
			rc = ph7_vm_config( pVm, PH7_VM_CONFIG_CREATE_VAR, "argc", pArgc );
			if( rc != PH7_OK ){ Fatal("Error while installing argc\n"); }
			for(int i=0; i<=argc-1; i++) {
				// PH7_VM_CONFIG_ARGV_ENTRY
				rc = ph7_vm_config( pVm, PH7_VM_CONFIG_ARGV_ENTRY, argv[i] );
				if( rc != PH7_OK ){ Fatal("Error while installing argv\n"); }
			}


			ph7_vm_config(pVm, PH7_VM_CONFIG_ERR_REPORT);
		
			ph7_vm_exec(pVm, 0);

			ph7_vm_release(pVm);
			ph7_release(pEngine);
			return 0;
		}