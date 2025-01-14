// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2024 Harry Duke <harryjduke@gmail.com>
// This file includes modifications made by Harry Duke.
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

//
//  XCube2d.h
//  GameEngineBase
//
// all code resources are taken from https://github.com/AlmasB/xcube2d/tree/master
//

#ifndef X_CUBE_2D_H
#define X_CUBE_2D_H

#include <cstdio>
#include <string>
#include <vector>
#include <memory>

#include "NetworkEngine.h"
#ifdef __DEBUG
#include "GraphicsEngine.h"
#include "EventEngine.h"
#endif

const int _ENGINE_VERSION_MAJOR = 0;
const int _ENGINE_VERSION_MINOR = 1;

class XCube2Engine {
private:
    static std::shared_ptr<XCube2Engine> instance;

    // Initialize subsystems
    std::shared_ptr<NetworkEngine> networkEngine;
#ifdef __DEBUG
    std::shared_ptr<GraphicsEngine> graphicsEngine;
    std::shared_ptr<EventEngine> eventEngine;
#endif

    XCube2Engine();
public:
    /**
    * @return the instance of game engine
    * @exception throws EngineException if init of any submodules failed
    */
    static std::shared_ptr<XCube2Engine> getInstance();
    ~XCube2Engine();

    std::shared_ptr<NetworkEngine> getNetworkEngine() {return networkEngine;}

#ifdef __DEBUG
    /** @return The graphics engine subsystem instance */
    std::shared_ptr<GraphicsEngine> getGraphicsEngine() {return graphicsEngine;}
    /** @return The event engine subsystem instance */
    std::shared_ptr<EventEngine> getEventEngine() {return eventEngine;}
#endif

    /**
    * Quits the engine, closes all the subsystems
    *
    * All subsequent calls to any of subsystems will have undefined behaviour
    */
    static void quit();

};

typedef XCube2Engine XEngine;

#endif