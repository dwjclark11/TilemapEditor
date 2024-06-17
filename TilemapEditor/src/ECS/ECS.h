#pragma once
#include "Logger/Logger.h"

#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <deque>

// Define Constants
const unsigned int MAX_COMPONENTS = 32;

// ============================================================================================
// Signature
// ============================================================================================
// We use a bitset (1s and 0s) to keep track of which components an
// entity has.
// Also helps keep track of which entities a given system is interested in
// ============================================================================================
typedef std::bitset<MAX_COMPONENTS> Signature;

struct BaseComponent
{
  protected:
	static int nextID;
};

// Used to assign a unique ID to a component type --> Template classes are placeHolders
template <typename T>
class Component : public BaseComponent
{
  public:
	// Returns the unique ID of Component<T>
	static int GetID()
	{
		static auto id = nextID++;
		return id;
	}
};

class Entity
{
  private:
	int ID;

  public:
	Entity( int id )
		: ID( id )
		, registry( nullptr ){};
	Entity( const Entity& entity ) = default;
	int GetID() const;
	void Kill();

	// Operator overloading
	inline bool operator==( const Entity& otherEntity ) const { return ID == otherEntity.ID; }
	inline bool operator!=( const Entity& otherEntity ) const { return ID != otherEntity.ID; }
	inline bool operator>( const Entity& otherEntity ) const { return ID > otherEntity.ID; }
	inline bool operator<( const Entity& otherEntity ) const { return ID < otherEntity.ID; }
	inline Entity& operator=( const Entity& otherEntity ) = default;

	// Manage entity tags and groups
	void Tag( const std::string& tag );
	bool HasTag( const std::string& tag ) const;
	void Group( const std::string& group );
	bool BelongsToGroup( const std::string& group ) const;

	// Manage Entity components
	template <typename TComponent, typename... TArgs>
	void AddComponent( TArgs&&... args );
	template <typename TComponent>
	void RemoveComponent();
	template <typename TComponent>
	bool HasComponent() const;
	template <typename TComponent>
	TComponent& GetComponent() const;

	// Hold a pointer to the entity's owner registry
	class Registry* registry; // Be careful for cyclic Dependencies
};
// ============================================================================================
// System
// ============================================================================================
// The system processes entities that contain a specific signature
// ============================================================================================
class System
{
  private:
	Signature componentSignature;
	std::vector<Entity> entities;

  public:
	System() = default;
	~System() = default;

	void AddEntityToSystem( Entity entity );
	void RemoveEntityFromSystem( Entity entity );

	std::vector<Entity> GetSystemEntities() const;
	const Signature& GetComponentSignature() const;

	// Defines the component type that entities must have to be considered by the system
	template <typename TComponent>
	void RequiredComponent();
};

class IPool // Parent class for the Template class Pool. This is for PolyMorphism
{
  public:
	virtual ~IPool() = default;
	virtual void RemoveEntityFromPool( int entityID ) = 0;
};

// ============================================================================================
// Pool
// ============================================================================================
// A pool is just a vector (contiguous data) of obects of type T
// ============================================================================================
template <typename T>
class Pool : public IPool
{
  private:
	// Keep track of the vector of objects and the current number of elements
	std::vector<T> data;
	int size;

	// Helper maps to keep track of entity ids per index, so the vector is always packed
	std::unordered_map<int, int> entityIDToIndex;
	std::unordered_map<int, int> indexToEntityID;

  public:
	Pool( int capacity = 500 )
	{
		size = 0;
		data.resize( capacity );
	}
	virtual ~Pool() = default;

	bool IsEmpty() const
	{
		return size == 0;
		;
	}

	int GetSize() const { return size; }

	void Resize( int n ) { data.resize( n ); }

	void Clear()
	{
		data.clear();
		size = 0;
	}

	void Add( T object ) { data.push_back( object ); }

	void Set( int entityID, T object )
	{
		if ( entityIDToIndex.find( entityID ) != entityIDToIndex.end() )
		{
			// If the element already exists, simply replace the component ogject
			int index = entityIDToIndex[ entityID ];
			data[ index ] = object;
		}
		else
		{
			// When adding a new object, we keep track of the entity IDs and their vector index
			int index = size;
			entityIDToIndex.emplace( entityID, index );
			indexToEntityID.emplace( index, entityID );
			if ( index >= data.capacity() )
			{
				// If necessary, we resize by always doubling the current capacity
				data.resize( size * 2 );
			}
			data[ index ] = object;
			size++;
		}
	}

