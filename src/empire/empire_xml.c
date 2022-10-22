#include "empire_xml.h"

#include "assets/assets.h"
#include "core/file.h"
#include "core/image_group.h"
#include "core/log.h"
#include "core/string.h"
#include "empire/city.h"
#include "empire/object.h"

#include "expat.h"

#include <stdio.h>
#include <string.h>

#define XML_BUFFER_SIZE 4096

typedef enum {
    LIST_NONE = -1,
    LIST_BUYS = 1,
    LIST_SELLS = 2
} city_resource_list;

static struct {
    int success;
    int version;
    int next_empire_obj_id;
    int current_city_id;
    city_resource_list current_city_resource_list;
    int current_invasion_path_id;
    int invasion_path_ids[10];
    int invasion_path_idx;
} data;

static int count_xml_attributes(const char **attributes)
{
    int total = 0;
    while (attributes[total]) {
        ++total;
    }
    return total;
}

static void xml_parse_empire(int num_attrs, const char **attributes)
{
    for (int i = 0; i < num_attrs; i += 2) {
        char *attr_name = attributes[i];
        char *attr_val = attributes[i + 1];
        if (strcmp(attr_name, "version") == 0) {
            data.version = string_to_int(attr_val);
        }
    }

    if (!data.version) {
        data.success = 0;
        log_error("No version set", 0, 0);
        return;
    }
}

static void xml_parse_city(int num_attrs, const char **attributes)
{
    if (data.next_empire_obj_id + 1 >= MAX_EMPIRE_OBJECTS) {
        data.success = 0;
        log_error("Too many objects", 0, data.next_empire_obj_id);
        return;
    }

    full_empire_object *city_obj = full_empire_object_get(data.next_empire_obj_id);
    city_obj->obj.id = data.next_empire_obj_id;
    data.current_city_id = data.next_empire_obj_id;
    data.next_empire_obj_id++;
    city_obj->in_use = 1;
    city_obj->obj.type = EMPIRE_OBJECT_CITY;
    city_obj->city_type = EMPIRE_CITY_TRADE;
    city_obj->obj.trade_route_id = data.next_empire_obj_id;
    city_obj->trade_route_cost = 500;
    city_obj->obj.image_id = image_group(GROUP_EMPIRE_CITY_TRADE);
    city_obj->obj.width = 44;
    city_obj->obj.height = 36;
    data.next_empire_obj_id++;

    full_empire_object *route_obj = full_empire_object_get(city_obj->obj.trade_route_id);
    route_obj->obj.id = city_obj->obj.trade_route_id;
    route_obj->in_use = 1;
    route_obj->obj.type = EMPIRE_OBJECT_LAND_TRADE_ROUTE;
    route_obj->obj.trade_route_id = city_obj->obj.trade_route_id;
    route_obj->obj.image_id = image_group(GROUP_EMPIRE_TRADE_ROUTE_TYPE) + 1;

    for (int i = 0; i < num_attrs; i += 2) {
        char *attr_name = attributes[i];
        char *attr_val = attributes[i + 1];
        if (strcmp(attr_name, "name") == 0) {
            strcpy(city_obj->city_custom_name, attr_val);
        } else if (strcmp(attr_name, "x") == 0) {
            city_obj->obj.x = string_to_int(attr_val);
        } else if (strcmp(attr_name, "y") == 0) {
            city_obj->obj.y = string_to_int(attr_val);
        } else if (strcmp(attr_name, "ours") == 0 && strcmp(attr_val, "true") == 0) {
            city_obj->city_type = EMPIRE_CITY_OURS;
            city_obj->obj.image_id = image_group(GROUP_EMPIRE_CITY);
        } else if (strcmp(attr_name, "trade_route_cost") == 0) {
            city_obj->trade_route_cost = string_to_int(attr_val);
        } else if (strcmp(attr_name, "trade_by_sea") == 0) {
            route_obj->obj.type = EMPIRE_OBJECT_SEA_TRADE_ROUTE;
            route_obj->obj.image_id--;
        } else if (strcmp(attr_name, "distant") == 0 && strcmp(attr_val, "true") == 0) {
            city_obj->city_type = EMPIRE_CITY_DISTANT_ROMAN;
            city_obj->obj.image_id = assets_get_image_id("UI", "Village");
        }
    }
}

