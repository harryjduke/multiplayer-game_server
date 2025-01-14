// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2025 Harry Duke <harryjduke@gmail.com>
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

#ifndef REPLICATABLE_H
#define REPLICATABLE_H

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
 *  @see Replicated
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

#endif //REPLICATABLE_H
