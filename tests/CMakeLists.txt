# Make sure it finds the headers
#include_directories (../include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include ${GNULIBS_INCLUDE_DIR})
LINK_LIBRARIES(${PROJECT_NAME}  ${REQUIRED_LIBRARIES} )

ADD_EXECUTABLE(detectobj detectobj.cpp)
INSTALL(TARGETS detectobj  RUNTIME DESTINATION bin)

ADD_EXECUTABLE(detectobj2 detectobj2.cpp)
INSTALL(TARGETS detectobj2  RUNTIME DESTINATION bin)

ADD_EXECUTABLE(detectvid detectvid.cpp)
INSTALL(TARGETS detectvid  RUNTIME DESTINATION bin)
