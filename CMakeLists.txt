project(lenzhound)
cmake_minimum_required(VERSION 2.8.7)

set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
set(CMAKE_CXX_FLAGS_RELEASE "/MTd")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
include_directories(tools/gtest/include Rxr/)
add_subdirectory(tools/gtest)
add_subdirectory(Rxr)
add_subdirectory(test)

set(BOARD arduino:avr:leonardo)

SET(PORT_DESC "USB port to connect to Arduino unit")
if(WIN32)
	set(PORT com3 CACHE STRING PORT_DESC)
else(WIN32)
	set(PORT /dev/ttyACM0 CACHE STRING PORT_DESC)
endif(WIN32)

set(SKETCHBOOK ${CMAKE_SOURCE_DIR})
set(OPTS 
	--board ${BOARD} --port ${PORT} --pref sketchbook.path=${SKETCHBOOK})

add_custom_target(verify-rxr
	arduino --verify ${OPTS} ${CMAKE_SOURCE_DIR}/Rxr/Rxr.ino)
add_custom_target(verify-txr
	arduino --verify ${OPTS} ${CMAKE_SOURCE_DIR}/Txr/Txr.ino)
add_custom_target(upload-rxr
	arduino --upload ${OPTS} ${CMAKE_SOURCE_DIR}/Rxr/Rxr.ino)
add_custom_target(upload-txr
	arduino --upload ${OPTS} ${CMAKE_SOURCE_DIR}/Txr/Txr.ino)
add_custom_target(run-tests
	./test/tests
	DEPENDS tests)

# folders for Visual Studio
set_target_properties(gtest PROPERTIES FOLDER tools)
set_target_properties(gtest_main PROPERTIES FOLDER tools)
set_target_properties(tests PROPERTIES FOLDER test)
set_target_properties(run-tests PROPERTIES FOLDER test)
set_target_properties(upload-rxr PROPERTIES FOLDER board-rxr)
set_target_properties(verify-rxr PROPERTIES FOLDER board-rxr)
set_target_properties(upload-txr PROPERTIES FOLDER board-txr)
set_target_properties(verify-txr PROPERTIES FOLDER board-txr)
