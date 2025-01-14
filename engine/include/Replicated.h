// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2025 Harry Duke <harryjduke@gmail.com>
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

#ifndef REPLICATED_H
#define REPLICATED_H

#include "NetworkEngine.h"

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
class Replicated : public IReplicatable {
public:
    /**
     * Constructs a replicatable object and registers it with the provided network engine.
     * @param networkEngine Reference to the network engine that will manage this object
     */
    explicit Replicated(NetworkEngine& networkEngine)
        : networkEngine_(networkEngine) {
        networkEngine.registerReplicatedObject(static_cast<Derived*>(this));
    }

    /**
     * Destructor that automatically unregisters the object from the network engine.
     */
    ~Replicated() override {
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

private:
    NetworkEngine& networkEngine_;
    InstanceId instanceId_{uninitializedInstanceID};
};

#endif //REPLICATED_H
