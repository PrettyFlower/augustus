#include "city_draw_highway.h"

#include "assets/assets.h"
#include "building/building.h"
#include "graphics/image.h"
#include "map/aqueduct.h"
#include "map/building.h"
#include "map/grid.h"
#include "map/random.h"
#include "map/terrain.h"

static int highway_wall_direction_offsets[4] = { 1, -GRID_SIZE, -1, GRID_SIZE };
static int highway_base_start;
static int highway_barrier_start;
static int aqueduct_with_water;
static int aqueduct_without_water;

void city_draw_highway_init(void)
{
    highway_base_start = assets_get_image_id("Logistics", "Highway_Base_Start");
    highway_barrier_start = assets_get_image_id("Logistics", "Highway_Barrier_Start");
    aqueduct_with_water = assets_get_image_id("Logistics", "Aqueduct_Bridge_Left_Water");
    aqueduct_without_water = assets_get_image_id("Logistics", "Aqueduct_Bridge_Left_Empty");
}

int city_draw_highway_base_image_start(void)
{
    return highway_base_start;
}

static int is_highway_access(int grid_offset)
{
    if (map_terrain_is(grid_offset, TERRAIN_HIGHWAY | TERRAIN_ROAD | TERRAIN_GATEHOUSE | TERRAIN_ACCESS_RAMP)) {
        return 1;
    }
    if (map_terrain_is(grid_offset, TERRAIN_BUILDING)) {
        const building *b = building_get(map_building_at(grid_offset));
        if (b->type == BUILDING_GRANARY) {
            return grid_offset == b->grid_offset + map_grid_delta(1, 0) ||
                grid_offset == b->grid_offset + map_grid_delta(0, 1) ||
                grid_offset == b->grid_offset + map_grid_delta(2, 1) ||
                grid_offset == b->grid_offset + map_grid_delta(1, 2);
        }
    }
    return 0;
}

int city_draw_highway_get_aqueduct_image(int grid_offset)
{
    int aqueduct_image_id = aqueduct_with_water;
    if (map_terrain_is(grid_offset - 1, TERRAIN_AQUEDUCT) || map_terrain_is(grid_offset + 1, TERRAIN_AQUEDUCT)) {
        aqueduct_image_id++;
    }
    if (!map_aqueduct_has_water_access_at(grid_offset)) {
        aqueduct_image_id += 2;
    }
    return aqueduct_image_id;
}

void city_draw_highway_footprint(int x, int y, float scale, int grid_offset)
{
    int random_offset = map_random_get(grid_offset) & 15;
    int base_image_id = highway_base_start + random_offset;
    image_draw_isometric_footprint_from_draw_tile(base_image_id, x, y, 0, scale);
    for (int d = 0; d < 4; d++) {
        int direction_offset = grid_offset + highway_wall_direction_offsets[d];
        // should this side have a wall?
        if (!is_highway_access(direction_offset)) {
            int wall_image_id = highway_barrier_start + d;
            image_draw_isometric_footprint_from_draw_tile(wall_image_id, x, y, 0, scale);
        }
    }
    if (!map_terrain_is(grid_offset, TERRAIN_AQUEDUCT)) {
        return;
    }
    int aqueduct_image_id = city_draw_highway_get_aqueduct_image(grid_offset);
    image_draw_isometric_footprint_from_draw_tile(aqueduct_image_id, x, y, 0, scale);
}
