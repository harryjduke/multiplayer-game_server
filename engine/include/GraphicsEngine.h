// This file is part of multiplayer-game_server <https://github.com/harryjduke/multiplayer-game_server>.
// Copyright (c) 2024 Harry Duke <harryjduke@gmail.com>
// This file includes modifications made by Harry Duke.
//
// This program is distributed under the terms of the GNU General Public License version 2.
// You should have received a copy of the GNU General Public License along with this program.
// If not, see <https://github.com/harryjduke/multiplayer-game_server/blob/main/LICENSE> or <https://www.gnu.org/licenses/>.

//
//  GraphicsEngine.hpp
//  GameEngineBase
//
// all code resources are taken from https://github.com/AlmasB/xcube2d/tree/master
//

#ifndef GRAPHICS_ENGINE_H
#define GRAPHICS_ENGINE_H

//#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
//code from xcube 2d
#include <SDL.h>

#include "utils/EngineCommon.h"
#include "utils/GameMath.h"

/* ENGINE DEFAULT SETTINGS */
static const int DEFAULT_WINDOW_WIDTH = 800;
static const int DEFAULT_WINDOW_HEIGHT = 600;

static const SDL_Color SDL_COLOR_GRAY = { 0x80, 0x80, 0x80 };
static const SDL_Color SDL_COLOR_YELLOW = { 0xFF, 0xFF, 0 };
static const SDL_Color SDL_COLOR_RED = { 0xFF, 0, 0 };
static const SDL_Color SDL_COLOR_GREEN = { 0, 0xFF, 0 };
static const SDL_Color SDL_COLOR_BLUE = { 0, 0, 0xFF };
static const SDL_Color SDL_COLOR_BLACK = { 0, 0, 0 };
static const SDL_Color SDL_COLOR_WHITE = { 0xFF, 0xFF, 0xFF };
static const SDL_Color SDL_COLOR_AQUA = { 0, 0xFF, 0xFF };
static const SDL_Color SDL_COLOR_ORANGE = { 0xFF, 0xA5, 0 };
static const SDL_Color SDL_COLOR_PINK = { 0xFF, 0xC0, 0xCB };
static const SDL_Color SDL_COLOR_PURPLE = { 0x80, 0, 0x80 };
static const SDL_Color SDL_COLOR_VIOLET = { 0xEE, 0x82, 0xEE };

inline SDL_Color getRandomColor(int minRGB, int maxRGB)
{
    SDL_Color color = { (Uint8)getRandom(minRGB, maxRGB), (Uint8)getRandom(minRGB, maxRGB), (Uint8)getRandom(minRGB, maxRGB) };
    return color;
}

struct SDL_Colorf
{
    float r, g, b, a;
};

inline SDL_Colorf toSDLColorf(SDL_Color color)
{
    SDL_Colorf colorf = {
            static_cast<float>(color.r) / 255.0f,
            static_cast<float>(color.g) / 255.0f,
            static_cast<float>(color.b) / 255.0f,
            static_cast<float>(color.a) / 255.0f };
    return colorf;
}

inline SDL_Color toSDLColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_Color color = { r, g, b, a };
    return color;
}

class GraphicsEngine
{
    friend class XCube2Engine;
private:
    SDL_Window* window;
    static SDL_Renderer* renderer;
    SDL_Color drawColor;


    Uint32 fpsAverage, fpsPrevious, fpsStart, fpsEnd;

    GraphicsEngine();

public:
    ~GraphicsEngine();

    /**
    * Clears everything on the screen
    * Call this before drawing anything to screen
    */
    void clearScreen() const;

    /**
    * Displays everything rendered on the screen
    * Call this method after you have finished drawing
    */
    static void showScreen();

    static void drawRect(const SDL_Rect&);
    void drawRect(const SDL_Rect&, const SDL_Color&) const;
    void drawRect(SDL_Rect*, const SDL_Color&) const;
    static void drawRect(SDL_Rect*);
    static void drawRect(const int& x, const int& y, const int& w, const int& h);

    static void fillRect(SDL_Rect*);
    static void fillRect(const int& x, const int& y, const int& w, const int& h);

    static void drawPoint(const Vector2I &p);
    static void drawPoint(const Vector2F&);

    static void drawLine(const Line2i&);
    static void drawLine(const Line2f&);
    static void drawLine(const Vector2I& start, const Vector2I& end);
    static void drawLine(const Vector2F& start, const Vector2F& end);

    static void drawCircle(const Vector2F& center, const float& radius);

    static void drawEllipse(const Vector2F& center, const float& radiusX, const float& radiusY);

    static void drawPolygon(std::vector<Vector2F> points);

    static void drawTexture(SDL_Texture*, SDL_Rect* src, SDL_Rect* dst, const double& angle = 0.0, const SDL_Point* center = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);
    static void drawTexture(SDL_Texture*, SDL_Rect* dst, SDL_RendererFlip flip = SDL_FLIP_NONE);


    void setDrawColor(const SDL_Color&);
    static void setDrawScale(const Vector2F&);    // not tested

    /**
    * @param fileName - name of the icon file
    */
    void setWindowIcon(const char* fileName);
    void setWindowSize(const int&, const int&);
    void setWindowTitle(const char* title);
    void setWindowTitle(const std::string&);
    void setFullscreen(bool);
    static void setVerticalSync(bool);

    /**
    * Shows a message box with given info and title
    *
    * Note: this function will block the execution on
    * thread where it was called
    *
    * @param info - the info to be shown
    * @param title - title of the message box, may be left out
    */
    void showInfoMessageBox(const std::string& info, const std::string& title = "");

    /**
    * @return current window's dimension
    */
    Dimension2i getCurrentWindowSize();

    /**
    * @return current display mode's resolution
    *         since most displays use native (max) resolution
    *         this returns maximum available (the one you can set) window size
    */
    static Dimension2i getMaximumWindowSize();

    void setFrameStart();
    void adjustFPSDelay(const Uint32&);
    [[nodiscard]] Uint32 getAverageFPS() const;

    static SDL_Texture* createTextureFromSurface(SDL_Surface*);

};

typedef GraphicsEngine GFX;

#endif