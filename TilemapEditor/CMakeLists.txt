add_executable(TilemapEditor
    "src/Components/AnimationComponent.h"
    "src/Components/BoxColliderComponent.h"
    "src/Components/SpriteComponent.h"
    "src/Components/TransformComponent.h"
	
    "src/ECS/ECS.h"
    "src/ECS/ECS.cpp"
	
    "src/Logger/Logger.h"
    "src/Logger/Logger.cpp"
    
	"src/Rendering/AnimationSystem.h"
	"src/Rendering/AnimationSystem.cpp"
	"src/Rendering/IconsFontAwesome.h"
	"src/Rendering/ImGuiFunc.h"
	"src/Rendering/ImGuiFunc.cpp"
	"src/Rendering/RenderableEntity.h"
	"src/Rendering/RenderCollisionSystem.h"
	"src/Rendering/RenderCollisionSystem.cpp"
	"src/Rendering/RenderImGui.h"
	"src/Rendering/RenderImGui.cpp"
	"src/Rendering/RenderSystem.h"
	"src/Rendering/RenderSystem.cpp"
	
	"src/Utilities/AddTileCommand.h"
	"src/Utilities/AddTileCommand.cpp"
	"src/Utilities/ChangeCanvasSizeCommand.h"
	"src/Utilities/ChangeCanvasSizeCommand.cpp"
	"src/Utilities/CommandManager.h"
	"src/Utilities/CommandManager.cpp"
	"src/Utilities/FileDialog.h"
	"src/Utilities/FileDialog.cpp"
	"src/Utilities/FileLoader.h"
	"src/Utilities/FileLoader.cpp"
	"src/Utilities/ICommand.h"
	"src/Utilities/LuaWriter.h"
	"src/Utilities/LuaWriter.cpp"
	"src/Utilities/RemoveTileCommand.h"
	"src/Utilities/RemoveTileCommand.cpp"
	"src/Utilities/Utilities.h"
	
	"src/Application.h"
	"src/Application.cpp"
	"src/AssetManager.h"
	"src/AssetManager.cpp"
	"src/Canvas.h"
	"src/main.cpp"
	"src/MouseControl.h"
	"src/MouseControl.cpp")

target_include_directories(
    TilemapEditor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(TilemapEditor PRIVATE nfd imgui SDL2::SDL2 
	$<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
	$<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>)
	
target_link_libraries(TilemapEditor PUBLIC sol2 glm::glm)

add_resources(TARGET Assets IN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets OUT_DIR assets)
add_dependencies(TilemapEditor Copy-Assets)

set_target_properties(TilemapEditor PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")