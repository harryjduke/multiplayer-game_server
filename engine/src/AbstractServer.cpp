// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2024 Harry Duke <harryjduke@gmail.com>
// This file includes modifications made by Harry Duke.
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

//
//  AbstractServer.cpp
//  GameEngineBase
// all code resources are taken from https://github.com/AlmasB/xcube2d/tree/master
//

#include "AbstractServer.h"
#include "utils/EngineCommon.h"

using namespace std;

AbstractServer::AbstractServer() : running(true), paused(false), serverTime(0.0)
{
    const std::shared_ptr<XCube2Engine> engine = XCube2Engine::getInstance();

#ifdef __DEBUG
    graphicsEngine = engine->getGraphicsEngine();
    eventEngine = engine->getEventEngine();
#endif
}

AbstractServer::~AbstractServer()
{
#ifdef __DEBUG
    debug("AbstractServer::~AbstractServer() started");
#endif


#ifdef __DEBUG
    // kill Game class' instance pointers
    // so that engine is isolated from the outside world
    // before shutting down
    graphicsEngine.reset();
    eventEngine.reset();
#endif

    // kill engine
    XCube2Engine::quit();

#ifdef __DEBUG
    debug("AbstractServer::~AbstractServer() finished");
    debug("The game finished and cleaned up successfully. Press Enter to exit");
    getchar();
#endif
}

int AbstractServer::runMainLoop() {
#ifdef __DEBUG
    debug("Entered Main Loop");
#endif

    while (running)
    {
#ifdef __DEBUG
        graphicsEngine->setFrameStart();
        eventEngine->pollEvents();

        if (eventEngine->isPressed(Key::ESC) || eventEngine->isPressed(Key::QUIT))
            running = false;

        handleKeyEvents();
        handleMouseEvents();
#endif


        if (!paused) {
            constexpr float deltaTime = 0.016;	// 60 times a sec
            update(deltaTime);
            serverTime += deltaTime;
        }

#ifdef __DEBUG
        graphicsEngine->clearScreen();
        render();
        graphicsEngine->showScreen();

        graphicsEngine->adjustFPSDelay(16);	// atm hardcoded to ~60 FPS
#endif
    }

#ifdef __DEBUG
    debug("Exited Main Loop");
#endif

    return 0;
}


#ifdef __DEBUG
void AbstractServer::handleMouseEvents() {
    if (eventEngine->isPressed(Mouse::BTN_LEFT)) onLeftMouseButton();
    if (eventEngine->isPressed(Mouse::BTN_RIGHT)) onRightMouseButton();
}

void AbstractServer::onLeftMouseButton() {

}

void AbstractServer::onRightMouseButton() {

}
#endif