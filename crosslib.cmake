get_filename_component(CROSSLIB_CUR_DIR ${CMAKE_CURRENT_LIST_FILE} PATH) # for cmake before 2.8.3

include_directories("${CROSSLIB_CUR_DIR}/include/")
include_directories("${CROSSLIB_CUR_DIR}/port/${CROSSLIB_PORT}/")

if(CROSSLIB_PORT STREQUAL "linux")
	set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -lpthread -lrt")
endif()

set(COMMON_SOURCES ${COMMON_SOURCES}
	"${CROSSLIB_CUR_DIR}/port/${CROSSLIB_PORT}/CLThread.cpp"
)

if(CROSSLIB_NAMESPACE)
	add_definitions(-DCROSSLIB_NAMESPACE=${CROSSLIB_NAMESPACE})
else()
	add_definitions(-DCROSSLIB_NAMESPACE=crosslib)
endif()

if(CROSSLIB_LOGFUNCNAME)
	add_definitions(-DCROSSLIB_LOGFUNCNAME=${CROSSLIB_LOGFUNCNAME})
else()
	add_definitions(-DCROSSLIB_LOGFUNCNAME=crosslib_on_error)
endif()
