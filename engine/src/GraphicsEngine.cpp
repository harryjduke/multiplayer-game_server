// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2024 Harry Duke <harryjduke@gmail.com>
// This file includes modifications made by Harry Duke.
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.


//
//  GraphicsEngine.cpp
//  GameEngineBase
//
// all code resources are taken from https://github.com/AlmasB/xcube2d/tree/master
//

#include "GraphicsEngine.h"

#include <cmath>

#include <SDL_image.h>

SDL_Renderer* GraphicsEngine::renderer = nullptr;

GraphicsEngine::GraphicsEngine() : fpsAverage(0), fpsPrevious(0), fpsStart(0), fpsEnd(0), drawColor(toSDLColor(0, 0, 0, 255))
{
    window = SDL_CreateWindow("The X-CUBE 2D Game Engine",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

    if (nullptr == window)
        throw EngineException("Failed to create window", SDL_GetError());

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (nullptr == renderer)
        throw EngineException("Failed to create renderer", SDL_GetError());

    // although not necessary, SDL doc says to prevent hiccups load it before using
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG)
        throw EngineException("Failed to init SDL_image - PNG", IMG_GetError());

}

GraphicsEngine::~GraphicsEngine() {
#ifdef __DEBUG
    debug("GraphicsEngine::~GraphicsEngine() started");
#endif

    IMG_Quit();
    SDL_DestroyWindow(window);
    SDL_Quit();

#ifdef __DEBUG
    debug("GraphicsEngine::~GraphicsEngine() finished");
#endif
}

void GraphicsEngine::setWindowTitle(const char* title) {
    SDL_SetWindowTitle(window, title);
#ifdef __DEBUG
    debug("Set window title to:", title);
#endif
}

void GraphicsEngine::setWindowTitle(const std::string& title) {
    SDL_SetWindowTitle(window, title.c_str());
#ifdef __DEBUG
    debug("Set window title to:", title.c_str());
#endif
}

void GraphicsEngine::setWindowIcon(const char* iconFileName) {
    SDL_Surface* icon = IMG_Load(iconFileName);
    if (nullptr == icon) {
        std::cout << "Failed to load icon: " << iconFileName << std::endl;
        std::cout << "Aborting: GraphicsEngine::setWindowIcon()" << std::endl;
        return;
    }
    SDL_SetWindowIcon(window, icon);
#ifdef __DEBUG
    debug("Set Window Icon to", iconFileName);
#endif
    SDL_FreeSurface(icon);
}

void GraphicsEngine::setFullscreen(bool b) {
    SDL_SetWindowFullscreen(window, b ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_MAXIMIZED);
}

void GraphicsEngine::setVerticalSync(bool b) {
    if (!SDL_SetHint(SDL_HINT_RENDER_VSYNC, b ? "1" : "0")) {
        std::cout << "Failed to set VSYNC" << std::endl;
        std::cout << SDL_GetError() << std::endl;
    }
#ifdef __DEBUG
    debug("Current VSYNC:", SDL_GetHint(SDL_HINT_RENDER_VSYNC));
#endif
}

void GraphicsEngine::setDrawColor(const SDL_Color& color) {
    drawColor = color;
    SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b, 255);    // may need to be adjusted for allowing alpha
}

void GraphicsEngine::setWindowSize(const int& w, const int& h) {
    SDL_SetWindowSize(window, w, h);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
#ifdef __DEBUG
    debug("Set Window W", w);
    debug("Set Window H", h);
#endif
}

Dimension2i GraphicsEngine::getCurrentWindowSize() {
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    return {w, h};
}

Dimension2i GraphicsEngine::getMaximumWindowSize() {
    SDL_DisplayMode current;
    if (SDL_GetCurrentDisplayMode(0, &current) == 0) {
        return {current.w, current.h};
    }
    else {
        std::cout << "Failed to get window data" << std::endl;
        std::cout << "GraphicsEngine::getMaximumWindowSize() -> return (0, 0)" << std::endl;
        return {};
    }
}

void GraphicsEngine::showInfoMessageBox(const std::string& info, const std::string& title) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.c_str(), info.c_str(), window);
}

void GraphicsEngine::clearScreen() const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b, 255);    // may need to be adjusted for allowing alpha
}

void GraphicsEngine::showScreen() {
    SDL_RenderPresent(renderer);
}


void GraphicsEngine::setFrameStart() {
    fpsStart = SDL_GetTicks();
}

