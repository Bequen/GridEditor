#pragma once

#include <cstdint>

#include "Editor/Scene.h"
#include "Editor/RenderInfo.h"
#include "Editor/Tiles/WindowTileInfo.h"

class WindowEditor {
    public:
        /**
         * @brief  Gets called when the instance is assigned to some window tile
         * @note   Gets called only once, use for initializing some data for future, per frame usage
         * @retval None
         */
        virtual void init() = 0;

        /**
         * @brief  Update => Gets called every frame, do NOT draw anything with this function, use draw function for that purpose
         * @note   Update function is useful to do some logic like moving, scrolling etc.
         * @retval None
         */
        virtual void update() = 0;

        /**
         * @brief  Draw function is called whenever the window have to be rendered
         * @note   Use this function to draw all your UI and in some cases scenes
         * @param  cursor: Where the mouse is currently pointing, not normalized on the current tile
         * @param  tileInfo: Information about the current tile
         * @retval None
         */
        virtual void draw(WindowTileInfo tileInfo) = 0;
        
        /**
         * @brief  Gets called whenever the user resizes the window
         * @note   This function should handle the resizing
         * @param  width: Width of the new window
         * @param  height: Height of the new window
         * @retval None
         */
        virtual void resize_callback(uint32_t width, uint32_t height) = 0;

        /**
         * @brief  Gets called when the window needs to be refreshed
         * @note   Example of need for refresh might be sudden change in scene etc.
         * @retval None
         */
        virtual void refresh() = 0;
};