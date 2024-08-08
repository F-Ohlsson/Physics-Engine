#--------------------------------------------------------------------------
# collisiondetection project
#--------------------------------------------------------------------------

PROJECT(collisiondetection)
FILE(GLOB project_headers code/*.h)
FILE(GLOB project_sources code/*.cc)

SET(files_project ${project_headers} ${project_sources})
SOURCE_GROUP("collisiondetection" FILES ${files_project})

ADD_EXECUTABLE(collisiondetection ${files_project})
TARGET_LINK_LIBRARIES(collisiondetection core render)
ADD_DEPENDENCIES(collisiondetection core render)

IF(MSVC)
    set_property(TARGET collisiondetection PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
ENDIF()