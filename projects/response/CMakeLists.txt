#--------------------------------------------------------------------------
# response project
#--------------------------------------------------------------------------

PROJECT(response)
FILE(GLOB project_headers code/*.h)
FILE(GLOB project_sources code/*.cc)

SET(files_project ${project_headers} ${project_sources})
SOURCE_GROUP("response" FILES ${files_project})

ADD_EXECUTABLE(response ${files_project})
TARGET_LINK_LIBRARIES(response core render)
ADD_DEPENDENCIES(response core render)

IF(MSVC)
    set_property(TARGET response PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
ENDIF()