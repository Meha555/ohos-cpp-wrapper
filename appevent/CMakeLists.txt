add_library(appevent INTERFACE error.h appevent.h)
target_link_libraries(appevent INTERFACE libhiappevent_ndk.z.so)
target_include_directories(appevent INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
add_library(appevent::appevent ALIAS appevent)