cmake_minimum_required(VERSION 3.2)

project(test C CXX)
set(CXX_STANDARD 11)

#find_package(OpenCV REQUIRED)
#find_package(Threads REQUIRED)

#if(NOT WIN32)
#  find_package(X11 REQUIRED)
#endif()

set(GXX_COMPILE_FLAGS "-fpermissive")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GXX_COMPILE_FLAGS}")

add_executable(test mg/mongoose.c b64/base64.c mgServer.cpp main.cpp)

target_link_libraries(test ssl crypto) # Threads::Threads ${OpenCV_LIBS})

if(WIN32)
  target_link_libraries(test ws2_32)
#else()
#  target_link_libraries(test ${X11_LIBRARIES})
endif()       

