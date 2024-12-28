// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2024 Harry Duke <harryjduke@gmail.com>
// This file includes modifications made by Harry Duke.
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

//
//  XCube2d.cpp
//  GameEngineBase
//
// all code resources are taken from https://github.com/AlmasB/xcube2d/tree/master
//

#include "../include/XCube2d.h"
#include <iostream>
#include "utils/EngineCommon.h"

using namespace std;

std::shared_ptr<XCube2Engine> XCube2Engine::instance = nullptr;

XCube2Engine::XCube2Engine() 
{
    std::cout << "Initializing X-CUBE 2D v" << _ENGINE_VERSION_MAJOR << "." << _ENGINE_VERSION_MINOR << std::endl;

    #ifdef __DEBUG
        #if defined(_WIN32)
            debug("WIN32");
        #elif defined(__linux__)
            debug("LINUX");
        #elif defined(__APPLE__)
            debug("MACOSX");
        #endif
    #endif


#ifdef __DEBUG
    graphicsEngine = std::shared_ptr<GraphicsEngine>(new GraphicsEngine());
    debug("GraphicsEngine() successful");
#endif

#ifdef __DEBUG
    eventEngine = std::shared_ptr<EventEngine>(new EventEngine());
    debug("EventEngine() successful");
#endif

}

XCube2Engine::~XCube2Engine() 
{
#ifdef __DEBUG
    debug("XCube2Engine::~XCube2Engine() started");
#endif

#ifdef __DEBUG
    graphicsEngine.reset();
    eventEngine.reset();
#endif

#ifdef __DEBUG
    debug("XCube2Engine::~XCube2Engine() finished");
#endif
}

void XCube2Engine::quit() 
{
    if (instance)
        instance.reset();
}

std::shared_ptr<XCube2Engine> XCube2Engine::getInstance() 
{
    if (!instance)
        instance = std::shared_ptr<XCube2Engine>(new XCube2Engine());
    return instance;
}