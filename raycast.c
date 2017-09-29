#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ib_3dmath.h"

#define RUN_SUCCESS 0
#define RUN_FAIL 1
#define MIN_ARGS 5
#define TRUE 1
#define FALSE 0

#define WIDTH_INVALID 1
#define HEIGHT_INVALID 2
#define INPUT_INVALID 3
#define OUTPUT_INVALID 4

#define CAMERA 0
#define SPHERE 1
#define PLANE 2

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
   int size;
};

// Forward declarations
void create_node(obj *data, linked_list *list);
void parse();

int main(int argc, char* argv[])
{
   // Variable declarations
   int width;
   int height;
   linked_list objs;
   char *file_in_name;
   char *file_out_name;
   obj data;   
   int run_result;
   bool valid = TRUE;

   // Relay error message if incorrect number of arguments were entered
   if (argc < MIN_ARGS)
   {
      // Subtract 1 from arguments entered, since 1st argument is the executable
      printf("Incorrect number of arguments. You entered %d. You must enter 4.\n", argc - 1);

      // Return error code
      return RUN_FAIL;
   }
   // If correct number of arguments supplied, validate the remaining arguments
   else
   {
      /*
      create_node(&data, &objs);
      create_node(&data, &objs);
      create_node(&data, &objs);

      obj_node *traverser = objs.first;
      for (int i = 0; i < objs.size; i++)
      {
         printf("%d\n", i);
         traverser = traverser->next;
      }
      */
   }
   
   return RUN_SUCCESS;
}

// Helper function used to append a new node to the end of a linked list
void create_node(obj *data, linked_list *list)
{
   // First, determine if this is first node in linked list
   if (list->size == 0)
   {
      // Last and first are both the same
      list->first = malloc(sizeof(obj_node));
      list->last = list->first;

      // Allocate size for next object
      list->last->next = malloc(sizeof(obj_node));
      
      // Increase size
      list->size = 1;
   }
   // If not first, simply append object
   else
   {
      // Apply data to new last node
      list->last->next->obj_ref = *data;
      list->last->next->next = malloc(sizeof(obj_node));

      // Increase size of linked list
      list->last = list->last->next;
      list->size = list->size + 1;
   }
}



