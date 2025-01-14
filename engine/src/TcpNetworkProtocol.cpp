// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2025 Harry Duke <harryjduke@gmail.com>
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

#include "TcpNetworkProtocol.h"

#include <iostream>
#include <mutex>
#include <stdexcept>

#include "utils/EngineCommon.h"

Socket::Socket(const TCPsocket& socket, const SDLNet_SocketSet& socketSet)
    : socketSet_(socketSet)
    , socket_(socket)
{
    if (socket_ == nullptr) {
        throw std::runtime_error("SDLNet_TCP_Open: " + std::string(SDLNet_GetError()));
    }

    if(SDLNet_TCP_AddSocket(socketSet, socket_) == -1) {
        throw std::runtime_error("SDLNet_TCP_AddSocket: " + std::string(SDLNet_GetError()));
    }
}

Socket::~Socket() {
    if (SDLNet_TCP_DelSocket(socketSet_, socket_) == -1) {
        std::cerr << "SDLNet_TCP_DelSocket: " << SDLNet_GetError() << std::endl;
    }
    SDLNet_TCP_Close(socket_);
}

TcpNetworkProtocol::TcpNetworkProtocol(const uint16_t port, const uint16_t maxSockets)
    : running_(true)
    , maxSockets_(maxSockets)
{
    if (SDLNet_Init() == -1) {
        throw std::runtime_error("SDLNet_Init: " + std::string(SDLNet_GetError()));
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, nullptr, port) == -1) {
        throw std::runtime_error("SDLNet_ResolveHost: " + std::string(SDLNet_GetError()));
    }

    socketSet_ = SDLNet_AllocSocketSet(maxSockets + 1);
    if (socketSet_ == nullptr) {
        throw std::runtime_error("SDLNet_AllocSocketSet: " + std::string(SDLNet_GetError()));
    }
    serverSocket_ = std::make_unique<Socket>(SDLNet_TCP_Open(&ip), socketSet_);

    recieveThread_ = std::thread(&TcpNetworkProtocol::processReceive, this);
    sendThread_ = std::thread(&TcpNetworkProtocol::processSend, this);

}

TcpNetworkProtocol::~TcpNetworkProtocol() {
    running_ = false;
    if (recieveThread_.joinable()) {
        recieveThread_.join();
    }
    if (sendThread_.joinable()) {
        sendThread_.join();
    }
    sockets_.clear();
    serverSocket_.reset();
    SDLNet_FreeSocketSet(socketSet_);
    SDLNet_Quit();
}

std::optional<Message> TcpNetworkProtocol::recieve() {
    std::lock_guard lock(incomingMessageQueueMutex_);
    if (incomingMessageQueue_.empty()) {
        return std::nullopt;
    }
    auto message = incomingMessageQueue_.front();
    incomingMessageQueue_.pop();
    return message;
}

void TcpNetworkProtocol::send(const Message message) {
    std::lock_guard lock(outgoingMessageQueueMutex_);
    outgoingMessageQueue_.push(message);
}

bool TcpNetworkProtocol::acceptSocket() {
    std::unique_lock lock(socketsMutex_);
    try {
        sockets_[nextClientId] = std::make_unique<Socket>(SDLNet_TCP_Accept(serverSocket_->get()), socketSet_);
    } catch (std::runtime_error&) {
        return false;
    }
    std::lock_guard incomingMessageQueueLock(incomingMessageQueueMutex_);
    incomingMessageQueue_.push({nextClientId, {}});
    nextClientId++;
    return true;
}

void TcpNetworkProtocol::processReceive() {
    while (running_) {
        int activeSockets = SDLNet_CheckSockets(socketSet_, 100);
        if (activeSockets <= 0) continue;
        if (SDLNet_SocketReady(serverSocket_->get())) {
            if (acceptSocket()) {
                debug("Client Connected");
            }
            activeSockets--;
        }
        if (activeSockets <= 0) continue;
        std::vector<ClientId> disconnectedClients;
        {
            std::shared_lock socketsLock(socketsMutex_);
            for (auto &[clientId, socket]: sockets_) {
                if (SDLNet_SocketReady(socket.get())) {
                    uint8_t buffer[256];
                    int receivedSize = SDLNet_TCP_Recv(socket->get(), buffer, 256);
                    if (receivedSize > 0) {
                        std::lock_guard incomingMessageQueueLock(incomingMessageQueueMutex_);
                        incomingMessageQueue_.push({clientId, std::vector(buffer, buffer + receivedSize)});
                    } else {
                        disconnectedClients.push_back(clientId);
                        debug("Client Disconnected");
                    }
                }
            }
        }
        {
            std::unique_lock socketsLock(socketsMutex_);
            for (ClientId clientId: disconnectedClients) {
                sockets_.erase(clientId);
            }
        }
    }
}

void TcpNetworkProtocol::processSend() {
    while (running_) {
        std::lock_guard outgoingMessageQueueLock(outgoingMessageQueueMutex_);
        while(!outgoingMessageQueue_.empty()) {
            auto [clientId, body] = outgoingMessageQueue_.front();
            if (sockets_[clientId]) {
                if (SDLNet_TCP_Send(sockets_[clientId]->get(), &body, static_cast<int>(body.size())) < body.size()) {
                    std::cerr << "SLDNet_TCP_Send: " << SDLNet_GetError() << std::endl;
                    sockets_.erase(clientId);
                }
            }
            outgoingMessageQueue_.pop();
        }
    }
}