static resource_type string_to_resource(const char *name)
{
    if (strcmp(name, "wheat") == 0) {
        return RESOURCE_WHEAT;
    } else if (strcmp(name, "vegetables") == 0) {
        return RESOURCE_VEGETABLES;
    } else if (strcmp(name, "fruit") == 0) {
        return RESOURCE_FRUIT;
    } else if (strcmp(name, "olives") == 0) {
        return RESOURCE_OLIVES;
    } else if (strcmp(name, "vines") == 0) {
        return RESOURCE_VINES;
    } else if (strcmp(name, "meat") == 0 || strcmp(name, "fish") == 0) {
        return RESOURCE_MEAT;
    } else if (strcmp(name, "wine") == 0) {
        return RESOURCE_WINE;
    } else if (strcmp(name, "oil") == 0) {
        return RESOURCE_OIL;
    } else if (strcmp(name, "iron") == 0) {
        return RESOURCE_IRON;
    } else if (strcmp(name, "timber") == 0 || strcmp(name, "wood") == 0) {
        return RESOURCE_TIMBER;
    } else if (strcmp(name, "clay") == 0) {
        return RESOURCE_CLAY;
    } else if (strcmp(name, "marble") == 0) {
        return RESOURCE_MARBLE;
    } else if (strcmp(name, "weapons") == 0) {
        return RESOURCE_WEAPONS;
    } else if (strcmp(name, "furniture") == 0) {
        return RESOURCE_FURNITURE;
    } else if (strcmp(name, "pottery") == 0) {
        return RESOURCE_POTTERY;
    } else {
        return RESOURCE_NONE;
    }
}

static void xml_parse_resource(int num_attrs, const char **attributes)
{
    if (data.current_city_id == -1) {
        data.success = 0;
        log_error("No active city when parsing resource", 0, 0);
        return;
    } else if (data.current_city_resource_list == 0) {
        data.success = 0;
        log_error("Resource not in buy or sell tag", 0, 0);
        return;
    }
    full_empire_object *city_obj = full_empire_object_get(data.current_city_id);
    resource_type resource = RESOURCE_NONE;
    int amount = 0;
    for (int i = 0; i < num_attrs; i += 2) {
        char *attr_name = attributes[i];
        char *attr_val = attributes[i + 1];
        if (strcmp(attr_name, "type") == 0) {
            resource = string_to_resource(attr_val);
            if (resource == RESOURCE_NONE) {
                data.success = 0;
                log_error("Unable to determine resource type", attr_val, 0);
                return;
            }
        } else if (strcmp(attr_name, "amount") == 0) {
            amount = string_to_int(attr_val);
        }
    }

    if (resource == RESOURCE_NONE) {
        data.success = 0;
        log_error("Unable to find resource type attribute", 0, 0);
        return;
    }

    if (data.current_city_resource_list == LIST_BUYS) {
        city_obj->city_buys_custom[resource] = amount;
    } else if (data.current_city_resource_list == LIST_SELLS) {
        city_obj->city_sells_custom[resource] = amount;
    }
}

static void xml_parse_invasion_path()
{
    data.current_invasion_path_id++;
}

static void xml_parse_battle(int num_attrs, const char **attributes)
{
    if (data.next_empire_obj_id >= MAX_EMPIRE_OBJECTS) {
        data.success = 0;
        log_error("Too many objects", 0, data.next_empire_obj_id);
        return;
    } else if (!data.current_invasion_path_id) {
        data.success = 0;
        log_error("Battle not in path tag", 0, 0);
        return;
    } else if (data.invasion_path_idx >= sizeof(data.invasion_path_ids) / sizeof(data.invasion_path_ids[0])) {
        data.success = 0;
        log_error("Invasion path too long", 0, 0);
    }

    full_empire_object *battle_obj = full_empire_object_get(data.next_empire_obj_id);
    battle_obj->obj.id = data.next_empire_obj_id;
    data.next_empire_obj_id++;
    battle_obj->in_use = 1;
    battle_obj->obj.type = EMPIRE_OBJECT_BATTLE_ICON;
    battle_obj->obj.invasion_path_id = data.current_invasion_path_id;
    battle_obj->obj.image_id = image_group(GROUP_EMPIRE_BATTLE);
    data.invasion_path_ids[data.invasion_path_idx] = battle_obj->obj.id;
    data.invasion_path_idx++;
    for (int i = 0; i < num_attrs; i += 2) {
        char *attr_name = attributes[i];
        char *attr_val = attributes[i + 1];
        if (strcmp(attr_name, "x") == 0) {
            battle_obj->obj.x = string_to_int(attr_val);
        } else if (strcmp(attr_name, "y") == 0) {
            battle_obj->obj.y = string_to_int(attr_val);
        }
    }
}

