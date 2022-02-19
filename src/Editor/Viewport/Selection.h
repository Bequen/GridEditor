#pragma once

#include <cstdint>
#include <err.h>
#include <avg/Debug.h>

#include "Editor/Scene/BitGrid.h"

#define DEFAULT_SELECTION_SIZE (SUBGRID_SIZE * SUBGRID_SIZE)

/**
 * @brief Selection of voxels
 * Selection works by saving a second version of the entire grid
 * and stores boolean in bit array to save space
 */
struct Selection {
    public:
        BitGrid grid;

    private:
        void
        allocate(uint32_t size);

    public:
        Selection();
        Selection(uint32_t size);


        inline int
        select(uint32_t index) {

        }


        inline void
        unselect(uint32_t index) {

        }

        inline bool
        is_selected(uint32_t index) {

        }

        /* inline uint32_t
        get_size() {
            return size;
        }

        inline uint32_t
        get_at_index(uint32_t index) {
            return selection[index];
        }

        uint32_t
        operator[](uint32_t index) const {
            return selection[index];
        }

        inline void
        terminate() {
            delete [] selection;
        } */
};
