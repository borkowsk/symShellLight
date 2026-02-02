# CMAKE configuration for SYMSHELLLIGHT libraries wbsyshSVG wbsyshX11
##################################################################################
# X11 auto-defined under linux/unix for compiling X11 version of the library.
# WINDOWS auto-defined under MS Windows for compiling appr. version of the lib.
#
# Manually define WITHTESTS=true, if you want build test applications!
#
# (c)ISS UW, @author Wojciech Borkowski
# @date 2026-02-02 (last update)
##################################################################################

cmake_minimum_required(VERSION 3.11)

project(symshelllight)
set(CMAKE_CXX_STANDARD 11) # See: https://matgomes.com/setting-cpp-standard-cmake/

# Define WITHTESTS if you want build test applications!
set( WITHTESTS true )

IF (WIN32)
  # set stuff for windows
  message( ${PROJECT_NAME} ": MS Windows platform assumed.\n\tInside C/C++ code automatic _MSC_VER variable will be used!" )
  set( WINDOWS true )
  #set( MSVC 1 ) #old macro used also somewhere in C code?
  #set( WINDOWS_ONLY_SRC  "${WBRTM}/BASE/wb_assert.c" )
  #add_compile_options( /W4 ) #  /WX
ELSE()
  # set stuff for other systems
  message( ${PROJECT_NAME} ": Linux/unix platform with X11 assumed.")
  set( X11 true )
  #set( X_ONLY_SRC  "" )
  #add_compile_options( -Wall )
ENDIF()

include_directories("./INCL")
#see -> https://cmake.org/cmake/help/v3.16/manual/cmake-compile-features.7.html#manual:cmake-compile-features(7)
#message("C++ compiler flags: ${CMAKE_CXX_FLAGS}")

if(true) # SVG SYMSHELL LIB
	add_library( wbsyshLightSVG STATIC
			"SVG/symshsvg.cpp"
			"UTILS/sshdefs.c"
			"UTILS/sshutils.cpp"
			"UTILS/strtorgb.cpp"
			"UTILS/wieloboki.cpp"
			"UTILS/wb_pchar.cpp" #used internally
			"UTILS/wb_rand.cpp"
			"UTILS/wb_randg.c"
			"UTILS/sshmenufdummy.c" #dummy functions of menus
			 "SVG/errorent.c"
		)
	#target_compile_features(wbsyshSVG PUBLIC cxx_std_11)
endif()

if(X11) # only for X11 LIB
	# Szukanie X11 (podstawowa biblioteka)
	find_package(X11 REQUIRED)

	# Szukanie Xpm (często jako komponent X11 lub osobny pakiet)
	# Uwaga: Xpm jest częścią pakietu X11 w CMake, ale wymaga jawnego wskazania
	find_package(X11 REQUIRED COMPONENTS Xpm)

	if(X11_FOUND)
		include_directories(${X11_INCLUDE_DIR})
		# Dla Xpm (jeśli find_package go znalazło)
		if(X11_Xpm_FOUND)
			include_directories(${X11_Xpm_INCLUDE_DIR})
		endif()
	endif()

	add_definitions( -DHAVE_X11 -DUSE_X11 )
	add_library( wbsyshLight STATIC
			"X11/symshx11.c"
			"X11/wbx11ico.c"
			"UTILS/sshdefs.c"
			"UTILS/sshutils.cpp"
			"UTILS/strtorgb.cpp"
			"UTILS/wieloboki.cpp"
			"UTILS/wb_pchar.cpp"
			"UTILS/wb_rand.cpp"
			"UTILS/wb_randg.c"
			"UTILS/sshmenufdummy.c" #dummy functions of menus
			"X11/errorent.c"
		)

	#target_compile_features(wbsyshX11 PUBLIC cxx_std_11)
endif(X11)

if(WINDOWS) # only for WINDOWS - NOT TESTED IN THIS VERSION (after 2016)
	add_library( wbsyshLight STATIC
			"WIN/symshwin.c"
			"UTILS/sshutils.cpp"
			"UTILS/strtorgb.cpp"
			"UTILS/wieloboki.cpp"
			"UTILS/wb_pchar.cpp"
			"UTILS/wb_rand.cpp"
			"UTILS/wb_randg.c"
			"WIN/sshmenufwin.c"
			#"UTILS/sshmenufdummy.c" #dummy functions of menus
			"WIN/errorent.c"
		)

	#target_compile_features(wbsyshWIN PUBLIC cxx_std_11)
endif(WINDOWS)


## PROGRAMY TESTOWE  ##
## TEST APPLICATIONS ##
#######################

if(X11 AND WITHTESTS ) #X11 tests
	add_executable("TestmousX11"	"EXAMPLES/Testmous.cpp"    	)
	add_executable("TestpaleX11"	"EXAMPLES/Testpale.cpp"    	)
	add_executable("TestpolyX11"	"EXAMPLES/Testpoly.cpp"	)
	add_executable("TestrandX11"
					"EXAMPLES/Testrand.cpp"
					"UTILS/wb_randg.c"
					"UTILS/wb_rand.cpp"
				)
	add_executable("TestsymsX11"	"EXAMPLES/Testsyms.cpp"	)
	add_executable("TestsyshX11"	"EXAMPLES/Testsysh.cpp"	)
	add_executable("TurmitX11"	"EXAMPLES/Turmit.cpp"		)
	add_executable("CAGasX11"	"EXAMPLES/CAGas.cpp"		)

	target_link_libraries("TestmousX11" wbsyshLight X11 Xpm rt)
	target_link_libraries("TestpaleX11" wbsyshLight X11 Xpm rt)
	target_link_libraries("TestpolyX11" wbsyshLight X11 Xpm rt)
	target_link_libraries("TestrandX11" wbsyshLight X11 Xpm rt)
	target_link_libraries("TestsymsX11" wbsyshLight X11 Xpm rt)
	target_link_libraries("TestsyshX11" wbsyshLight X11 Xpm rt)
	target_link_libraries("TurmitX11"   wbsyshLight X11 Xpm rt)
	target_link_libraries("CAGasX11"    wbsyshLight X11 Xpm pthread rt)

	#add_executable("LinktestX11" "EXAMPLES/Linktest.cpp"    )
	#target_link_libraries("LinktestX11" wbsyshX11 X11 Xpm rt)