void GraphicsEngine::adjustFPSDelay(const Uint32& delay)
{
    fpsEnd = SDL_GetTicks() - fpsStart;
    if (fpsEnd < delay) {
        SDL_Delay(delay - fpsEnd);
    }

    Uint32 fpsCurrent = 1000 / (SDL_GetTicks() - fpsStart);
    fpsAverage = (fpsCurrent + fpsPrevious + fpsAverage * 8) / 10;    // average, 10 values / 10
    fpsPrevious = fpsCurrent;
}

Uint32 GraphicsEngine::getAverageFPS() const {
    return fpsAverage;
}

SDL_Texture* GraphicsEngine::createTextureFromSurface(SDL_Surface* surf) {
    return SDL_CreateTextureFromSurface(renderer, surf);
}



void GraphicsEngine::setDrawScale(const Vector2F& v) {
    SDL_RenderSetScale(renderer, v.x, v.y);
}

/* ALL DRAW FUNCTIONS */
/* overloads explicitly call SDL funcs for better performance hopefully */

void GraphicsEngine::drawRect(const SDL_Rect& rect)
{
    // const Rectangle2I *p = &rect;
    SDL_RenderDrawRect(renderer, &rect);
}

void GraphicsEngine::drawRect(const SDL_Rect& rect, const SDL_Color& color) const {

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

    SDL_RenderDrawRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b, 255);
}

void GraphicsEngine::drawRect(SDL_Rect* rect, const SDL_Color& color) const {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderDrawRect(renderer, rect);
    SDL_SetRenderDrawColor(renderer, drawColor.r, drawColor.g, drawColor.b, 255);
}

void GraphicsEngine::drawRect(SDL_Rect* rect) {
    SDL_RenderDrawRect(renderer, rect);
}

void GraphicsEngine::drawRect(const int& x, const int& y, const int& w, const int& h) {
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderDrawRect(renderer, &rect);
}

void GraphicsEngine::fillRect(SDL_Rect* rect) {
    SDL_RenderFillRect(renderer, rect);
}

void GraphicsEngine::fillRect(const int& x, const int& y, const int& w, const int& h) {
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderFillRect(renderer, &rect);
}

void GraphicsEngine::drawPoint(const Vector2I& p) {
    SDL_RenderDrawPoint(renderer, p.x, p.y);
}

void GraphicsEngine::drawPoint(const Vector2F& p) {
    SDL_RenderDrawPointF(renderer, p.x, p.y);
}

void GraphicsEngine::drawLine(const Line2i& line) {
    SDL_RenderDrawLine(renderer, line.start.x, line.start.y, line.end.x, line.end.y);
}

void GraphicsEngine::drawLine(const Line2f& line) {
    SDL_RenderDrawLineF(renderer, line.start.x, line.start.y, line.end.x, line.end.y);
}

void GraphicsEngine::drawLine(const Vector2I& p0, const Vector2I& p1) {
    SDL_RenderDrawLine(renderer, p0.x, p0.y, p1.x, p1.y);
}

void GraphicsEngine::drawLine(const Vector2F& p0, const Vector2F& p1) {
    SDL_RenderDrawLineF(renderer, p0.x, p0.y, p1.x, p1.y);
}

void GraphicsEngine::drawCircle(const Vector2F& center, const float& radius) {
    for (float i = 0.0f; i < 2 * M_PI; i += PI/180) {
        int x = (int)(center.x + radius * std::cos(i));
        int y = (int)(center.y + radius * std::sin(i));
        SDL_RenderDrawPoint(renderer, x, y);
    }
}

void GraphicsEngine::drawEllipse(const Vector2F& center, const float& radiusX, const float& radiusY) {
    for (float i = 0.0f; i < 2 * M_PI; i += PI/180) {
        int x = (int)(center.x + radiusX * std::cos(i));
        int y = (int)(center.y + radiusY * std::sin(i));
        SDL_RenderDrawPoint(renderer, x, y);
    }
}

void GraphicsEngine::drawPolygon(std::vector<Vector2F> points) {
    if (points.size() < 2) return; // There are not enough points to draw a line

    for (auto it = points.begin(); it != points.end(); ++it)
    {
        if (std::next(it) != points.end()) drawLine(*it, *std::next(it));
        else drawLine(*it, *points.begin());
    }
}

void GraphicsEngine::drawTexture(SDL_Texture* texture, SDL_Rect* src, SDL_Rect* dst, const double& angle, const SDL_Point* center, SDL_RendererFlip flip) {
    SDL_RenderCopyEx(renderer, texture, src, dst, angle, center, flip);
}

void GraphicsEngine::drawTexture(SDL_Texture* texture, SDL_Rect* dst, SDL_RendererFlip flip) {
    SDL_RenderCopyEx(renderer, texture, nullptr, dst, 0.0, nullptr, flip);
}