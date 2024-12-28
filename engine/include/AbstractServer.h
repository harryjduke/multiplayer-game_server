// This file is part of CI517_GameEngine <https://github.com/harryjduke/CI517_GameEngine>.
// Copyright (c) 2024 Harry Duke <harryjduke@gmail.com>
// This file includes modifications made by Harry Duke.
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/CI517_GameEngine/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.
//
//  AbstractServer.h
//  GameEngineBase
//
// all code resources are taken from https://github.com/AlmasB/xcube2d/tree/master

#ifndef ABSTRACT_SERVER_H
#define ABSTRACT_SERVER_H

#include "XCube2d.h"

#include <cstdio>

class AbstractServer {

protected:
    AbstractServer();
    virtual ~AbstractServer();

    /* Main loop control */
    bool running;
    bool paused;
    double serverTime;

    virtual void update(float deltaTime) = 0;

public:
    int runMainLoop();

#ifdef __DEBUG
private:
    void handleMouseEvents();

protected:
    /* Engine systems */
    std::shared_ptr<GraphicsEngine> graphicsEngine;
    std::shared_ptr<EventEngine> eventEngine;

    virtual void render() = 0;

    //Define input handlers
    virtual void handleKeyEvents() = 0;
    virtual void onLeftMouseButton();
    virtual void onRightMouseButton();
#endif
};

#endif