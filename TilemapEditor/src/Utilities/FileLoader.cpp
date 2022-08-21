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
#include "../Components/AnimationComponent.h"
#include "LuaWriter.h"

namespace fs = std::filesystem;

FileLoader::FileLoader()
{
}

FileLoader::~FileLoader()
{
}

void FileLoader::LoadProject(sol::state& lua, const std::string& filename, const AssetManager_Ptr& assetManager,
	Renderer& renderer, std::vector<std::string>& assetIds, std::vector<std::string>& assetFilepaths,
	std::shared_ptr<Canvas>& canvas,  int& tileSize)
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
		sol::optional<sol::table> hasMaps = project["maps"][mapNum];
		if (hasMaps == sol::nullopt)
		{
			LOG_INFO("FILELOADER__LINE_75: Finished loading assets");
			break;
		}

		sol::table maps = project["maps"][mapNum];
		mapFile = maps["file_path"];

		mapNum++;
	}

	sol::optional<sol::table> luaCanvas = project["canvas"];
	if (luaCanvas != sol::nullopt)
	{
		int canvasWidth = project["canvas"]["canvas_width"];
		int canvasHeight = project["canvas"]["canvas_height"];
		tileSize = project["canvas"]["tile_size"];

		canvas->SetWidth(std::move(canvasWidth));
		canvas->SetHeight(std::move(canvasHeight));
	}


	LoadMap(assetManager, mapFile);
}

void FileLoader::LoadMap(const AssetManager_Ptr& assetManager, const std::string& filename)
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
		int numFrames = 0, frameSpeed = 0, frameOffset = 0;
		bool collider = false, animated = false, vertical = false, looped = false;

		// Read the contents of the file into the temporary variables
		mapFile >> group >> assetID >> tileWidth >> tileHeight >> srcRectX >> srcRectY >> layer >> transform.x >> transform.y >> scale.x >> scale.y >> collider;

		// If the tile is also a collider, load collider data
		if (collider) 
			mapFile >> colWidth >> colHeight >> offset.x >> offset.y;

		mapFile >> animated;
		
		if (animated)
			mapFile >> numFrames >> frameSpeed >> vertical >> looped >> frameOffset;

		// Create a new entity based on the above information
		Entity tile = Registry::Instance().CreateEntity();
		tile.Group(group);
		tile.AddComponent<SpriteComponent>(assetID, tileWidth, tileHeight, layer, false, srcRectX, srcRectY);
		tile.AddComponent<TransformComponent>(transform, scale, 0.0);

		if (collider)
			tile.AddComponent<BoxColliderComponent>(colWidth, colHeight, offset);
		if (animated)
			tile.AddComponent<AnimationComponent>(numFrames, frameSpeed, vertical, looped, frameOffset);

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
		bool collider = false, animated = false;
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
			mapFile << collider << " " << boxCollider.mWidth << " " << boxCollider.mHeight << " " << boxCollider.mOffset.x << " " << boxCollider.mOffset.y << " ";// << std::endl;
		}
		else
		{
			collider = false;
			mapFile << collider << " ";// << std::endl;
		}

		// Check to see if the tile has an animation component
		if (tile.HasComponent<AnimationComponent>())
			animated = true;

		if (animated)
		{
			const auto& animation = tile.GetComponent<AnimationComponent>();
			mapFile << animated << " " << animation.mNumFrames << " " << animation.mFrameSpeedRate << " " << animation.mVertical << " " <<
				animation.mIsLooped << " " << animation.mFrameOffset << " " << std::endl;
		}
		else
		{
			animated = false;
			mapFile << animated << " " << std::endl;
		}

	}
	// Close the file
	mapFile.close();
}

