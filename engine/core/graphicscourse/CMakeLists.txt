#--------------------------------------------------------------------------
# graphicscourse
#--------------------------------------------------------------------------

SET(files_graphicscourse
	camera/Camera.h
	camera/Camera.cc
	dirLight/DirLight.h
	dirLight/DirLight.cc
	graphicsNode/GraphicsNode.h
	graphicsNode/GraphicsNode.cc
	inputHandler/InputHandler.h
	inputHandler/InputHandler.cc
	lightManager/LightManager.h
	lightManager/LightManager.cc
	material/Material.h
	material/Material.cc
	material/blinnPhongMaterial/BlinnPhongMaterial.h
	material/blinnPhongMaterial/BlinnPhongMaterial.cc
	material/defaultMaterial/DefaultMaterial.h
	material/defaultMaterial/DefaultMaterial.cc
	meshResource/MeshResource.h
	meshResource/MeshResource.cc
	pointLight/PointLight.h
	pointLight/PointLight.cc
	shaderResource/ShaderResource.h
	shaderResource/ShaderResource.cc
	textureResource/TextureResource.h
	textureResource/TextureResource.cc
	)
SOURCE_GROUP("graphicscourse" FILES ${files_graphicscourse})
	
SET(files_pch ../../config.h ../../config.cc)
SOURCE_GROUP("pch" FILES ${files_pch})
ADD_LIBRARY(graphicscourse STATIC ${files_graphicscourse} ${files_pch})
TARGET_PCH(graphicscourse ../../)
ADD_DEPENDENCIES(graphicscourse glew enet)
TARGET_LINK_LIBRARIES(graphicscourse PUBLIC engine core exts glew enet soloud)

