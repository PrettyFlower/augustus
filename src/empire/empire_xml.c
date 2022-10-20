#include "empire_xml.h"

#include "core/file.h"
#include "core/log.h"
#include "empire/city.h"
#include "empire/object.h"

#include "expat.h"

#include <stdio.h>
#include <string.h>

#define XML_BUFFER_SIZE 4096

static struct {
    int success;
    int current_city_id;
} data;

static void XMLCALL xml_start_element(void *unused, const char *name, const char **attributes)
{
    if (!data.success) {
        return;
    }
    if (strcmp(name, "city")) {
        data.current_city_id++;
        if (data.current_city_id >= EMPIRE_CITY_MAX_CITIES) {
            data.success = 0;
            log_error("Too many cities", 0, data.current_city_id);
        }
        return;
    }
}

static void XMLCALL xml_end_element(void *unused, const char *name)
{
    if (!data.success) {
        return;
    }

}

static void reset_data(void)
{
    data.success = 1;
    data.current_city = 0;
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
        size_t bytes_read = fread(buffer, 1, XML_BUFFER_SIZE, xml_file);
        done = bytes_read < sizeof(buffer);
        if (XML_Parse(parser, buffer, (int)bytes_read, done) == XML_STATUS_ERROR) {
            log_error("Error parsing file", file_name, 0);
            data.success = 0;
            break;
        }
    } while (!done && data.success);

    XML_ParserFree(parser);
    file_close(file);

    return data.success;
}
