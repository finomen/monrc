set(SOURCES
    Service.cpp
    Runlevel.cpp
    monrc.cpp
)

add_executable(monrc ${SOURCES})
target_link_libraries(monrc rc)    