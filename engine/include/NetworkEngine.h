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

#include "Replicatable.h"

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
 * @see IReplicatable, Replicated
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

#endif //NETWORKENGINE_H
