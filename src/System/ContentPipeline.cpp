#include "ContentPipeline.h"

#include <iostream>
#include <cstring>

#include <avg/Debug.h>

#define FORMAT_RGB      0x0001
#define FORMAT_RGBA     0x0002

#define GRID_FILE_CATEGORY_GRIDS        0x01
#define GRID_FILE_CATEGORY_SCENE_GRAPH  0x02
#define GRID_FILE_CATEGORY_LIGHTS       0x03

#define GRID_FILE_VALUE_SIZE            0x01
#define GRID_FILE_VALUE_CATEGORY_SIZE   0x02
#define GRID_FILE_VALUE_CATEGORY_COUNT  0x03
#define GRID_FILE_VALUE_GRID            0x04
#define GRID_FILE_VALUE_VALUE_COUNT     0x05
#define GRID_FILE_VALUE_OBJECT_ID       0x06
#define GRID_FILE_VALUE_CHILD_COUNT     0x07
#define GRID_FILE_VALUE_CHILDREN        0x08
#define GRID_FILE_VALUE_OBJECT_TYPE     0x09
#define GRID_FILE_VALUE_DATA_OFFSET     0x0a
#define GRID_FILE_VALUE_NAME            0x0b

string256 ContentPipeline::asset_path(char* dir, char* name, char* extension) {
    string256 path;
    uint32_t length = readlink("/proc/self/exe", path.str, 256);
    path.length += length - strlen(PROJECT_NAME);

    path = string256_concat(path, "assets/");
    path = string256_concat(path, dir);
    path = string256_concat(path, name);
    path = string256_concat(path, extension);

    return path;
}

char* ContentPipeline::load_text(char* path, uint32_t* length) {
    std::ifstream stream = std::ifstream(path, std::ios::ate | std::ios::binary);

    if(!stream.is_open()) {
        ERROR("Couldn't open file!");
        return nullptr;
    }

    uint32_t textLength = stream.tellg();
    stream.seekg(0);

    char* result = new char[textLength + 1];
    stream.read(result, textLength);
    result[textLength] = '\0';

    if(length)
        *length = textLength;

    return result;
}

void ContentPipeline::save_grid(Scene scene, char* path) {
    std::ofstream stream = std::ofstream(path, std::ios::out | std::ios::binary);
    if(!stream.is_open()) {
        ERROR("Cannot open stream, saving failed");
        return;
    }

    uint32_t categoryCount = 2;
    stream.put(GRID_FILE_VALUE_CATEGORY_COUNT);
    stream.write((char*)&categoryCount, 4);


    #pragma region SAVE GRIDS
    stream.put(GRID_FILE_CATEGORY_GRIDS);
    stream.write("{", 1);

    stream.put(GRID_FILE_VALUE_CATEGORY_SIZE);
    stream.write((char*)&scene.gridCount, 4);

    for(uint32_t i = 0; i < scene.gridCount; i++) {
        stream.write("{", 1);

        uint32_t valueCount = 2;
        stream.put(GRID_FILE_VALUE_VALUE_COUNT);
        stream.write((char*)&valueCount, 4);

        stream.put(GRID_FILE_VALUE_SIZE);
        stream.write((char*)&scene.grids[i].width, 4);
        stream.write((char*)&scene.grids[i].depth, 4);
        stream.write((char*)&scene.grids[i].height, 4);

        stream.put(GRID_FILE_VALUE_GRID);
        stream.write((char*)&scene.grids[i].buffer, scene.grids[i].width * scene.grids[i].depth * scene.grids[i].height);

        stream.write("}", 1);
    }

    stream.write("}", 1);
    
    #pragma endregion

    #pragma region SAVE LIGHTS
    #pragma endregion

    #pragma region SAVE SCENE GRAPH
    stream.put(GRID_FILE_CATEGORY_SCENE_GRAPH);
    stream.put('{');

    save_scene_object(&scene, &scene.sceneGraph, &stream);

    stream.put('}');
    #pragma endregion

    stream.write("\0", 1);
    stream.close();
}

void ContentPipeline::save_scene_object(const Scene* scene, const SceneObject* sceneObject, std::ofstream* stream) {
    stream->put('{');

    stream->put(GRID_FILE_VALUE_VALUE_COUNT);
    uint32_t valueCount = 5;
    stream->write((char*)&valueCount, 4);

    stream->put(GRID_FILE_VALUE_OBJECT_TYPE);
    stream->write((char*)&sceneObject->type, 4);

    stream->put(GRID_FILE_VALUE_NAME);
    uint32_t l = strlen(sceneObject->name);
    stream->write((char*)&l, 4);
    stream->write(sceneObject->name, l);

    stream->put(GRID_FILE_VALUE_DATA_OFFSET);
    uint32_t offset = 0;
    switch(sceneObject->type) {
        case SCENE_GRID: {
            offset = ((uint64_t)sceneObject->data - (uint64_t)scene->grids) / sizeof(Grid);
            break;
        } case SCENE_LIGHT: {
            offset = ((uint64_t)sceneObject->data - (uint64_t)scene->lights) / sizeof(Light);
            break;
        } default: {
            ERROR("Undefined object type!");
            break;
        }
    }
    stream->write((char*)&offset, 4);

    stream->put(GRID_FILE_VALUE_CHILD_COUNT);
    stream->write((char*)&sceneObject->childrenCount, 4);
    stream->put(GRID_FILE_VALUE_CHILDREN);
    for(uint32_t i = 0; i < sceneObject->childrenCount; i++) {
        ContentPipeline::save_scene_object(scene, &sceneObject->children[i], stream);
    }

    stream->put('}');
}

