#pragma once

#include <cstdint>

typedef uint32_t DrawMode;
typedef uint32_t ShapeMode;
typedef uint32_t BrushMode;
typedef uint32_t SelectMode;

#define SHAPE_CUBE                  0x0000
#define SHAPE_LINE                  0x0001
#define SHAPE_CIRCLE                0x0002

#define DRAW_MODE_BRUSH             0x0000
#define DRAW_MODE_SHAPE             0x0001
#define DRAW_MODE_EXTRUDE           0x0002
#define DRAW_MODE_SELECTION         0x0003

#define BRUSH_MODE_ADD              0x0000
#define BRUSH_MODE_SUBSTRACT        0x0001
#define BRUSH_MODE_PAINT            0x0002

#define SELECT_MODE_BOX             0x0000
