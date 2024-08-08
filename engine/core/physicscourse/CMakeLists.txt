#--------------------------------------------------------------------------
# physicscourse
#--------------------------------------------------------------------------

SET(files_physicscourse
	CollisionSystem.cc
	CollisionSystem.h
	ForceApplier.cc
	ForceApplier.h
	PhysicsNode.cc
	PhysicsNode.h
	PhysicsObject.cc
	PhysicsObject.h
	PhysicsWorld.cc
	PhysicsWorld.h
	Picker.cc
	Picker.h
	Plane.cc
	Plane.h
	Ray.cc
	Ray.h
	)
	
SOURCE_Group("physicscourse" FILES ${files_physicscourse})

SET(files_pch ../../config.h ../../config.cc)
SOURCE_GROUP("pch" FILES ${files_pch})
ADD_LIBRARY (physicscourse STATIC ${files_physicscourse} ${files_pch})
TARGET_PCH(physicscourse ../../)
ADD_DEPENDENCIES(physicscourse glew enet)
TARGET_LINK_LIBRARIES(physicscourse PUBLIC engine exts glew enet soloud)