	void Remove( int entityID )
	{
		if ( size > 0 )
		{
			// Copy the last element to the deleted position to keep the array packed
			int indexOfRemoved = entityIDToIndex[ entityID ];
			int indexOfLast = size - 1;
			data[ indexOfRemoved ] = data[ indexOfLast ];

			// Update the index-entity maps to point to the correct elements
			int entityIDOfLastElement = indexToEntityID[ indexOfLast ];
			entityIDToIndex[ entityIDOfLastElement ] = indexOfRemoved;
			indexToEntityID[ indexOfRemoved ] = entityIDOfLastElement;

			entityIDToIndex.erase( entityID );
			indexToEntityID.erase( indexOfLast );

			size--;
		}
	}

	void RemoveEntityFromPool( int entityID ) override
	{
		if ( entityIDToIndex.find( entityID ) != entityIDToIndex.end() )
		{
			// Logger::Log("Entity Removed: " + std::to_string(entityID));
			Remove( entityID );
		}
	}

	T& Get( int entityID )
	{
		int index = entityIDToIndex[ entityID ]; // Get the index then return the data
		return static_cast<T&>( data[ index ] );
	}

	T& operator[]( unsigned int index ) { return data[ index ]; }
};

// ============================================================================================
// Registry --> Manager
// ============================================================================================
// The registry manages the creation and destruction of entities, as
// well as adding systems and adding components to entities
// ============================================================================================
class Registry
{
  private:
	// Keep track of how many entities were added to the scene
	int numEntities = 0;
	int entityID;
	std::set<Entity> entitiesToBeAdded;	 // Entities awaitng creation in the next Registry Update()
	std::set<Entity> entitiesToBeKilled; // Entities awaitng destruction in the next Registry Update()

	// List of free entity IDs that were previously removed
	std::deque<int> freeIDs;

	// Vector of component pools --> Buffer of Values each pool contains all the data for a certain component type
	// Each pool contains all the data for a certain component type
	// [vector index = component[ID],  [pool index = entity[ID]

	std::vector<std::shared_ptr<IPool>> componentPools;

	// Vector of component signatures The signature lets us know which components are turned "on" for an entity
	//[vector index = entity id]
	std::vector<Signature> entityComponentSignatures;

	// Map of active systems [index = system typeID]
	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	// Entity tags (one tag name per entity)
	std::unordered_map<std::string, Entity> entityPerTag;
	std::unordered_map<int, std::string> tagPerEntity; // Int is used to go by ID #

	// Entiy groups (a set of entities per group name)
	std::unordered_map<std::string, std::set<Entity>> entitiesPerGroup;
	std::unordered_map<int, std::string> groupPerEntity;
	;

	// static Registry* instance;
	static std::unique_ptr<Registry> instance;

  public:
	// Management of entities, systems, and components
	Registry()
		: entityID( 1 )
	{
	}
	~Registry() {}

	// Entity Management
	Entity CreateEntity();
	void KillEntity( Entity entity );

	// Checks the Component Signature of a certain entity and adds the entity to the
	// Systems that we are interested in
	void AddEntityToSystem( Entity entity ); // Only if the component Signatures match
	void RemoveEntityFromSystems( Entity entity );

	void Update();
	void RemoveEntityFromPool( Entity entity );
	// Tag Management
	void TagEntity( Entity entity, const std::string& tag );
	bool EntityHasTag( Entity entity, const std::string& tag ) const;
	Entity GetEntityByTag( const std::string& tag ) const;
	void RemoveEntityTag( Entity entity );

	// Group Management
	void GroupEntity( Entity entity, const std::string& group );
	bool EntityBelongsToGroup( Entity entity, const std::string& group ) const;
	std::vector<Entity> GetEntitiesByGroup( const std::string& group ) const;
	bool DoesGroupExist( const std::string& group ) const;
	void RemoveEntityGroup( Entity entity );
	std::set<Entity> GetEntitiesToBeKilled() const { return entitiesToBeKilled; }

	// static Registry* Instance();
	static Registry& Instance();
	// static void ReleaseInstance();

	// Component Management Functions
	// Function template to add a componenet of type T to a given entity
	template <typename TComponent, typename... TArgs>
	void AddComponent( Entity entity, TArgs&&... args );
	template <typename TComponent>
	void RemoveComponent( Entity entity );
	template <typename TComponent>
	bool HasComponent( Entity entity ) const;
	template <typename TComponent>
	TComponent& GetComponent( Entity entity ) const;

	// System Management Functions
	template <typename TSystem, typename... TArgs>
	void AddSystem( TArgs&&... args );
	template <typename TSystem>
	void RemoveSystem();
	template <typename TSystem>
	bool HasSystem();
	template <typename TSystem>
	TSystem& GetSystem() const;
};

// ======================================================================================================
// Templates --> Generics or Function Holders
// ======================================================================================================
template <typename TComponent>
void System::RequiredComponent()
{
	const auto componentID = Component<TComponent>::GetID();
	componentSignature.set( componentID );
}