void FileLoader::SaveColliders(std::filesystem::path filename)
{
	// Check to see if there are any tile entities to save to a file
	if (!Registry::Instance().DoesGroupExist("colliders"))
	{
		LOG_ERROR("FILELOADER__LINE__220: Trying to save Colliders that do not exist!");
		return;
	}

	std::ofstream mapFile;

	mapFile.open(filename);

	if (!mapFile.is_open())
	{
		LOG_ERROR("FILELOADER__LINE__230: Unable to open[{0}] for saving", filename);
		return;
	}

	auto colliders = Registry::Instance().GetEntitiesByGroup("colliders");

	for (const auto& collider : colliders)
	{
		std::string group = "collider";
		const auto& boxCollider = collider.GetComponent<BoxColliderComponent>();
		const auto& transform = collider.GetComponent<TransformComponent>();

		// Save to the map file
		mapFile << group << " " << transform.mPosition.x << " " << transform.mPosition.y << " " << transform.mScale.x << " " << transform.mScale.y 
			<< " " << boxCollider.mWidth << " " << boxCollider.mHeight << " " << boxCollider.mOffset.x << " " << boxCollider.mOffset.y << " " << std::endl;
	}
	// Close the file
	mapFile.close();
}

void FileLoader::SaveProject(const std::string& filename, std::vector<std::string>& assetIds, std::vector<std::string>& assetFilepaths,
	const int& canvasWidth, const int& canvasHeight, const int& tileSize)
{

	std::fstream projFile;
	projFile.open(filename, std::ios::out | std::ios::trunc);

	if (!projFile.is_open())
	{
		LOG_ERROR("FILELOADER__LINE__223: Unable to open[{0}] for saving", filename);
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
		luaWriter.WriteKeyAndQuotedValue("asset_id", asset, projFile, true);
		luaWriter.WriteKeyAndQuotedValue("file_path", assetFilepaths[assetNum], projFile, true);
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
		LOG_ERROR("FILELOADER__LINE__313: Unable to open[{0}] for saving", filepath.u8string());
		return;
	}

	SaveMap(filepath);

	if (!Registry::Instance().DoesGroupExist("colliders"))
		return;

	std::fstream colliderFile;
	std::string newFile = filepath.stem().string() += "_colliders.map";

	filepath.replace_filename(newFile);

	colliderFile.open(filepath, std::ios::out);
	
	if (!colliderFile.is_open())
	{
		LOG_ERROR("FILELOADER__LINE__327: Unable to open[{0}] for saving", filepath.u8string());
		return;
	}
	LOG_INFO("Collider: {0}", filepath.string());

	SaveColliders(filepath);

}

