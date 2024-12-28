// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2024 Harry Duke <harryjduke@gmail.com>
// This file includes modifications made by Harry Duke.
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

#include <iostream>
#include "Server.h"

int main (int argc, char* args[])
{
    try
    {
        Server server;
        server.runMainLoop();
    }
    catch (std::exception &e)
    {
        std::cout << "error: " << std::endl;
    }
    return 0;
}

