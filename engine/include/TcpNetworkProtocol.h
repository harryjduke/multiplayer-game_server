// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2025 Harry Duke <harryjduke@gmail.com>
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

#ifndef TCPNETWORKPROTOCOL_H
#define TCPNETWORKPROTOCOL_H

#include <memory>
#include <mutex>
#include <queue>
#include <SDL_net.h>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

#include "NetworkProtocol.h"

class Socket {
public:
    Socket(const TCPsocket& socket, const SDLNet_SocketSet& socketSet);
    ~Socket();

    [[nodiscard]] TCPsocket get() const { return socket_; }

private:
    const SDLNet_SocketSet& socketSet_;
    TCPsocket socket_;
};

class TcpNetworkProtocol final : public INetworkProtocol {
public:
    explicit TcpNetworkProtocol(uint16_t port = 8099, uint16_t maxSockets = 16);
    ~TcpNetworkProtocol() override;

    std::optional<Message> recieve() override;
    void send(Message message) override;

private:
    bool running_;
    uint16_t maxSockets_;
    SDLNet_SocketSet socketSet_;
    std::unique_ptr<Socket> serverSocket_;
    std::unordered_map<ClientId, std::unique_ptr<Socket>> sockets_;
    ClientId nextClientId{};
    std::queue<Message> incomingMessageQueue_;
    std::queue<Message> outgoingMessageQueue_;

    std::shared_mutex socketsMutex_;
    std::mutex incomingMessageQueueMutex_;
    std::mutex outgoingMessageQueueMutex_;

    std::thread recieveThread_;
    std::thread sendThread_;

    bool acceptSocket();
    void processReceive();
    void processSend();
};

#endif //TCPNETWORKPROTOCOL_H
