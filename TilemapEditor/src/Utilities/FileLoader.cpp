#include "FileLoader.h"
#include "../AssetManager.h"
#include <SDL.h>
#include <fstream>
#include <vector>
#include <filesystem>

#include "../Logger/Logger.h"
#include <glm/glm.hpp>
#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "LuaWriter.h"

namespace fs = std::filesystem;

FileLoader::FileLoader()
{
}

FileLoader::~FileLoader()
{
}

void FileLoader::LoadProject(sol::state& lua, const std::string& filename, const std::unique_ptr<AssetManager>& assetManager,
	std::unique_ptr<SDL_Renderer, Util::SDLDestroyer>& renderer, std::vector<std::string>& assetIds, std::vector<std::string>& assetFilepaths, 
	int& canvasWidth, int& canvasHeight, int& tileSize)
{
	/*
		Things that need to be set by load project:
			- TileSize
			- Canvas Width
			- Canvas Height
			- tilesets --> could be more than one -->ID and file Location
				- These need to be loaded into a vector<std::string>
			- tilemaps --> File Location

	*/

	fs::path filepath(filename);

	if (filepath.extension() != ".lua")
	{
		LOG_ERROR("FILELOADER__LINE__44: Project File must be a [.lua] file!");
		// TODO: ADD ERROR MSG TO IMGUI
		return;
	}

	sol::load_result script = lua.load_file(filename);

	// Check to see if the script is a valid Lua file
	if (!script.valid())
	{
		sol::error err = script;
		std::string errorMsg = err.what();
		LOG_ERROR("FILELOADER__LINE__55: Error loading the Lua Script -- " + errorMsg);
		// TODO: ADD ERROR MSG TO IMGUI
		return;
	}

	// Execute the script
	lua.script_file(filename);

	sol::table project = lua["project"];
	int assetNum = 0;
	int mapNum = 0;

	std::string mapFile = "";

	while (true)
	{
		sol::optional<sol::table> hasAssets = project["assets"][assetNum];
		if (hasAssets == sol::nullopt)
		{
			LOG_INFO("FILELOADER__LINE_75: Finished loading assets");
			break;
		}

		sol::table assets = project["assets"][assetNum];
		std::string assetId = assets["asset_id"];
		std::string file_path = assets["file_path"];

		assetIds.push_back(assetId);
		assetFilepaths.push_back(file_path);

		assetManager->AddTexture(renderer, std::move(assetId), std::move(file_path));
		
		assetNum++;
	}

	while (true)
	{
		sol::optional<sol::table> hasMaps= project["maps"][mapNum];
		if (hasMaps == sol::nullopt)
		{
			LOG_INFO("FILELOADER__LINE_75: Finished loading assets");
			break;
		}

		sol::table maps = project["maps"][mapNum];
		mapFile = maps["file_path"];

		mapNum++;
	}

	sol::optional<sol::table> canvas = project["canvas"];
	if (canvas != sol::nullopt)
	{
		canvasWidth = project["canvas"]["canvas_width"];
		canvasHeight = project["canvas"]["canvas_height"];
		tileSize = project["canvas"]["tile_size"];
	}

	LoadMap(assetManager, mapFile);
}

void FileLoader::LoadMap(const std::unique_ptr<AssetManager>& assetManager, const std::string& filename)
{
	// Open and read the tilemap
	std::fstream mapFile;
	mapFile.open(filename);

	if (!mapFile.is_open())
	{
		LOG_ERROR("FILELOADER__LINE__37: Unable to open[{0}] for loading", filename);
		return;
	}

	// Loop through the tiles and add them to the registry
	while (true)
	{
		int srcRectX = 0, srcRectY = 0, layer = 0, colWidth = 0, colHeight = 0, tileWidth = 0, tileHeight = 0;
		glm::vec2 transform = glm::vec2(0, 0);
		glm::vec2 scale = glm::vec2(1, 1);
		glm::vec2 offset = glm::vec2(1, 1);
		std::string group = "", assetID = "";
		bool collider = false;

		// Read the contents of the file into the temporary variables
		mapFile >> group >> assetID >> tileWidth >> tileHeight >> srcRectX >> srcRectY >> layer >> transform.x >> transform.y >> scale.x >> scale.y >> collider;

		// If the tile is also a collider, load collider data
		if (collider) mapFile >> colWidth >> colHeight >> offset.x >> offset.y;

		// Create a new entity based on the above information
		Entity tile = Registry::Instance().CreateEntity();
		tile.Group(group);
		tile.AddComponent<SpriteComponent>(assetID, tileWidth, tileHeight, layer, false, srcRectX, srcRectY);
		tile.AddComponent<TransformComponent>(transform, scale, 0.0);
		
		if (collider)
			tile.AddComponent<BoxColliderComponent>(colWidth, colHeight, offset);

		// Check for end of file
		if (mapFile.eof())
			break;
	}

	// Close the file
	mapFile.close();
}

