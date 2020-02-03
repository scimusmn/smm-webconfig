cmake_minimum_required(VERSION 3.2)

project(smm-webconfig-demo C)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

find_package(Threads REQUIRED)

add_executable(smm-webconfig-demo src/mg/mongoose.c src/smm_server.c src/callback_map.c src/main.c)

target_link_libraries(smm-webconfig-demo ssl crypto Threads::Threads) # ${OpenCV_LIBS})

if(WIN32)
  target_link_libraries(smm-webconfig-demo ws2_32)
endif()       

file(COPY web_root DESTINATION bin/)