static void XMLCALL xml_start_element(void *unused, const char *name, const char **attributes)
{
    if (!data.success) {
        return;
    }
    int num_attrs = count_xml_attributes(attributes);
    if (num_attrs % 2) {
        data.success = 0;
        log_error("Unexpected number of attributes", name, num_attrs);
        return;
    }
    if (strcmp(name, "empire") == 0) {
        xml_parse_empire(num_attrs, attributes);
    }
    if (!data.version) {
        data.success = 0;
        log_error("Not in empire tag", 0, 0);
        return;
    }
    if (strcmp(name, "city") == 0) {
        xml_parse_city(num_attrs, attributes);
    } else if (strcmp(name, "sells") == 0) {
        data.current_city_resource_list = LIST_SELLS;
    } else if (strcmp(name, "buys") == 0) {
        data.current_city_resource_list = LIST_BUYS;
    } else if (strcmp(name, "resource") == 0) {
        xml_parse_resource(num_attrs, attributes);
    } else if (strcmp(name, "path") == 0) {
        xml_parse_invasion_path();
    } else if (strcmp(name, "battle") == 0) {
        xml_parse_battle(num_attrs, attributes);
    }
}

static void XMLCALL xml_end_element(void *unused, const char *name)
{
    if (!data.success) {
        return;
    }

    if (strcmp(name, "city") == 0) {
        data.current_city_id = -1;
        data.current_city_resource_list = LIST_NONE;
    } else if (strcmp(name, "sells") == 0 || strcmp(name, "buys") == 0) {
        data.current_city_resource_list = LIST_NONE;
    } else if (strcmp(name, "path") == 0) {
        for (int i = 0; i < data.invasion_path_idx; i++) {
            int idx = data.invasion_path_idx - i - 1;
            full_empire_object *battle = full_empire_object_get(data.invasion_path_ids[idx]);
            battle->obj.invasion_years = i + 1;
        }
        memset(data.invasion_path_ids, 0, sizeof(data.invasion_path_ids));
        data.invasion_path_idx = 0;
    }
}

static void reset_data(void)
{
    data.success = 1;
    data.next_empire_obj_id = 0;
    data.current_city_id = -1;
    data.current_city_resource_list = LIST_NONE;
    data.current_invasion_path_id = 0;
    memset(data.invasion_path_ids, 0, sizeof(data.invasion_path_ids));
    data.invasion_path_idx = 0;
}

int empire_xml_parse_empire(const char *file_name)
{
    log_info("Loading empire file", file_name, 0);
    reset_data();

    FILE *file = file_open(file_name, "r");

    if (!file) {
        log_error("Error opening empire file", file_name, 0);
        return 0;
    }

    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetElementHandler(parser, xml_start_element, xml_end_element);

    char buffer[XML_BUFFER_SIZE];
    int done = 0;
    do {
        size_t bytes_read = fread(buffer, 1, XML_BUFFER_SIZE, file);
        done = bytes_read < sizeof(buffer);
        if (XML_Parse(parser, buffer, (int)bytes_read, done) == XML_STATUS_ERROR) {
            log_error("Error parsing file", file_name, 0);
            data.success = 0;
            break;
        }
    } while (!done && data.success);

    XML_ParserFree(parser);
    file_close(file);

    empire_object *our_city = empire_object_get_our_city();
    if (!our_city) {
        log_error("No home city specified", 0, 0);
        return 0;
    }

    for (int i = 0; i < MAX_EMPIRE_OBJECTS; i++) {
        full_empire_object *trade_city = full_empire_object_get(i);
        if (!trade_city->in_use) {
            break;
        }
        if (trade_city->obj.type != EMPIRE_OBJECT_CITY || trade_city->city_type == EMPIRE_CITY_OURS || !trade_city->obj.trade_route_id) {
            continue;
        }
        empire_object *trade_route = empire_object_get(trade_city->obj.trade_route_id);
        if (!trade_route) {
            continue;
        }
        trade_route->x = (our_city->x + trade_city->obj.x) / 2;
        trade_route->y = (our_city->y + trade_city->obj.y) / 2 + 16;
    }

    return data.success;
}
