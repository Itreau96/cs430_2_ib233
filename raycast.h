#ifndef RAYCAST
#define RAYCAST

#include "ib_3dmath.h"

#define RUN_SUCCESS 0
#define RUN_FAIL 1
#define MIN_ARGS 5
#define TRUE 1
#define FALSE 0

#define CAMERA 0
#define SPHERE 1
#define PLANE 2

#define CENTER_XY 0.0

// Type definitions
typedef int bool;
typedef struct rgb rgb;
typedef struct obj obj;
typedef struct obj_node obj_node;
typedef struct linked_list linked_list;

// Color in rgb format
struct rgb
{
   float r;
   float g;
   float b;
};

// Create one central object that contains all variables for any object
struct obj
{
   int type;
   float width;
   float height;
   rgb color;
   ib_v3 position;
   float radius;
   ib_v3 normal;
};

// Struct used used as a linked list node
struct obj_node
{
   obj_node *next;
   obj obj_ref;
};

// Linked list structure
struct linked_list
{
   obj_node *first;
   obj_node *last;
   obj_node *main_camera;
   int size;
};

// Forward declarations
void create_node(obj *data, linked_list *list);

#endif