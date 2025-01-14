// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2025 Harry Duke <harryjduke@gmail.com>
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.


#include "NetworkEngine.h"

//NetworkEngine::NetworkEngine() : NetworkEngine(std::make_unique<INetworkPort>()) {}

NetworkEngine::NetworkEngine(std::unique_ptr<INetworkProtocol> networkPort) : networkPort_(std::move(networkPort)) {
}

void NetworkEngine::update() {
    while (const auto message = networkPort_->recieve()) {
        if(std::ranges::find(players_, message->clientId) == players_.end()) {
            players_.push_back(message->clientId);
        }
    }

    const std::vector<uint8_t> gameState = getReplicatedObjectsSerialized();
    for (const auto playerClientId: players_) {
        networkPort_->send({playerClientId, gameState});
    }
}

void NetworkEngine::registerReplicatedObject(IReplicatable* object) {
    if (!object) {
        throw std::invalid_argument("Cannot register null object");
    }

    if (nextReplicatedObjectInstanceId_ == std::numeric_limits<InstanceId>::max()) {
        throw std::runtime_error("Instance ID overflow");
    }

    auto& objectsOfType = replicatedObjects_[object->getTypeId()];
    if (std::ranges::find(objectsOfType, object) != objectsOfType.end()) {
        throw std::runtime_error("Object already registered");
    }

    if (object->initializeInstanceId(nextReplicatedObjectInstanceId_++)) {
        objectsOfType.push_back(object);
    }
    else {
        throw std::runtime_error("Object instance ID already initialized");
    }
}

void NetworkEngine::unregisterReplicatedObject(IReplicatable* object) {
    auto& objectsOfType = replicatedObjects_[object->getTypeId()];
    std::erase(objectsOfType, object);

    if (objectsOfType.empty()) {
        replicatedObjects_.erase(object->getTypeId());
    }
}

// Custom adaptor class template specialization to support packing `replicatableObjects_`
// ReSharper disable once CppRedundantNamespaceDefinition - msgpack-c documented syntax
namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
template <>
struct adaptor::pack<std::unordered_map<TypeId, std::vector<IReplicatable*>>> {
    template <typename Stream>
    packer<Stream>& operator()(packer<Stream>& o,
        const std::unordered_map<TypeId, std::vector<IReplicatable*>>& v) const {
        static_assert(std::is_same_v<Stream, sbuffer>, "IReplicatable* can only be packed into a msgpack::sbuffer");
        o.pack_map(v.size());
        for (const auto& [typeId, objects] : v) {
            o.pack(typeId);
            o.pack_map(objects.size());
            for (auto object: objects) {
                if (object == nullptr) {
                    throw std::runtime_error("Attempting to serialize null pointer");
                }
                o.pack(object->getInstanceId());
                o.pack(*object);
            }
        }

        return o;
    }
}; // namespace adaptor
} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack

std::vector<uint8_t> NetworkEngine::getReplicatedObjectsSerialized() const {
    msgpack::sbuffer buffer;
    msgpack::pack(buffer, replicatedObjects_);
    return {buffer.data(), buffer.data() + buffer.size()};
}