#message(AUTHOR_WARNING "OHOS API version must be at least 18.")

add_library(ddk_base INTERFACE base.h)
target_link_libraries(ddk_base INTERFACE libddk_base.z.so nlohmann_json::nlohmann_json)
target_include_directories(ddk_base INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
add_library(DDK::base ALIAS ddk_base)

add_subdirectory(usb)

#add_subdirectory(libusb-adapter)