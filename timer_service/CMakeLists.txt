# Timer Service Library CMakeLists.txt

# Gather source files
set(SOURCES
    timer_service.cpp
)

# Gather header files
set(HEADERS
    timer_service.h
)

# Create static library
add_library(timer_service STATIC ${SOURCES} ${HEADERS})

# Include directories (public so that consumers can include the headers)
target_include_directories(timer_service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link threads library since the timer service uses std::thread
find_package(Threads REQUIRED)
target_link_libraries(timer_service PUBLIC Threads::Threads)

# todo: add install