// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2024 Harry Duke <harryjduke@gmail.com>
// This file includes modifications made by Harry Duke.
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

#ifndef NETWORKENGINE_H
#define NETWORKENGINE_H
#include <vector>
#include <unordered_map>
#include <msgpack.hpp>

/**
 * Type alias for object type identification.
 * Used to uniquely identify different classes of replicatable objects.
 */
using TypeId = std::string_view;

/**
 * Type alias for instance identification.
 * Used to uniquely identify specific instances of replicatable objects.
 */
using InstanceId = uint32_t;

/**
 * Special instance ID indicating an uninitialized replicatable object.
 * Objects with this ID have not yet been registered with a NetworkEngine.
 */
static constexpr InstanceId uninitializedInstanceID = 0;

// Forward declaration
class IReplicatable;

/**
 * Manages the network replication of game objects.
 *
 * NetworkEngine is responsible for:
 * - Tracking all replicatable objects in the game
 * - Assigning unique instance IDs to objects
 * - Serializing objects for network transmission
 * - Managing object lifecycle with respect to replication
 *
 * The engine maintains references but does not own the objects - objects must unregister themselves before destruction.
 *
 * @note This class is designed to work with objects deriving from IReplicatable, objects deriving from Replicatable
 * will automatically be registered and unregistered and provide an instanceId.
 * @see IReplicatable, Replicatable
 */
class NetworkEngine {
    friend class XCube2Engine;
public:
    /**
     * Registers a replicatable object for network replication.
     *
     * Upon registration object is assigned a unique instance ID and stored for serialization if not already
     * initialized.
     *
     * @param object Pointer to the object to register.
     * @pre object != nullptr
     * @note Objects must unregister before destruction
     */
    void registerReplicatedObject(IReplicatable* object);

    /**
     * Unregisters a replicatable object.
     *
     * After unregistration, the object will no longer be included in
     * serialization operations.
     *
     * @param object Pointer to the object to unregister
     */
    void unregisterReplicatedObject(IReplicatable* object);

    /**
     * Gets the current map of all registered replicatable objects.
     *
     * Objects are organized by their TypeId, with each type mapping to a vector of object instances of that type.
     *
     * @return Constant reference to the map of registered objects
     * @note The returned pointers remain owned by their respective objects and could be deallocated at any point
     */
    [[nodiscard]] std::unordered_map<TypeId, std::vector<IReplicatable *>> getReplicatedObjects() const {
        return replicatedObjects_;
    }

    /**
     * Serializes all registered objects into a msgpack buffer.
     *
     * The resulting buffer contains all necessary information to reconstruct the objects' state on another system.
     *
     * @return msgpack buffer containing serialized object data
     * @see IReplicatable::msgpack_pack for individual object serialization
     */
    [[nodiscard]] msgpack::sbuffer getReplicatedObjectsSerialized() const;

private:
    // NetworkEngine tracks but does not own these objects.
    // Objects must unregister themselves before destruction.
    std::unordered_map<TypeId, std::vector<IReplicatable*>> replicatedObjects_{};
    InstanceId nextReplicatedObjectInstanceId_{1};
};

/**
 * Interface base class for replicatable objects, used for network serialization.
 *
 * This abstract class defines the interface that all replicatable objects must implement to support network
 * serialization. It provides:
 *  - Methods that get IDs representing the type and instance of the object
 *  - Instance ID initialization
 *  - msgpack serialization interface
 *
 *  @note objects can instead inherit from Replicatable for automatic registration, unregistration and handing of
 *  instanceID
 *  @see Replicatable
 */
class IReplicatable {
public:
    virtual ~IReplicatable() = default;

    /**
     * Gets the type identifier used for serialization of this object.
     *
     * The type ID uniquely identifies the class/type of the replicatable object and is used during
     * serialization/deserialization to ensure proper object reconstruction.
     * @return The ID for serialization representing the type of this object.
     */
    [[nodiscard]] virtual TypeId getTypeId() const = 0;

    /**
     * Gets the instance identifyer for this object instance.
     *
     * The instance ID uniquely identifies this specific instance among all replicatable objects in the network system.
     * This ID is used to track and update specific object instances during network replication.
     *
     * @return The ID for serialization representing this object instance.
     */
    [[nodiscard]] virtual InstanceId getInstanceId() const = 0;

    /**
     * Initializes this object with a unique instance identifier.
     *
     * This method should only succeed once per object instance. Subsequent calls
     * should return false to prevent ID reassignment.
     *
     * @param instanceId The unique instance identifier to assign to this object
     * @return true if the ID was successfully set, false if already initialized
     */
    virtual bool initializeInstanceId(InstanceId instanceId) = 0;

    /**
     * Serializes this object using msgpack.
     *
     * Implementing classes should serialize all relevant member variables that
     * need to be replicated across the network.
     *
     * @param msgpack_pk The msgpack packer to use for serialization
     */
    virtual void msgpack_pack(msgpack::packer<msgpack::sbuffer>& msgpack_pk) const = 0;

    /**
     * Deserializes this object using msgpack.
     *
     * Implementing classes should deserialize all member variables that were
     * serialized in msgpack_pack().
     *
     * @param msgpack_o The msgpack object containing the serialized data
     */
    virtual void msgpack_unpack(msgpack::object const &msgpack_o) = 0;

