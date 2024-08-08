#--------------------------------------------------------------------------
# core
#--------------------------------------------------------------------------

SET(files_core
	app.h
	app.cc
	debug.cc
	debug.h
	random.h
	random.cc
	cvar.h
	cvar.cc
	idpool.h
	)

	
SOURCE_GROUP("core" FILES ${files_core})

SET(files_pch ../config.h ../config.cc)
SOURCE_GROUP("pch" FILES ${files_pch})
ADD_LIBRARY(core STATIC ${files_core} ${files_pch})
TARGET_PCH(core ../)
ADD_DEPENDENCIES(core glew enet)
TARGET_LINK_LIBRARIES(core PUBLIC physicscourse graphicscourse engine exts glew enet soloud)

ADD_SUBDIRECTORY(graphicscourse)
ADD_SUBDIRECTORY(physicscourse)
SET_TARGET_PROPERTIES(graphicscourse PROPERTIES FOLDER "engine")
SET_TARGET_PROPERTIES(physicscourse PROPERTIES FOLDER "engine")