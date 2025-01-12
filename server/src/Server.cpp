// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2024 Harry Duke <harryjduke@gmail.com>
// This file includes modifications made by Harry Duke.
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.


#include "Server.h"

#include <memory>

#include "utils/GameMath.h"

Server::Server()
{
# ifdef __DEBUG
    // Setup graphics engine
    graphicsEngine->setVerticalSync(true);
#endif
}
Server::~Server() = default;

void Server::update(float deltaTime)
{

}

#ifdef __DEBUG

void Server::handleKeyEvents()
{

}

void Server::render()
{

}

void Server::renderUI()
{

}

#endif