cmake_minimum_required (VERSION 3.0.0)
project(abxhttpd)
set(CMAKE_CXX_STANDARD 11)
include_directories(.)
find_package(Threads REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(ABXHTTPD_LIB "${CMAKE_THREAD_LIBS_INIT}")
if (CMAKE_HOST_WIN32)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MP /source-charset:utf-8 /execution-charset:gbk")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MP /source-charset:utf-8 /execution-charset:gbk")
	link_libraries(ws2_32)
	list(APPEND RES_FILES Resource/Windows/abxhttpd.rc)
	list(APPEND RES_FILES_LIB Resource/Windows/xhttpd.rc)
endif()

aux_source_directory(Httpd SOURCE_FILES)
aux_source_directory(Parser SOURCE_FILES)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -lc++")
endif()

if(DEFINED ABXHTTPD_SSL)
	add_definitions(-DABXHTTPD_SSL=${ABXHTTPD_SSL})
	set(OPENSSL_USE_STATIC_LIBS TRUE)
	find_package(OpenSSL REQUIRED)
	list(APPEND ABXHTTPD_LIB OpenSSL::SSL)
	list(APPEND SOURCE_FILES Extension/SSL.cpp Extension/Hash.cpp)
	install(FILES include/hash.H DESTINATION include/abxhttpd)
endif()

if(DEFINED ABXHTTPD_SQLITE3)
	add_definitions(-DABXHTTPD_SQLITE3=${ABXHTTPD_SQLITE3})
	set(CMAKE_FIND_STATIC FIRST)
	find_package(SQLite3 REQUIRED)
	if(SQLITE3_FOUND)
		include_directories(${SQLite3_INCLUDE_DIRS})
		list(APPEND ABXHTTPD_LIB ${SQLite3_LIBRARIES})
		list(APPEND SOURCE_FILES Extension/sqlite_3.cpp)
		install(FILES include/sqlite_3.H DESTINATION include/abxhttpd)
	endif(SQLITE3_FOUND)
	set(CMAKE_FIND_STATIC LAST)
endif()

list(APPEND SOURCE_FILES Extension/random.cpp)

set(ABXHTTPD_TARGET abxhttpd)
if(DEFINED ABXHTTPD_STATIC)
add_definitions(-DABXHTTPD_STATIC=${ABXHTTPD_STATIC})
include_directories(${OPENSSL_INCLUDE_DIR})
add_library(xhttpd_static STATIC ${SOURCE_FILES})
list(APPEND ABXHTTPD_TARGET xhttpd_static)
endif()
if(DEFINED ABXHTTPD_SHARED)
	set(CMAKE_SKIP_BUILD_RPATH  FALSE)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
	#set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
	set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
	set(INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}")
	add_definitions(-DABXHTTPD_SHARED=${ABXHTTPD_SHARED})
	add_library(xhttpd SHARED ${SOURCE_FILES} ${RES_FILES_LIB})
	add_executable(abxhttpd CLI.cpp ${RES_FILES})
	list(APPEND ABXHTTPD_TARGET xhttpd)
	target_link_libraries(xhttpd ${ABXHTTPD_LIB})
	target_link_libraries(abxhttpd xhttpd)
else()
	add_definitions(-DABXHTTPD_CLI)
	add_executable(abxhttpd CLI.cpp ${SOURCE_FILES} ${RES_FILES})
	target_link_libraries(abxhttpd ${ABXHTTPD_LIB})
endif()

install(
	TARGETS ${ABXHTTPD_TARGET} 
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
)

install(FILES include/abxhttpd.H DESTINATION include/abxhttpd)
install(DIRECTORY include/ DESTINATION include/abxhttpd PATTERN "*.H" EXCLUDE)
install(DIRECTORY Resource/man/ TYPE MAN)
install(DIRECTORY Resource/doc/ TYPE DOC)

if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