void FileLoader::SaveMap(std::filesystem::path filename)
{
	// Check to see if there are any tile entities to save to a file
	if (!Registry::Instance().DoesGroupExist("tiles"))
	{
		LOG_ERROR("FILELOADER__LINE__85: Trying to save entities that do not exist!");
		return;
	}

	std::ofstream mapFile;

	mapFile.open(filename);

	if (!mapFile.is_open())
	{
		LOG_ERROR("FILELOADER__LINE__97: Unable to open[{0}] for saving", filename);
		return;
	}

	auto tiles = Registry::Instance().GetEntitiesByGroup("tiles");

	for (const auto& tile : tiles)
	{
		bool collider = false;
		std::string group = "tiles";
		const auto& sprite = tile.GetComponent<SpriteComponent>();
		const auto& transform = tile.GetComponent<TransformComponent>();

		// Save to the map file
		mapFile << group << " " << sprite.mAssetId << " " << sprite.mWidth << " " << sprite.mHeight << " " << sprite.mSrcRect.x << " " << sprite.mSrcRect.y << " " << sprite.mLayer << " " <<
			transform.mPosition.x << " " << transform.mPosition.y << " " << transform.mScale.x << " " << transform.mScale.y << " ";

		// Check to see if the tile has a collider component
		if (tile.HasComponent<BoxColliderComponent>())
			collider = true;

		if (collider)
		{
			const auto& boxCollider = tile.GetComponent<BoxColliderComponent>();
			mapFile << collider << " " << boxCollider.mWidth << " " << boxCollider.mHeight << " " << boxCollider.mOffset.x << " " << boxCollider.mOffset.y << " " << std::endl;
		}
		else
		{
			collider = false; 
			mapFile << collider << " " << std::endl;
		}
	}
	// Close the file
	mapFile.close();
}

void FileLoader::SaveProject(const std::string& filename, std::vector<std::string>& assetIds, std::vector<std::string>& assetFilepaths, int& canvasWidth, int& canvasHeight, int& tileSize)
{
	
	std::fstream projFile;
	projFile.open(filename);

	if (!projFile.is_open())
	{
		LOG_ERROR("FILELOADER__LINE__147: Unable to open[{0}] for saving", filename);
		return;
	}

	LuaWriter luaWriter;
	// Start the lua project document
	luaWriter.WriteStartDocument();
	
	luaWriter.WriteCommentSeparation(projFile);
	luaWriter.WriteCommentLine("", projFile);
	luaWriter.WriteCommentSeparation(projFile);
	int assetNum = 0;
	luaWriter.WriteDeclareTable("project", projFile);
	luaWriter.WriteDeclareTable("assets", projFile);
	for (const auto& asset : assetIds)
	{
		luaWriter.WriteStartTable(assetNum, false, projFile);
		luaWriter.WriteKeyAndQuotedValue("asset_id", asset, projFile);
		luaWriter.WriteKeyAndQuotedValue("file_path", assetFilepaths[assetNum], projFile);
		luaWriter.WriteEndTable(false, projFile);
		assetNum++;
	}
	// End that table
	luaWriter.WriteEndTable(false, projFile);

	fs::path filepath(filename);
	filepath.replace_extension(".map");

	luaWriter.WriteDeclareTable("maps", projFile);
	luaWriter.WriteStartTable(0, false, projFile);
	luaWriter.WriteKeyAndQuotedValue("file_path", filepath.string(), projFile);
	luaWriter.WriteEndTableWithSeparator(false, projFile);
	luaWriter.WriteEndTable(false, projFile);
	
	luaWriter.WriteDeclareTable("canvas", projFile);
	luaWriter.WriteKeyAndUnquotedValue("canvas_width", canvasWidth, projFile, false, false);
	luaWriter.WriteKeyAndUnquotedValue("canvas_height", canvasHeight, projFile, false, false);
	luaWriter.WriteKeyAndUnquotedValue("tile_size", tileSize, projFile, false, true);

	luaWriter.WriteEndTable(false, projFile);
	luaWriter.WriteEndTable(false, projFile);
	
	// Check to see if there are no indents remaining
	luaWriter.WriteEndDocument(projFile);

	// Close the project file
	projFile.close();

	std::fstream mapFile;



	mapFile.open(filepath, std::ios::out);

	if (!mapFile.is_open())
	{
		LOG_ERROR("FILELOADER__LINE__187: Unable to open[{0}] for saving", filepath.u8string());
		return;
	}

	SaveMap(filepath);
}

