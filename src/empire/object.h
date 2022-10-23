#ifndef EMPIRE_OBJECT_H
#define EMPIRE_OBJECT_H

#include "core/buffer.h"
#include "empire/type.h"
#include "game/resource.h"

#define MAX_EMPIRE_OBJECTS 200

typedef struct {
    int id;
    empire_object_type type;
    int animation_index;
    int x;
    int y;
    int width;
    int height;
    int image_id;
    struct {
        int x;
        int y;
        int image_id;
    } expanded;
    int distant_battle_travel_months;
    int trade_route_id;
    int invasion_path_id;
    int invasion_years;
} empire_object;

typedef struct {
    int in_use;
    empire_city_type city_type;
    int city_name_id;
    uint8_t city_custom_name[50];
    int trade_route_open;
    int trade_route_cost;
    int city_sells_resource[10];
    int city_buys_resource[8];
    int city_sells_custom[RESOURCE_MAX];
    int city_buys_custom[RESOURCE_MAX];
    int trade40;
    int trade25;
    int trade15;
    empire_object obj;
} full_empire_object;

void empire_object_clear(void);

void empire_object_load(buffer *buf);

void empire_object_init_cities(void);

int empire_object_init_distant_battle_travel_months(int object_type);

const full_empire_object *full_empire_object_get(int object_id);

const empire_object *empire_object_get(int object_id);

const empire_object *empire_object_get_our_city(void);

const empire_object *empire_object_get_trade_city(int trade_route_id);

void empire_object_foreach(void (*callback)(const empire_object *));

const empire_object *empire_object_get_battle_icon(int path_id, int year);

int empire_object_get_max_invasion_path(void);

int empire_object_get_closest(int x, int y);

void empire_object_set_expanded(int object_id, int new_city_type);

int empire_object_city_buys_resource(int object_id, int resource);
int empire_object_city_sells_resource(int object_id, int resource);

void empire_object_city_force_sell_resource(int object_id, int resource);

int empire_object_update_animation(const empire_object *obj, int image_id);

int is_sea_trade_route(int route_id);

#endif // EMPIRE_OBJECT_H
