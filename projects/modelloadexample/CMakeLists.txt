#--------------------------------------------------------------------------
# modelload project
#--------------------------------------------------------------------------

PROJECT(modelload)
FILE(GLOB project_headers code/*.h)
FILE(GLOB project_sources code/*.cc)

SET(files_project ${project_headers} ${project_sources})
SOURCE_GROUP("modelload" FILES ${files_project})

ADD_EXECUTABLE(modelload ${files_project})
TARGET_LINK_LIBRARIES(modelload core render)
ADD_DEPENDENCIES(modelload core render)

IF(MSVC)
    set_property(TARGET modelload PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
ENDIF()