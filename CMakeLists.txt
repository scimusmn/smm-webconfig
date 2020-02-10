cmake_minimum_required(VERSION 3.2)

project(smm-webconfig-demo C CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

find_package(Threads REQUIRED)

add_executable(smm-webconfig-demo src/mg/mongoose.c src/smmServer.cpp src/main.cpp)

target_link_libraries(smm-webconfig-demo ssl crypto Threads::Threads) # ${OpenCV_LIBS})

if(WIN32)
  target_link_libraries(smm-webconfig-demo ws2_32)
endif()       
