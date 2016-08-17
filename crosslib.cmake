get_filename_component(CROSSLIB_CLIENT_CUR_DIR ${CMAKE_CURRENT_LIST_FILE} PATH) # for cmake before 2.8.3

include_directories("${CROSSLIB_CLIENT_CUR_DIR}/include/")
include_directories("${CROSSLIB_CLIENT_CUR_DIR}/port/${CROSSLIB_PORT}/")

set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -lpthread")
