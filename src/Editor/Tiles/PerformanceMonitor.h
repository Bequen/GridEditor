#pragma once

#include <cstdint>
#include <vector>

#include "WindowTile.h"
#include "Editor/PerformanceStat.h"

#define REPORT_INTERVAL 1.0

struct Profiler {
    PerformanceStat stats[10];
    uint32_t count = 0;
};

extern Profiler profiler;

class PerformanceMonitor : public WindowTile {
    public:
        double elapsedTime;
        double* deltaTime;

        PerformanceMonitor(Scene* scene, double* deltaTime) :
        WindowTile(scene), deltaTime(deltaTime) {
            elapsedTime = 0.0;
        }

        void init();
        void update();
        void terminate();

        void refresh();

        void draw(Cursor cursor, WindowTileInfo tileInfo);
        void resize_callback(uint32_t width, uint32_t height);
};