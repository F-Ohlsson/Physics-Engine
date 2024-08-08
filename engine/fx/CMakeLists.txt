#--------------------------------------------------------------------------
# render
#--------------------------------------------------------------------------

SET(files_fx
	gltf.h
	)
SOURCE_GROUP("fx" FILES ${files_fx})

SET(files_pch ../config.h ../config.cc)
SOURCE_GROUP("pch" FILES ${files_pch})
ADD_LIBRARY(fx STATIC ${files_fx} ${files_pch})
TARGET_PCH(fx ../)	
TARGET_LINK_LIBRARIES(fx PUBLIC engine exts glew glfw imgui ${OPENGL_LIBS})