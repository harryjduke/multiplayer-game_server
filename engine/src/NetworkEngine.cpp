// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2025 Harry Duke <harryjduke@gmail.com>
// This file includes modifications made by Harry Duke.
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

#include "NetworkEngine.h"

uint64_t NetworkEngine::registerReplicatedObject(const std::vector<void *>& replicatedProperties) {
    const uint64_t nextID = nextReplicatedObjectID;
    replicatedObjects[nextID] = replicatedProperties;
    nextReplicatedObjectID++;
    return nextID;
}

std::unordered_map<uint64_t, std::vector<void *> > NetworkEngine::getReplicatedObjects() const {
    return replicatedObjects;
}

void NetworkEngine::unregisterReplicatedObject(const uint64_t objectID) {
    replicatedObjects.erase(objectID);
}
