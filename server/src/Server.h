// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2024 Harry Duke <harryjduke@gmail.com>
// This file includes modifications made by Harry Duke.
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

#ifndef SERVER_H
#define SERVER_H

#include "AbstractServer.h"

class Server final : public AbstractServer
{
    /* GAMEPLAY */
#ifdef __DEBUG
    void handleKeyEvents() override;
#endif

    void update(float deltaTime) override;

#ifdef __DEBUG
    void render() override;
    void renderUI();
#endif

public:
    Server();
    ~Server() override;

};

#endif