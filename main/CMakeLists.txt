FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/main/*.*)
idf_component_register(SRCS ${app_sources}
                    INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include
                    REQUIRES freertos jomjol_tfliteclass jomjol_helper sdmmc fatfs)

                
