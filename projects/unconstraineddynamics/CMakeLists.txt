#--------------------------------------------------------------------------
# unconstraineddynamics project
#--------------------------------------------------------------------------

PROJECT(unconstraineddynamics)
FILE(GLOB project_headers code/*.h)
FILE(GLOB project_sources code/*.cc)

SET(files_project ${project_headers} ${project_sources})
SOURCE_GROUP("unconstraineddynamics" FILES ${files_project})

ADD_EXECUTABLE(unconstraineddynamics ${files_project})
TARGET_LINK_LIBRARIES(unconstraineddynamics core render)
ADD_DEPENDENCIES(unconstraineddynamics core render)

IF(MSVC)
    set_property(TARGET unconstraineddynamics PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
ENDIF()