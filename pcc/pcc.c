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

			rc = ph7_compile_v2( pEngine, "<?php class Compiler {\n"
"\n"
"	public function main($argc, $argv) {\n"
"		$os=$this->getopt($argv);\n"
"		if(!empty($os[\'f\'])) {\n"
"			// We have something to compile.\n"
"			$objs = array();\n"
"			foreach($os[\'f\'] as $src) {\n"
"				if(isset($os[\'v\'])) echo \"-> Working: $src\\n\";\n"
"				if(is_readable($src)) {\n"
"					$objs[] = $this->createObject(file_get_contents($src));\n"
"				} else {\n"
"					die(\"!> Could not work <{$src}> - file not readable.\\n\");\n"
"				}\n"
"			}\n"
"			// Build the source code\n"
"			if(isset($os[\'v\'])) echo \"-> Creating source off \".count($objs).\" objects\\n\";\n"
"			$source = $this->createSource($objs);\n"
"			$fname = null;\n"
"			if(isset($os[\'p\'])) {\n"
"				// The user wants to print the C files - so save them to the output path\n"
"				$fname = $os[\'o\'].\".c\";\n"
"			} else {\n"
"				$fname = sys_get_temp_dir().DIRECTORY_SEPARATOR.uniqid().\"-pcc.c\";\n"
"			}\n"
"			if(isset($os[\'v\'])) echo \"-> Source code saved to: {$fname}\\n\";\n"
"			// output\n"
"			file_put_contents($fname, $source);				\n"
"			$me = dirname(__FILE__);\n"
"			// compile\n"
"			$cmd = \"gcc -w -I$me $me/ph7.c {$os[\'o\']}.c -o {$os[\'o\']}\";\n"
"			if(isset($os[\'v\'])) echo \"-> Compiling: {$cmd}\\n\";\n"
"			system($cmd);\n"
"		} else {\n"
"			echo implode(\"\\n\", array(\n"
"				\"PCC by Ingwie Phoenix\",\n"
"				\" \",\n"
"				\"Php Code Compiler lets you compile your PHP source into a C file, using the ph7 embedded engine.\",\n"
"				\" \",\n"
"				\" \",\n"
"				\"Usage: pcc [-i file] [-f file] [-e php_binary] [-o output] file...\",\n"
"				\" \",\n"
"				\"   -f file :: Add this file to the compilation.\",\n"
"				\"   -i file :: Include this file even if it\'s not found in the source.\",\n"
"				\"   -e bin  :: Specify a different PHP binary to operate with (not required if using the compiled version of PCC).\",\n"
"				\"   -o out  :: Name of the output binary.\",\n"
"				\"   file    :: Same as -f.\",\n"
"				\" \",\n"
"				\"Example: pcc ./foo.php -o foo\"\n"
"			)).\"\\n\";\n"
"			return 0;\n"
"		}\n"
"	}\n"
"	\n"
"	public function getopt($argv) {\n"
"		$args = array(\"f\"=>array(), \"i\"=>array(), \"o\"=>\"p.out\", \"e\"=>PHP_BINARY);\n"
"		$opts = array(\n"
"			\"-f\",  // = File\n"
"			\"-o\",  // = Output file\n"
"			\"-i\",  // = Included file (Will be written in preprocessing)\n"
"			\"-p\",   // = Print the source to where the -o utput is.\n"
"			\"-v\"    // Be verbose\n"
"		);\n"
"		for($i=1; $i <= count($argv); null) {\n"
"			$i==10 && print_r($argv) && die();\n"
"			$v=$argv[$i];\n"
"			if(strlen($v)!=2 && in_array(substr($v,0,2), $opts)) {\n"
"				$argv[$i] = substr($v,0,2);					// insert arg back to place\n"
"				array_splice($argv, $i+1, 0, substr($v,2)); // add val after\n"
"				$v=$argv[$i]; 								// Re-register $v\n"
"			}\n"
"			switch($v) {\n"
"				case \"-f\":\n"
"					$v2=$argv[$i+1]; // Our file is the next arg...i think.\n"
"					if(file_exists($v2)) {\n"
"						$args[\"f\"][]=$v2;\n"
"					} else echo \"!(-f)> Omitting $v2 - file not found!\\n\";\n"
"					$i=$i+2;\n"
"				break;\n"
"				case \"-i\":\n"
"					$v2=$argv[$i+1]; // Our file is the next arg...i think.\n"
"					if(file_exists($v2)) {\n"
"						$args[\"i\"][]=$v2;\n"
"					} else echo \"!(-i)> Omitting $v2 - file not found!\\n\";\n"
"					$i=$i+2;\n"
"				break;\n"
"				case \"-o\":\n"
"					$args[\"o\"]=$argv[$i+1];\n"
"					$i=$i+2;\n"
"				break;\n"
"				case \"-p\":\n"
"					$args[\"p\"]=true;\n"
"					$i++;\n"
"				break;\n"
"				case \"-v\":\n"
"					$args[\"v\"]=true;\n"
"					$i++;\n"
"				break;\n"
"				default:\n"
"					if(file_exists($v)) { $args[\"f\"][]=$v; }\n"
"					$i++;\n"
"				break;\n"
"			}\n"
"		}\n"
"		return $args;\n"
"	}\n"
"	\n"
"	public function createObject($phpString) {\n"
"		// Edit the source\n"
"		$phpString = trim(addslashes($phpString));\n"
"		$phpStringArray = explode(\"\\n\",$phpString);\n"
"		$count=count($phpStringArray)-1;\n"
"		foreach($phpStringArray as $index=>$string) {\n"
"			echo \"-> $count versus $index\\n\";\n"
"			if($count !== $index) {\n"
"				echo \"-> Adding newline\\n\";\n"
"				$nl = \'\\\\n\';\n"
"			} else {\n"
"				echo \"-> NOT adding newline\\n\";\n"
"				$nl = null;\n"
"			}\n"
"			$phpStringArray[$index] = \'\"\'.$string.$nl.\'\"\';\n"
"		}\n"
"		return $phpStringArray; #Array\n"
"	}\n"
"	public function createSource($objs) {\n"
"		// Joined array of everything.\n"
"		$phpStringArray=array();\n"
"		foreach($objs as $obj) {\n"
"			// Each object is one exploded script\n"
"			$phpStringArray = array_merge($phpStringArray, $obj);\n"
"		}\n"
"		$phpString = implode(\"\\n\",$phpStringArray);\n"
"		\n"
"		// Source generator\n"
"		$c = \'// Generated using PCC\n"
"			#include \"ph7.h\"\n"
"			#include <stdio.h>\n"
"			#include <stdlib.h>\n"
"			#ifdef __WINNT__\n"
"			#include <Windows.h>\n"
"			#else\n"
"			/* Assume UNIX */\n"
"			#include <unistd.h>\n"
"			#endif\n"
"			#ifndef STDOUT_FILENO\n"
"			#define STDOUT_FILENO	1\n"
"			#endif\n"
"\n"
"			static void Fatal(const char *zMsg) { puts(zMsg); ph7_lib_shutdown(); exit(0); }\n"
"			\n"
"			#ifdef __APPLE__\n"
"				#define HAS_getMe\n"
"				#include <stdint.h>\n"
"				#include <mach-o/dyld.h>\n"
"				char *getMe() {\n"
"					char path[1024];\n"
"					char *empty=\" \";\n"
"					uint32_t *size = (uint32_t)sizeof(path);\n"
"					if( _NSGetExecutablePath(path, &size) == 0 ) {\n"
"						return path;\n"
"					} else {\n"
"						printf(\"Error with exe\\n\");\n"
"						return empty;\n"
"					}\n"
"				}\n"
"			#endif\n"
"			\n"
"			int ph7_fnc_system(ph7_context *pCtx, int argc, ph7_value **argv) {\n"
"				const char *cmd = ph7_value_to_string(argv[0], NULL);\n"
"				int res = system(cmd);\n"
"				ph7_result_int(pCtx, res);\n"
"				return PH7_OK;\n"
"			}\n"
"			#ifdef HAS_getMe\n"
"			void ph7_getMe(ph7_value *pVal, void *userData) {\n"
"				char *me = getMe();\n"
"				int *nlen=(int*)sizeof(me);\n"
"				ph7_value_string(pVal, me, -1);\n"
"			}\n"
"			#endif\n"
"\n"
"			static int Output_Consumer(const void *pOutput, unsigned int nOutputLen, void *pUserData/* Unused */) {\n"
"			#ifdef __WINNT__\n"
"				BOOL rc;\n"
"				rc = WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), pOutput, (DWORD)nOutputLen, 0, 0);\n"
"				if( !rc ){\n"
"					/* Abort processing */\n"
"					return PH7_ABORT;\n"
"				}\n"
"			#else\n"
"				ssize_t nWr;\n"
"				nWr = write(STDOUT_FILENO, pOutput, nOutputLen);\n"
"				if( nWr < 0 ){\n"
"					/* Abort processing */\n"
"					return PH7_ABORT;\n"
"				}\n"
"			#endif /* __WINT__ */\n"
"			return PH7_OK;\n"
"		}\n"
"\n"
"		int main(int argc, char *argv[]) {\n"
"			ph7 *pEngine; /* PH7 engine */\n"
"			ph7_vm *pVm;  /* Compiled PHP program */\n"
"			int rc;\n"
"			rc = ph7_init(&pEngine);\n"
"			if( rc != PH7_OK ){ Fatal(\"Error while allocating a new PH7 engine instance\"); }\n"
"\n"
"			rc = ph7_compile_v2( pEngine, \'.$phpString.\', -1, &pVm, 0);\n"
"			if( rc != PH7_OK ){\n"
"				if( rc == PH7_COMPILE_ERR ){\n"
"					const char *zErrLog;\n"
"					int nLen;\n"
"					ph7_config( pEngine, PH7_CONFIG_ERR_LOG, &zErrLog, &nLen );\n"
"					if( nLen > 0 ){ puts(zErrLog); }\n"
"				} Fatal(\"Compile error\");\n"
"			}\n"
"\n"
"			rc = ph7_vm_config( pVm, PH7_VM_CONFIG_OUTPUT, Output_Consumer, 0 );\n"
"			if( rc != PH7_OK ){ Fatal(\"Error while installing the VM output consumer callback\"); }\n"
"			\n"
"			// funcs\n"
"			ph7_create_function(\n"
"				pVm,\n"
"				\"system\",\n"
"				ph7_fnc_system,\n"
"				0\n"
"			);\n"
"			\n"
"			#ifdef HAS_getMe\n"
"			// fix __FILE__\n"
"			ph7_create_constant(pVm, \"__FILE__\", ph7_getMe, 0);\n"
"			#endif\n"
"			\n"
"			// argc, argv\n"
"			ph7_value *pArgc = ph7_new_scalar(pVm);\n"
"			ph7_value_int(pArgc, argc);\n"
"			rc = ph7_vm_config( pVm, PH7_VM_CONFIG_CREATE_VAR, \"argc\", pArgc );\n"
"			if( rc != PH7_OK ){ Fatal(\"Error while installing argc\\n\"); }\n"
"			for(int i=0; i<=argc-1; i++) {\n"
"				// PH7_VM_CONFIG_ARGV_ENTRY\n"
"				rc = ph7_vm_config( pVm, PH7_VM_CONFIG_ARGV_ENTRY, argv[i] );\n"
"				if( rc != PH7_OK ){ Fatal(\"Error while installing argv\\n\"); }\n"
"			}\n"
"\n"
"\n"
"			ph7_vm_config(pVm, PH7_VM_CONFIG_ERR_REPORT);\n"
"		\n"
"			ph7_vm_exec(pVm, 0);\n"
"\n"
"			ph7_vm_release(pVm);\n"
"			ph7_release(pEngine);\n"
"			return 0;\n"
"		}\';\n"
"		return $c;\n"
"	}\n"
"\n"
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