#include "PerformanceMonitor.h"

#include "ImGUI/imgui.h"

Profiler profiler;

void PerformanceMonitor::init() {

}

void PerformanceMonitor::update() {

}

void PerformanceMonitor::terminate() {

}

void PerformanceMonitor::draw(Cursor cursor, WindowTileInfo tileInfo) {
    if(deltaTime != nullptr) {
        elapsedTime += *deltaTime;
    }
    
    if(elapsedTime > REPORT_INTERVAL) {
        for(uint32_t i = 0; i < profiler.count; i++) {
            #define p profiler.stats[i]
            p.time[p.index++ % p.bufferSize] = p.sum / p.count;
            p.count = 0; p.sum = 0.0;
            
        }
        elapsedTime = 0.0;
    }
    for(uint32_t i = 0; i < profiler.count; i++) {
        //ERROR(i << " : " << profiler.stats[i].sum);
        ImGui::PlotLines(profiler.stats[i].name, profiler.stats[i].time, profiler.stats[i].bufferSize);
    }
}

void PerformanceMonitor::resize_callback(uint32_t width, uint32_t height) {

}

void PerformanceMonitor::refresh() {
    
}