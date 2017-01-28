cmake_minimum_required(VERSION 3.6 FATAL_ERROR)
set(CMAKE_VERBOSE_MAKEFILE ON)
project(Cmaketest)

if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

if(POLICY CMP0066)
  cmake_policy(SET CMP0066 NEW)
endif()

# Default build type if none was specified.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  message(STATUS "Setting build type to '${CMAKE_BUILD_TYPE} as none was specified.")

  # Possible values of build type for CMake GUI.
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# The path to extra modules.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Setup build locations.
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()
set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/build)

# See rationale at https://crascit.com/2015/03/28/enabling-cxx11-in-cmake/.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
	# Compiles with all cores in Windows.
	add_compile_options(/MP)
endif()

include(ExternalProject)
include(ProcessorCount)
ProcessorCount(Processor_Count)
if(NOT Processor_Count EQUAL 0)
   set(CMAKE_BUILD_FLAGS -j${Processor_Count})
endif()

# http://stackoverflow.com/questions/6646405/how-do-you-add-boost-libraries-in-cmakelists-txt
add_definitions(-DBOOST_ALL_NO_LIB)
# set(BOOST_VERSION 1.63.0)
# Boost related settings: https://cmake.org/cmake/help/v3.6/module/FindBoost.html.
# set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
#TODO: Probably a Windows path...
find_package(Boost)
if(NOT Boost_Found)
	# The Boost version this project uses is the preferred one.
	# It shouldn't hurt to enable extensive diagnostics, just in case.
	# Also, a different set of files is downloaded for UNIX and WIN32
	# due to file permissions and line-feeds (Windows should handle
	# also Unix style line-feeds).
	set(Boost_DETAILED_FAILURE_MSG on)
	add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})

	if(UNIX)
		set(Boost_Url http://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.gz)
		set(Boost_Sha1 2cecf1848a813de55e5770f324f084c568abca0a)
		set(Boost_Bootstrap_Command ./bootstrap.sh)
		set(Boost_b2_Command ./b2)
	elseif(WIN32)
		set(Boost_Url http://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.zip)
		set(Boost_Sha1 4364989afbe6b11f2d5e59df902c3ca4d7851824)
		set(Boost_Bootstrap_Command bootstrap.bat)
		set(Boost_b2_Command b2.exe)
	endif()

	set(Config_Libraries chrono,filesystem,program_options,system,thread,test)	
	ExternalProject_Add(libboost
		TMP_DIR ${Cmaketest_SOURCE_DIR}/temp
		DOWNLOAD_DIR ${Cmaketest_SOURCE_DIR}/downloads
		URL ${Cmaketest_SOURCE_DIR}/downloads/boost_1_63_0.zip
		URL_HASH SHA1=${Boost_Sha1}
		SOURCE_DIR ${Cmaketest_SOURCE_DIR}/depends/boost_1_63_0	
		BUILD_IN_SOURCE true
		UPDATE_COMMAND ""
		PATCH_COMMAND ""
		CONFIGURE_COMMAND ${Boost_Bootstrap_Command} --without-icu --with_libraries=${Config_Libraries}
		BUILD_COMMAND  ${Boost_b2_Command}
			#--prefix=${boost-download_SOURCE_DIR}/depends/boost_1_63_0/lib/stage
			--without-python
			--address-model=64
			--architecture=x64
			--threading=multi
			--link=static
			--variant=complete			
			--build-type=complete
			-j${Processor_Count}
		INSTALL_COMMAND ${Boost_b2_Command}
			--prefix=${Cmaketest_SOURCE_DIR}/depends/boost_1_63_0/lib/stage
			--without-python
			--address-model=64
			--architecture=x64
			--threading=multi
			--link=static
			--variant=complete			
			--build-type=complete
			-j${Processor_Count} install		
		INSTALL_DIR ${Cmaketest_SOURCE_DIR}/depends/boost_1_63_0
	)
endif()


include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(PRIVATE ${Cmaketest_SOURCE_DIR}/depends/boost_1_63_0)

# Grabbing just all the test project files.
file(GLOB SOURCES "src/*.*")
add_executable(cmaketest ${SOURCES})
target_link_libraries(cmaketest PRIVATE ${Cmaketest_SOURCE_DIR}/depends/boost_1_63_0/stage/lib)
