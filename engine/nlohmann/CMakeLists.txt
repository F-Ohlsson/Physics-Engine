#--------------------------------------------------------------------------
# render
#--------------------------------------------------------------------------

SET(files_nlohmann
	json.hpp
	)
SOURCE_GROUP("nlohmann" FILES ${files_nlohmann})

SET(files_pch ../config.h ../config.cc)
SOURCE_GROUP("pch" FILES ${files_pch})
ADD_LIBRARY(nlohmann STATIC ${files_nlohmann} ${files_pch})
TARGET_PCH(nlohmann ../)	
TARGET_LINK_LIBRARIES(nlohmann PUBLIC engine exts glew glfw imgui ${OPENGL_LIBS})