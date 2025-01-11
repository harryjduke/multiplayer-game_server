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

using TypeId = std::string_view;
using InstanceId = uint32_t;
static constexpr InstanceId uninitializedInstanceID = 0;
class IReplicatable;

class NetworkEngine {
    friend class XCube2Engine;
public:
    /**
     * Register a `Replicatable` object to be replicated.
     * @param object The `Replicatable` object to register.
     */
    void registerReplicatedObject(IReplicatable* object);

    /**
     * Unregister a `Replicatable` object to no longer be replicated. Called in the `Replicatable` destructor.
     * @param object The `Replicatable` object to unregister.
     */
    void unregisterReplicatedObject(IReplicatable* object);

    [[nodiscard]] std::unordered_map<TypeId, std::vector<IReplicatable *>> getReplicatedObjects() const {
        return replicatedObjects_;
    }

    /**
     * Get the replicated objects as a buffer, serialized using the msgpack spec.
     * @return A buffer containing the replicated objects, serialized using the msgpack spec.
     */
    [[nodiscard]] msgpack::sbuffer getReplicatedObjectsSerialized() const;

private:
    // NetworkEngine tracks but does not own these objects.
    // Objects must unregister themselves before destruction.
    std::unordered_map<TypeId, std::vector<IReplicatable*>> replicatedObjects_{};
    InstanceId nextReplicatedObjectInstanceId_{1};
};

class IReplicatable {
public:
    virtual ~IReplicatable() = default;

    /**
     * @return The ID for serialization representing the type of this object.
     */
    [[nodiscard]] virtual TypeId getTypeId() const = 0;

    /**
     * @return The ID for serialization representing this object instance.
     */
    [[nodiscard]] virtual InstanceId getInstanceId() const = 0;

    /**
     * Initialize this object instance with an ID.
     * @param instanceId The instance ID to initialize this object with.
     * @return True if the initialization succeeded, false if the ID is already set.
     */
    virtual bool initializeInstanceId(InstanceId instanceId) = 0;

    /** Pure virtual method used by msgpack to serialize this object. */
    virtual void msgpack_pack(msgpack::packer<msgpack::sbuffer>& msgpack_pk) const = 0;
    /** Pure virtual method used by msgpack to deserialize this object. */
    virtual void msgpack_unpack(msgpack::object const &msgpack_o) = 0;
    /** Pure virtual method used by msgpack to convert this object into a `msgpack::object`. */
    virtual void msgpack_object(msgpack::object *msgpack_o, msgpack::zone &msgpack_z) const = 0;
};

template<typename Derived>
class Replicatable : public IReplicatable {
public:
    explicit Replicatable(NetworkEngine& networkEngine)
        : networkEngine_(networkEngine) {
        networkEngine.registerReplicatedObject(static_cast<Derived*>(this));
    }

    ~Replicatable() override {
        networkEngine_.unregisterReplicatedObject(static_cast<Derived*>(this));
    }

    /**
     * Static method for getting the ID for serialization representing the type of this object.
     * @return The ID for serialization representing the type of this object.
     */
    static constexpr TypeId typeId() {
        static_assert(std::is_same_v<decltype(Derived::typeId), const TypeId>,
            "Classes derived from Replicatable must have a static constexpr typeId set to a unique string identifyer");
        return Derived::typeId;
    }

    [[nodiscard]] TypeId getTypeId() const override {
        return typeId();
    }

    [[nodiscard]] InstanceId getInstanceId() const override {
        return instanceId_;
    }

    bool initializeInstanceId(const InstanceId instanceId) override {
        if(instanceId_ == uninitializedInstanceID) {
            instanceId_ = instanceId;
            return true;
        }
        return false;
    }

    void msgpack_pack(msgpack::packer<msgpack::sbuffer>& msgpack_pk) const override {
        static_cast<const Derived*>(this)->msgpack_pack(msgpack_pk);
    }

    void msgpack_unpack(msgpack::object const &msgpack_o) override {
        static_cast<Derived*>(this)->msgpack_unpack(msgpack_o);
    }

    void msgpack_object(msgpack::object *msgpack_o, msgpack::zone &msgpack_z) const override{
        static_cast<const Derived*>(this)->msgpack_object(msgpack_o, msgpack_z);
    }

protected:
private:
    NetworkEngine& networkEngine_;
    InstanceId instanceId_{uninitializedInstanceID};

};

#endif //NETWORKENGINE_H
