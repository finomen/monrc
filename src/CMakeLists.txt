set(SOURCES
    Service.cpp
    Runlevel.cpp
    System.cpp
    monrc.cpp
)

add_executable(monrc ${SOURCES})
target_link_libraries(monrc rc)

install(TARGETS monrc DESTINATION sbin)