SceneObject ContentPipeline::load_scene_object(Scene* scene, std::ifstream* stream) {
    stream->get();

    uint32_t valueCount = 0;
    if(stream->get() == GRID_FILE_VALUE_VALUE_COUNT) {
        stream->read((char*)&valueCount, 4);
    } else {
        ERROR("Value count was not specified!");
    }

    SceneObject result = SceneObject();

    for(uint32_t i = 0; i < valueCount; i++) {
        switch(stream->get()) {
            case GRID_FILE_VALUE_OBJECT_TYPE: {
                stream->read((char*)&result.type, 4);
                break;
            } case GRID_FILE_VALUE_DATA_OFFSET: {
                uint32_t offset = 0;
                stream->read((char*)&offset, 4);

                switch(result.type) {
                    case SCENE_GRID: {
                        result.data = (void*)&scene->grids[offset];
                        break;
                    } case SCENE_LIGHT: {
                        result.data = (void*)&scene->lights[offset];
                        break;
                    }
                }
                break;
            } case GRID_FILE_VALUE_NAME: {
                uint32_t l = 0;
                stream->read((char*)&l, 4);

                result.name = new char[l + 1];
                stream->read(result.name, l);
                result.name[l] = '\0';
                break;
            } case GRID_FILE_VALUE_CHILD_COUNT: {
                uint32_t childCount = 0;
                stream->read((char*)&childCount, 4);

                result.children = new SceneObject[childCount];
                result.childrenBufferSize = childCount;
                MESSAGE("Child count: " << childCount);
                break;
            } case GRID_FILE_VALUE_CHILDREN: {
                for(uint32_t i = 0; i < result.childrenBufferSize; i++) {
                    result.children[result.childrenCount++] = load_scene_object(scene, stream);
                }
                break;
            }
        }
    }

    stream->get();

    return result;
}

Scene ContentPipeline::load_grid(char* path) {
    std::ifstream stream = std::ifstream(path, std::ios::in | std::ios::binary);
    if(!stream.is_open()) {
        ERROR("File at " << path << " doesn't exist, loading failed");
        return Scene();
    }

    Scene result;

    uint32_t categoryCount = 0;
    if(stream.get() == GRID_FILE_VALUE_CATEGORY_COUNT)
        stream.read((char*)&categoryCount, 4);

    MESSAGE("Category Count " << categoryCount)

    for(uint32_t c = 0; c < categoryCount; c++) {
        char* buffer = new char[256];
        switch(stream.get()) {
            case GRID_FILE_CATEGORY_GRIDS: {
                // Solve grids category
                if(stream.get() == '{') {
                    uint32_t gridCount = 0;
                    if(stream.get() == GRID_FILE_VALUE_CATEGORY_SIZE) {
                        stream.read((char*)&gridCount, 4);
                        MESSAGE("Grid count: " << gridCount);
                    }

                    result.init(gridCount);
                    result.gridCount = 0;
                    MESSAGE("Current grid count: " << result.gridCount);

                    for(uint32_t i = 0; i < gridCount; i++) {
                        Grid grid;

                        uint32_t width = 0;
                        uint32_t depth = 0;
                        uint32_t height = 0;

                        if(stream.get() == '{') {
                            uint32_t valueCount = 0;
                            if(stream.get() == GRID_FILE_VALUE_VALUE_COUNT)
                                stream.read((char*)&valueCount, 4);

                            for(uint32_t v = 0; v < valueCount; v++) {
                                switch(stream.get()) {
                                    case GRID_FILE_VALUE_SIZE: {
                                        stream.read((char*)&width, 4);
                                        stream.read((char*)&depth, 4);
                                        stream.read((char*)&height, 4);

                                        grid = Grid(width, depth, height);

                                        MESSAGE("Grid size: " << width << ":" << depth << ":" << height);
                                        break;
                                    } case GRID_FILE_VALUE_GRID: {
                                        if(width + depth + height == 0) {
                                            ERROR("You are trying to load grid data but grid's size have not been declared yet!");
                                            break;
                                        }
                                        stream.read((char*)grid.buffer, width * depth * height);
                                        break;
                                    } default: {
                                        ERROR("Unknown value in file");
                                        break;
                                    }
                                }
                            }

                            result.add_grid(grid);
                        } stream.get();
                    }
                } stream.get();
                break;
            } case GRID_FILE_CATEGORY_SCENE_GRAPH: {
                MESSAGE("Loading scene graph");
                if(stream.get() == '{') {
                    result.sceneGraph = load_scene_object((&result), &stream);
                } stream.get();
                break;
            } default: {
                ERROR("Undefined category");
                break;
            }
        } 
    }

    if(stream.get() == '\0') {
        SUCCESS("File was read successfully");
    }

    stream.close();

    return result;
}

