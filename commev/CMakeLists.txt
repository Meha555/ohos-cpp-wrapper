add_library(commev INTERFACE error.h event.h)
target_link_libraries(commev INTERFACE libohcommonevent.so)
target_include_directories(commev INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
add_library(common::event ALIAS commev)