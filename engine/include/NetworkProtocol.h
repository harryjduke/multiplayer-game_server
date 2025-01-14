// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2025 Harry Duke <harryjduke@gmail.com>
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

#ifndef NETWORKPROTOCOL_H
#define NETWORKPROTOCOL_H

#include <cstdint>
#include <optional>
#include <vector>

using ClientId = uint32_t;

struct Message {
    ClientId clientId;
    std::vector<uint8_t> body;
};

class INetworkProtocol {
public:
    virtual ~INetworkProtocol() = default;

    virtual std::optional<Message> recieve() = 0;

    virtual void send(Message message) = 0;
};

#endif //NETWORKPROTOCOL_H
