#ifndef EMPIRE_XML_H
#define EMPIRE_XML_H

#include "core/buffer.h"

int empire_xml_parse_file(const char *filename);

void empire_xml_save_state(buffer *buf);

void empire_xml_load_state(buffer *buf);

#endif // EMPIRE_XML_H
