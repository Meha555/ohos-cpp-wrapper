add_library(hilogpp hilog_ostream_adaptor.cpp)
target_include_directories(hilogpp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hilogpp PUBLIC libhilog_ndk.z.so)