endif()

if( WINDOWS AND WITHTESTS ) #X11 tests
	add_executable("TestmousWIN"
				"EXAMPLES/Testmous.cpp"
			)
	add_executable("TestpaleWIN"
				"EXAMPLES/Testpale.cpp"
			)
	add_executable("TestpolyWIN"
				"EXAMPLES/Testpoly.cpp"
			)
	add_executable("TestrandWIN"
				"EXAMPLES/Testrand.cpp"
				"UTILS/wb_randg.c"
				"UTILS/wb_rand.cpp"
			)
	add_executable("TestsymsWIN"
				"EXAMPLES/Testsyms.cpp"
			)
	add_executable("TestsyshWIN"
				"EXAMPLES/Testsysh.cpp"
			)
	add_executable("TurmitWIN"
				"EXAMPLES/Turmit.cpp"
			)
	add_executable("CAGasWIN"
				"EXAMPLES/CAGas.cpp"
			)

	target_link_libraries("TestmousWIN" wbsyshLight  )
	target_link_libraries("TestpaleWIN" wbsyshLight  )
	target_link_libraries("TestpolyWIN" wbsyshLight  )
	target_link_libraries("TestrandWIN" wbsyshLight  )
	target_link_libraries("TestsymsWIN" wbsyshLight  )
	target_link_libraries("TestsyshWIN" wbsyshLight  )
	target_link_libraries("TurmitWIN"   wbsyshLight  )
	target_link_libraries("CAGasWIN"    wbsyshLight  )

	add_executable("LinktestWIN" "EXAMPLES/Linktest.cpp"    )
	target_link_libraries("LinktestWIN" wbsyshLight )
endif()


if(WITHTESTS) #SVG tests
	add_executable("TestmousSVG" "EXAMPLES/Testmous.cpp"    )
	add_executable("TestpaleSVG" "EXAMPLES/Testpale.cpp"    )
	add_executable("TestpolySVG" "EXAMPLES/Testpoly.cpp"    )
	add_executable("TestrandSVG"
					"EXAMPLES/Testrand.cpp"
					"UTILS/wb_randg.c"
					"UTILS/wb_rand.cpp"
				)
	add_executable("TestsymsSVG" "EXAMPLES/Testsyms.cpp"    )
	add_executable("TestsyshSVG" "EXAMPLES/Testsysh.cpp"    )
	add_executable("TurmitSVG" "EXAMPLES/Turmit.cpp"    )
	add_executable("CAGasSVG" "EXAMPLES/CAGas.cpp"    )

	if( WINDOWS )
		target_link_libraries("TestmousSVG" wbsyshLightSVG  )
		target_link_libraries("TestpaleSVG" wbsyshLightSVG  )
		target_link_libraries("TestpolySVG" wbsyshLightSVG  )
		target_link_libraries("TestrandSVG" wbsyshLightSVG  )
		target_link_libraries("TestsymsSVG" wbsyshLightSVG  )
		target_link_libraries("TestsyshSVG" wbsyshLightSVG  )
		target_link_libraries("TurmitSVG"   wbsyshLightSVG  )
		target_link_libraries("CAGasSVG"    wbsyshLightSVG  )

		add_executable("LinktestSVG"
					"EXAMPLES/Linktest.cpp"
				)

		target_link_libraries("LinktestSVG" wbsyshLightSVG  )
	else()
		target_link_libraries("TestmousSVG" wbsyshLightSVG  rt)
		target_link_libraries("TestpaleSVG" wbsyshLightSVG  rt)
		target_link_libraries("TestpolySVG" wbsyshLightSVG  rt)
		target_link_libraries("TestrandSVG" wbsyshLightSVG  rt)
		target_link_libraries("TestsymsSVG" wbsyshLightSVG  rt)
		target_link_libraries("TestsyshSVG" wbsyshLightSVG  rt)
		target_link_libraries("TurmitSVG"   wbsyshLightSVG  rt)
		target_link_libraries("CAGasSVG"    wbsyshLightSVG pthread rt)

		add_executable("LinktestSVG"
					"EXAMPLES/Linktest.cpp"
				)

		target_link_libraries("LinktestSVG" wbsyshLightSVG rt )
	endif()

endif()

#KONIEC - THE END
#********************************************************************/
#*              SYMSHELLLIGHT  version 2026                         */
#********************************************************************/
#*           THIS CODE IS DESIGNED & COPYRIGHT  BY:                 */
#*            W O J C I E C H   B O R K O W S K I                   */
#*    WWW: https://www.researchgate.net/profile/WOJCIECH_BORKOWSKI  */
#*    GITHUB: https://github.com/borkowsk                           */
#*                                                                  */
#*    Instytut Studiów Społecznych im. Profesora Roberta Zajonc  */
#*    Uniwersytetu Warszawskiego                                    */
#*    PL.WWW: http://iss.uw.edu.pl/                                 */
#*                                                                  */
#*    Robert Zajonc Institute for Social Studies                    */
#*    University of Warsaw                                          */
#*    EN.WWW: http://iss.uw.edu.pl/en/                              */
#*                                                                  */
#*                               (Don't change or remove this note) */
#********************************************************************/

