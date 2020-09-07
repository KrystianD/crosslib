add_library(crosslib STATIC
        "${CMAKE_CURRENT_LIST_DIR}/port/${CROSSLIB_PORT}/CLThread.cpp")

target_include_directories(crosslib PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/")
target_include_directories(crosslib PUBLIC "${CMAKE_CURRENT_LIST_DIR}/port/${CROSSLIB_PORT}/")

if(CROSSLIB_NAMESPACE)
    target_compile_definitions(crosslib PUBLIC -DCROSSLIB_NAMESPACE=${CROSSLIB_NAMESPACE})
else()
    target_compile_definitions(crosslib PUBLIC -DCROSSLIB_NAMESPACE=crosslib)
endif()

if(CROSSLIB_LOGFUNCNAME)
    target_compile_definitions(crosslib PUBLIC -DCROSSLIB_LOGFUNCNAME=${CROSSLIB_LOGFUNCNAME})
else()
    target_compile_definitions(crosslib PUBLIC -DCROSSLIB_LOGFUNCNAME=crosslib_on_error)
endif()



