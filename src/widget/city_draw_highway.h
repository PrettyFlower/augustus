#ifndef CITY_DRAW_HIGHWAY_H
#define CITY_DRAW_HIGHWAY_H

void city_draw_highway_init(void);

int city_draw_highway_base_image_start(void);

int city_draw_highway_get_aqueduct_image(int grid_offset);

void city_draw_highway_footprint(int x, int y, float scale, int grid_offset);

#endif // CITY_DRAW_HIGHWAY_H