void FileLoader::SaveToLuaTable(const std::string& filename, std::vector<std::string>& assetIds, std::vector<std::string>& assetFilepaths, const int& tileSize)
{
	std::fstream projFile;
	projFile.open(filename, std::ios::out | std::ios::trunc);

	if (!projFile.is_open())
	{
		LOG_ERROR("FILELOADER__LINE__223: Unable to open[{0}] for saving", filename);
		return;
	}

	LuaWriter luaWriter;
	// Start the lua project document
	luaWriter.WriteStartDocument();

	luaWriter.WriteCommentSeparation(projFile);
	luaWriter.WriteCommentLine("", projFile);
	luaWriter.WriteCommentSeparation(projFile);

	luaWriter.WriteWords("return {", projFile, true);
	luaWriter.WriteKeyAndUnquotedValue("id", "id", projFile, false, false);
	luaWriter.WriteKeyAndUnquotedValue("name", "", projFile, false, false);
	luaWriter.WriteKeyAndUnquotedValue("tileWidth", tileSize, projFile, false, false);
	luaWriter.WriteKeyAndUnquotedValue("tileHeight", tileSize, projFile, false, false);
	luaWriter.WriteDeclareTable("on_wake", projFile);
	luaWriter.WriteEndTable(false, projFile);
	luaWriter.WriteDeclareTable("actions", projFile);
	luaWriter.WriteEndTable(false, projFile);
	luaWriter.WriteDeclareTable("trigger_types", projFile);
	luaWriter.WriteEndTable(false, projFile);
	luaWriter.WriteDeclareTable("triggers", projFile);
	luaWriter.WriteEndTable(false, projFile);
	luaWriter.WriteDeclareTable("tiles", projFile);

	if (Registry::Instance().DoesGroupExist("tiles"))
	{
		int i = 1;
		for (const auto& tile : Registry::Instance().GetEntitiesByGroup("tiles"))
		{
			luaWriter.WriteStartTable(i, false, projFile);
			luaWriter.WriteDeclareTable("components", projFile);
			if (tile.HasComponent<TransformComponent>())
			{
				const auto& transform = tile.GetComponent<TransformComponent>();

				luaWriter.WriteDeclareTable("transform", projFile);
				luaWriter.WriteDeclareTable("position", projFile);
				luaWriter.WriteKeyAndValue("x", transform.mPosition.x, false, projFile);
				luaWriter.WriteKeyAndValue("y", transform.mPosition.y, true, projFile);
				luaWriter.WriteEndTable(true, projFile);
				luaWriter.WriteDeclareTable("scale", projFile);
				luaWriter.WriteKeyAndValue("x", transform.mScale.x, false, projFile);
				luaWriter.WriteKeyAndValue("y", transform.mScale.y, true, projFile);
				luaWriter.WriteEndTable(true, projFile);
				luaWriter.WriteKeyAndUnquotedValue("rotation", transform.mRotation, projFile, false, false);
				luaWriter.WriteEndTable(false, projFile);
			}

			if (tile.HasComponent<SpriteComponent>())
			{
				const auto& sprite = tile.GetComponent<SpriteComponent>();

				std::string fixed = "false";
				
				if (sprite.mIsFixed)
					fixed = "true";

				luaWriter.WriteDeclareTable("sprite", projFile);
				luaWriter.WriteKeyAndQuotedValue("asset_id", sprite.mAssetId, projFile);
				luaWriter.WriteKeyAndValue("width", sprite.mWidth, false, projFile);
				luaWriter.WriteKeyAndValue("height", sprite.mHeight, false, projFile);
				luaWriter.WriteKeyAndValue("z_index", sprite.mLayer, false, projFile);
				luaWriter.WriteKeyAndValue("is_fixed", fixed, true, projFile);
				luaWriter.WriteDeclareTable("src_rect", projFile);
				luaWriter.WriteKeyAndValue("x", sprite.mSrcRect.x, false, projFile);
				luaWriter.WriteKeyAndValue("y", sprite.mSrcRect.y, true, projFile);
				luaWriter.WriteEndTable(true, projFile);
				luaWriter.WriteDeclareTable("offset", projFile);
				luaWriter.WriteKeyAndValue("x", sprite.mOffset.x, false, projFile);
				luaWriter.WriteKeyAndValue("y", sprite.mOffset.y, true, projFile);
				luaWriter.WriteEndTable(true, projFile);
				luaWriter.WriteEndTable(false, projFile);
			}

			if (tile.HasComponent<BoxColliderComponent>())
			{
				const auto& boxCollider = tile.GetComponent<BoxColliderComponent>();

				luaWriter.WriteDeclareTable("box_collider", projFile);
				luaWriter.WriteKeyAndValue("width", boxCollider.mWidth, false, projFile);
				luaWriter.WriteKeyAndValue("height", boxCollider.mHeight, true, projFile);
				luaWriter.WriteDeclareTable("offset", projFile);
				luaWriter.WriteKeyAndValue("x", boxCollider.mOffset.x, false, projFile);
				luaWriter.WriteKeyAndValue("y", boxCollider.mOffset.y, true, projFile);
				luaWriter.WriteEndTable(true, projFile);
				luaWriter.WriteKeyAndUnquotedValue("is_collider", "true", projFile);
				luaWriter.WriteKeyAndUnquotedValue("is_trigger", "false", projFile);
				luaWriter.WriteEndTable(false, projFile);
			}

			luaWriter.WriteEndTable(false, projFile);
			luaWriter.WriteEndTable(false, projFile);
			i++;
		}
	}

	// Loop through all the tiles 
	luaWriter.WriteEndTable(false, projFile);
	luaWriter.WriteEndTable(false, projFile);
	luaWriter.WriteEndDocument(projFile);
	luaWriter.WriteWords("end", projFile);
	projFile.close();
}

