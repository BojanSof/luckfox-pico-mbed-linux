cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER "arm-rockchip830-linux-uclibcgnueabihf-gcc"
)
set(CMAKE_CXX_COMPILER "arm-rockchip830-linux-uclibcgnueabihf-g++"
)

set(PROJECT_NAME object-detection)
project(${PROJECT_NAME})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

set(APP_DIR yolov8_rtsp)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ 3rdparty.out)

set(CMAKE_INSTALL_RPATH "${ORIGIN}/lib")
add_definitions(-DRV1106_1103)

#Opencv 4
set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)
#Thread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SRC_DIR "${APP_DIR}/src")
file(GLOB SRC_FILES "${SRC_DIR}/*.cc")
add_executable(${PROJECT_NAME} ${SRC_FILES})

add_compile_options(-g -Wall
                    -DISP_HW_V30 -DRKPLATFORM=ON -DARCH64=OFF
                    -DROCKIVA -DUAPI2
                    -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 
                    )

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link,${CMAKE_CURRENT_SOURCE_DIR}/lib:/usr/lib")

target_link_libraries(${PROJECT_NAME}  
                    ${OpenCV_LIBS}
                    ${LIBRKNNRT}
                    Threads::Threads
                    rockiva
                    sample_comm
                    rockit
                    rockchip_mpp
                    rkaiq
                    pthread
                    rtsp
                    )

target_include_directories(${PROJECT_NAME} PRIVATE                        
                            ${OpenCV_INCLUDE_DIRS}
                            ${APP_DIR}/include
                            ${CMAKE_CURRENT_SOURCE_DIR}
                            ${CMAKE_CURRENT_SOURCE_DIR}/utils
                            ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/rknpu2/include
                            ${CMAKE_CURRENT_SOURCE_DIR}/common 
                            ${CMAKE_CURRENT_SOURCE_DIR}/common/isp3.x   
                            ${CMAKE_CURRENT_SOURCE_DIR}/include
                            ${CMAKE_CURRENT_SOURCE_DIR}/include/rknn
                            ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq
                            ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/uAPI2
                            ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/common
                            ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/xcore
                            ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/algos
                            ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/iq_parser
                            ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/iq_parser_v2
                            ${CMAKE_CURRENT_SOURCE_DIR}/include/rkaiq/smartIr
                          )
                          

message(STATUS "model dir PATH: ${APP_DIR}/model")

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install/${PROJECT_NAME}_demo")
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX})
SET(MODEL_DIR "${APP_DIR}/model")
file(GLOB MODEL_FILES "${MODEL_DIR}/*")  
install(FILES ${MODEL_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/model)                    
