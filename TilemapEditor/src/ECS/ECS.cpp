#include "ECS.h"
#include <algorithm>

// Initialize Statics
int BaseComponent::nextID = 0;
//Registry* Registry::instance = nullptr;
std::unique_ptr<Registry> Registry::instance = nullptr;

int Entity::GetID() const
{
	return ID;
}

void Entity::Kill()
{
	registry->KillEntity(*this);
}

void Entity::Tag(const std::string& tag)
{
	registry->TagEntity(*this, tag);
}

bool Entity::HasTag(const std::string& tag) const
{
	return registry->EntityHasTag(*this, tag);
}

void Entity::Group(const std::string& group)
{
	registry->GroupEntity(*this, group);
}

bool Entity::BelongsToGroup(const std::string& group) const
{
	return registry->EntityBelongsToGroup(*this, group);
}

void System::AddEntityToSystem(Entity entity)
{
	entities.push_back(entity);
}

void System::RemoveEntityFromSystem(Entity entity)
{
	// This uses the functions of the vector class. It contains a Lambda function that looks for a 
	// match amd then removes it
	entities.erase(std::remove_if(entities.begin(), entities.end(), [&entity](Entity otherEntity)
		{
			return entity == otherEntity;
		}),
		entities.end());
}

std::vector<Entity> System::GetSystemEntities() const
{
	return entities;
}

const Signature& System::GetComponentSignature() const
{
	return componentSignature;
}

Entity Registry::CreateEntity()
{

	int entityID;

	if (freeIDs.empty())
	{
		// If there are no freeIDs to be used
		entityID = numEntities++;

		if (entityID >= entityComponentSignatures.size())
		{
			entityComponentSignatures.resize(entityID + 1);
		}
	}
	else
	{
		//Reuse an ID from the list of previously removed entities
		entityID = freeIDs.front();
		freeIDs.pop_front();
	}



	Entity entity(entityID);
	entity.registry = this; // The object pointer of the registry class
	entitiesToBeAdded.insert(entity); // New entity that needs to be added 

	LOG_INFO("Entity created with ID: {0}", entityID);

	return entity;
}

void Registry::KillEntity(Entity entity)
{
	entitiesToBeKilled.insert(entity);
	//Logger::Log("Entity " + std::to_string(entity.GetID()) + " was killed");
}

void Registry::AddEntityToSystem(Entity entity)
{

	const auto entityID = entity.GetID();
	const auto& entityComponentSignature = entityComponentSignatures[entityID];

	for (auto& system : systems)
	{

		const auto& systemComponentSignature = system.second->GetComponentSignature();

		/*	Perform a Bitwise Comparison of the component signatures --> the and operation will only work
			if the bits it is comparing are equal to 1. signatures are equal than we are interested!!
		*/
		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested)
		{
			system.second->AddEntityToSystem(entity);
		}
	}
}

void Registry::RemoveEntityFromSystems(Entity entity)
{
	for (auto system : systems)
	{
		system.second->RemoveEntityFromSystem(entity);
	}
}

void Registry::Update()
{
	// Add the entities that are waiting to be created to the active systems
	for (auto entity : entitiesToBeAdded)
	{
		AddEntityToSystem(entity);
	}

	// once the entities are added we must clear the entities that were added
	entitiesToBeAdded.clear();

	// Process the entities that are waiting to be klled from the Active Systems
	for (auto entity : entitiesToBeKilled)
	{

		RemoveEntityFromSystems(entity);

		entityComponentSignatures[entity.GetID()].reset(); // Reset the component signatures from the removed entity

		// Remove the entity from the component pools
		for (auto pool : componentPools)
		{
			if (pool) // If the pool is not null
			{
				pool->RemoveEntityFromPool(entity.GetID());
			}

		}

		// Make the Entity ID available to be used
		freeIDs.push_back(entity.GetID()); // Push back the ID that was just removed

		// Remove any traces of that entity from the  tag/groups maps
		RemoveEntityTag(entity);
		RemoveEntityGroup(entity);
	}

	entitiesToBeKilled.clear();
}
void Registry::RemoveEntityFromPool(Entity entity)
{
	entityComponentSignatures[entity.GetID()].reset(); // Reset the component signatures from the removed entity

	// Remove the entity from the component pools
	for (auto pool : componentPools)
	{
		if (pool) // If the pool is not null
		{
			pool->RemoveEntityFromPool(entity.GetID());
		}

	}

	// Make the Entity ID available to be used
	freeIDs.push_back(entity.GetID()); // Push back the ID that was just removed

	// Remove any traces of that entity from the  tag/groups maps
	//RemoveEntityTag(entity);
	//RemoveEntityGroup(entity);
	//RemoveEntityFromSystems(entity);
}
void Registry::TagEntity(Entity entity, const std::string& tag)
{
	entityPerTag.emplace(tag, entity);
	tagPerEntity.emplace(entity.GetID(), tag);
}

bool Registry::EntityHasTag(Entity entity, const std::string& tag) const
{
	if (tagPerEntity.find(entity.GetID()) == tagPerEntity.end())
	{
		return false;
	}

	return entityPerTag.find(tag)->second == entity;
}

Entity Registry::GetEntityByTag(const std::string& tag) const
{
	return entityPerTag.at(tag);
}

void Registry::RemoveEntityTag(Entity entity)
{
	auto taggedEntity = tagPerEntity.find(entity.GetID());
	if (taggedEntity != tagPerEntity.end())
	{
		auto tag = taggedEntity->second;
		entityPerTag.erase(tag);
		tagPerEntity.erase(taggedEntity);
	}
}

void Registry::GroupEntity(Entity entity, const std::string& group)
{
	entitiesPerGroup.emplace(group, std::set<Entity>());
	entitiesPerGroup[group].emplace(entity);
	groupPerEntity.emplace(entity.GetID(), group);
}

bool Registry::EntityBelongsToGroup(Entity entity, const std::string& group) const
{
	// Error checking and Validation

	if (entitiesPerGroup.find(group) == entitiesPerGroup.end())
	{
		//Logger::Err("The Group [" + group + "]  Does Not Exist");
		return false;
	}

	auto groupEntities = entitiesPerGroup.at(group);

	return groupEntities.find(entity.GetID()) != groupEntities.end();
}

std::vector<Entity> Registry::GetEntitiesByGroup(const std::string& group) const
{
	auto& setOfEntities = entitiesPerGroup.at(group);

	return std::vector<Entity>(setOfEntities.begin(), setOfEntities.end()); // This create a new vector from the set
}

bool Registry::DoesGroupExist(const std::string& group) const
{
	// If the group exists
	if (entitiesPerGroup.find(group) != entitiesPerGroup.end())
		return true;

	return false;
}

void Registry::RemoveEntityGroup(Entity entity)
{
	// If in group, remove entity from group management
	auto groupedEntity = groupPerEntity.find(entity.GetID());
	if (groupedEntity != groupPerEntity.end())
	{
		auto group = entitiesPerGroup.find(groupedEntity->second);
		if (group != entitiesPerGroup.end())
		{
			auto entityInGroup = group->second.find(entity);
			if (entityInGroup != group->second.end())
			{
				group->second.erase(entityInGroup);
			}
		}
		groupPerEntity.erase(groupedEntity);
	}
}

Registry& Registry::Instance()
{
	if (instance == nullptr)
	{
		instance.reset(new Registry());
	}
	return *instance;
}
