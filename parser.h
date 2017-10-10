#ifndef PARSER
#define PARSER

#include "raycast.h"

#define LINE_TERM '\n'
#define PROP_SEP ':'
#define VALUE_SEP ','
#define STR_END '\0'
#define V3_START '['
#define V3_END ']'
#define TYPE_LEN 7
#define PROPERTY_LEN 11
#define VALUE_LEN 11

#define CAM_VAL_COUNT 2
#define SPHERE_VAL_COUNT 3
#define PLANE_VAL_COUNT 3

#define WIDTH_INVALID 1
#define HEIGHT_INVALID 2
#define INPUT_INVALID 3
#define OUTPUT_INVALID 4

// Public function declarations
void parse(linked_list *list, char *file_name, int *result);

#endif