template <typename TComponent, typename... TArgs>
void Registry::AddComponent( Entity entity, TArgs&&... args )
{
	const auto componentID = Component<TComponent>::GetID();
	const auto entityID = entity.GetID();

	// If the component ID is greater than the current size of the componentPools, then resize the vector
	if ( componentID >= componentPools.size() )
	{
		componentPools.resize( componentID + 1, nullptr );
	}

	// If we still don't have a Pool for that component type
	if ( !componentPools[ componentID ] )
	{
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		componentPools[ componentID ] = newComponentPool;
	}
	// std::shared_ptr<Pool<TComponent>> componentPool =
	// std::static_pointer_cast<Pool<TComponent>>(componentPools[componentID]);
	std::shared_ptr<Pool<TComponent>> componentPool =
		std::static_pointer_cast<Pool<TComponent>>( componentPools[ componentID ] );

	// Create a new Component object of type TComponent, and forward the various parameters to the constructor
	TComponent newComponent( std::forward<TArgs>( args )... );

	// Add the new Component object to the component pool list, using the entity ID as index
	componentPool->Set( entityID, newComponent );

	// Finally, change the component signature of the entity and set the component ID on the bitset to 1
	entityComponentSignatures[ entityID ].set( componentID );

	// Logger::Log("Component ID = " + std::to_string(componentID) + " was added to entity ID: " +
	// std::to_string(entityID));

	// std::cout << "COMPONENT ID = " << componentID << " --> POOL SIZEL " << componentPool->GetSize() << std::endl;
}

template <typename TComponent>
void Registry::RemoveComponent( Entity entity )
{
	const auto componentID = Component<TComponent>::GetID();
	const auto entityID = entity.GetID();

	// Find the desired component pool --> Remove the component from the component list for that entiy
	std::shared_ptr<Pool<TComponent>> componentPool =
		std::static_pointer_cast<Pool<TComponent>>( componentPools[ componentID ] );
	componentPool->Remove( entityID );

	// Set this component signature for that entity to false
	entityComponentSignatures[ entityID ].set( componentID, false );

	// Logger::Log("Component ID = " + std::to_string(componentID) + " was Removed to entity ID: " +
	// std::to_string(entityID));
}

template <typename TComponent>
bool Registry::HasComponent( Entity entity ) const
{
	const auto componentID = Component<TComponent>::GetID();
	const auto entityID = entity.GetID();

	return entityComponentSignatures[ entityID ].test( componentID ); // Bitset.test will look at the component ID
}

template <typename TComponent>
inline TComponent& Registry::GetComponent( Entity entity ) const
{
	const auto componentID = Component<TComponent>::GetID();
	const auto entityID = entity.GetID();

	auto componentPool = std::static_pointer_cast<Pool<TComponent>>( componentPools[ componentID ] );

	return componentPool->Get( entityID );
}

// System Templates
template <typename TSystem, typename... TArgs>
inline void Registry::AddSystem( TArgs&&... args )
{
	std::shared_ptr<TSystem> newSystem = std::make_shared<TSystem>( std::forward_as_tuple<TArgs>( args )... );
	systems.insert( std::make_pair( std::type_index( typeid( TSystem ) ),
									newSystem ) ); // Adding a new Key value to the sytem data structure
}

template <typename TSystem>
inline void Registry::RemoveSystem()
{
	auto system = systems.find( std::type_index( typeid( TSystem ) ) ); // Find a system!!
	systems.erase( system );											// remove that system
}

template <typename TSystem>
inline bool Registry::HasSystem()
{
	/*
		Find for the key and the pointer is different than the last end pointer
		Than this is the key
	*/

	return systems.find( std::type_index( typeid( TSystem ) ) ) != systems.end();
}

template <typename TSystem>
inline TSystem& Registry::GetSystem() const
{
	/*
		In a map, if you use map.first()   --> It will get you the Key
				  If you use map.second()  --> It will get you the value!
	*/
	auto system = systems.find( std::type_index( typeid( TSystem ) ) );

	return *( std::static_pointer_cast<TSystem>( system->second ) );
}

template <typename TComponent, typename... TArgs>
inline void Entity::AddComponent( TArgs&&... args )
{
	// registry->AddComponent<TComponent>(*this); // try this later
	registry->AddComponent<TComponent>( *this, std::forward<TArgs>( args )... );
}

template <typename TComponent>
inline void Entity::RemoveComponent()
{
	registry->RemoveComponent<TComponent>( *this );
}

template <typename TComponent>
inline bool Entity::HasComponent() const
{
	return registry->HasComponent<TComponent>( *this );
}

template <typename TComponent>
inline TComponent& Entity::GetComponent() const
{
	return registry->GetComponent<TComponent>( *this );
}
