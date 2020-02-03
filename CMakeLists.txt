cmake_minimum_required(VERSION 3.2)

project(smm-webconfig-demo C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
#set(CXX_STANDARD 11)

#find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

#if(NOT WIN32)
#  find_package(X11 REQUIRED)
#endif()

#set(GXX_COMPILE_FLAGS "-fpermissive")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GXX_COMPILE_FLAGS}")

add_executable(smm-webconfig-demo src/mg/mongoose.c src/callback_map.c src/main.c)

target_link_libraries(smm-webconfig-demo ssl crypto Threads::Threads) # ${OpenCV_LIBS})

if(WIN32)
  target_link_libraries(smm-webconfig-demo ws2_32)
#else()
#  target_link_libraries(smm-webconfig-demo ${X11_LIBRARIES})
endif()       

