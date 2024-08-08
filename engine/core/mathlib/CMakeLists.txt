#--------------------------------------------------------------------------
# mathlib
#--------------------------------------------------------------------------

SET(files_mathlib
	#vec3.h
	#vec4.h
	#mat4.h
	)
	
SOURCE_Group("mathlib" FILES ${files_mathlib})

SET(files_pch ../../config.h ../../config.cc)
SOURCE_GROUP("pch" FILES ${files_pch})
ADD_LIBRARY (mathlib STATIC ${files_mathlib} ${files_pch})
TARGET_PCH(mathlib ../../)
ADD_DEPENDENCIES(mathlib glew enet)
TARGET_LINK_LIBRARIES(mathlib PUBLIC engine exts glew enet soloud)