    /**
     * Converts this object into a msgpack object.
     *
     * This method is used by msgpack's internal serialization mechanism.
     * Implementing classes should ensure consistency with msgpack_pack().
     *
     * @param msgpack_o Pointer to the msgpack object to populate
     * @param msgpack_z msgpack zone for memory management
     */
    virtual void msgpack_object(msgpack::object *msgpack_o, msgpack::zone &msgpack_z) const = 0;
};

/**
 * CRTP base class for replicatable objects that enables automatic network serialization and replication.
 *
 * This class automatically registers and unregisters the derived object with a NetworkEngine for replication across a
 * network. It handles the instance ID and provides an interface for getting the typeId defined in the derived class.It
 * also provides an interface for msgpack seralization.
 *
 * Requirements for derived classes:
 * 1. Must define a public static constexpr typeId member with a unique string identifier:
 *    @code
 *    static constexpr typeId{"UniqueReplicatedObject"};
 *    @endcode
 *
 * 2. Must implement msgpack serialization using the MSGPACK_DEFINE macro:
 *    @code
 *    MSGPACK_DEFINE(member1, member2);  // List all members to be replicated
 *    @endcode
 *
 * Example usage:
 * @code
 * class MyReplicatedObject : public Replicatable<MyReplicatedObject> {
 * public:
 *     static constexpr typeId{"MyReplicatedObject"};
 *
 *     MyReplicatedObject(NetworkEngine& engine, bool initialState)
 *         : Replicatable<MyReplicatedObject>(engine)
 *         , state_(initialState) {}
 *
 *     MSGPACK_DEFINE(state_);
 *
 * private:
 *     bool state_;
 * };
 * @endcode
 *
 * `static constexpr typeId` member set to a unique string identifyer with public visibility, e.g.:
 * `static contexpr typeId{"ReplicatedObject"}`. Classes derrived from this class must also use the `MSGPACK_DEFINE()`
 * macro with members intended for replication passed as parameters, e.g.: `MSGPACK_DEFINE(exampleBool, exampleInt);`
 * @tparam Derived The derived class type. Must be the same type as the derived class
 *                 (@code class Derived : Replicatable<Derived> @endcode)
 */
template<typename Derived>
class Replicatable : public IReplicatable {
public:
    /**
     * Constructs a replicatable object and registers it with the provided network engine.
     * @param networkEngine Reference to the network engine that will manage this object
     */
    explicit Replicatable(NetworkEngine& networkEngine)
        : networkEngine_(networkEngine) {
        networkEngine.registerReplicatedObject(static_cast<Derived*>(this));
    }

    /**
     * Destructor that automatically unregisters the object from the network engine.
     */
    ~Replicatable() override {
        networkEngine_.unregisterReplicatedObject(static_cast<Derived*>(this));
    }

    /**
     * Static method for getting the type identifier used for serialization.
     * @return The unique type identifier for this replicatable object type
     * @throws Compile-time error if derived class doesn't properly define typeId
     */
    static constexpr TypeId typeId() {
        static_assert(std::is_same_v<decltype(Derived::typeId), const TypeId>,
            "Classes derived from Replicatable must have a static constexpr typeId set to a unique string identifyer");
        return Derived::typeId;
    }

    /**
     * Gets the type identifier for this object.
     * @copydoc IReplicatable::getTypeId
     */
    [[nodiscard]] TypeId getTypeId() const override {
        return typeId();
    }

    /**
     * Gets the instance identifier for this object.
     * @copydoc IReplicatable::getInstanceId
     */
    [[nodiscard]] InstanceId getInstanceId() const override {
        return instanceId_;
    }

    /**
     * Initializes the instance identifier for this object
     * @copydoc IReplicatable::getTypeId
     */
    bool initializeInstanceId(const InstanceId instanceId) override {
        if(instanceId_ == uninitializedInstanceID) {
            instanceId_ = instanceId;
            return true;
        }
        return false;
    }

    /**
     * Calls the derived class method for msgpack serialization
     * @copydoc IReplicatable::msgpack_pack
     */
    void msgpack_pack(msgpack::packer<msgpack::sbuffer>& msgpack_pk) const override {
        static_cast<const Derived*>(this)->msgpack_pack(msgpack_pk);
    }

    /**
     * Calls the derived class method for msgpack deserialization
     * @copydoc IReplicatable::msgpack_unpack
     */
    void msgpack_unpack(msgpack::object const &msgpack_o) override {
        static_cast<Derived*>(this)->msgpack_unpack(msgpack_o);
    }

    /**
     * Calls the derived class method for msgpack object conversion
     * @copydoc IReplicatable::msgpack_object
     */
    void msgpack_object(msgpack::object *msgpack_o, msgpack::zone &msgpack_z) const override{
        static_cast<const Derived*>(this)->msgpack_object(msgpack_o, msgpack_z);
    }

protected:
private:
    NetworkEngine& networkEngine_;
    InstanceId instanceId_{uninitializedInstanceID};
};

#endif //NETWORKENGINE_H
