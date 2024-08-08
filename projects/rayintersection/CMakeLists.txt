#--------------------------------------------------------------------------
# rayintersect project
#--------------------------------------------------------------------------

PROJECT(rayintersect)
FILE(GLOB project_headers code/*.h)
FILE(GLOB project_sources code/*.cc)

SET(files_project ${project_headers} ${project_sources})
SOURCE_GROUP("rayintersect" FILES ${files_project})

ADD_EXECUTABLE(rayintersect ${files_project})
TARGET_LINK_LIBRARIES(rayintersect core render)
ADD_DEPENDENCIES(rayintersect core render)

IF(MSVC)
    set_property(TARGET rayintersect PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
ENDIF()