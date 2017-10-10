#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ib_3dmath.h"
#include "raycast.h"
#include "parser.h"

// Forward declarations
void create_node(obj *data, linked_list *list);
void render(int *width, int *height, linked_list *objs);
void raycast(ib_v3 *r0, ib_v3 *rd, rgb *cur_rgb);
void sphere_intersection(ib_v3 *r0, ib_v3 *rd, obj *cur_obj, float *t);
void plane_intersection(ib_v3 *r0, ib_v3 *rd, obj *cur_obj, float *t);

int main(int argc, char* argv[])
{
   // Variable declarations
   int width;
   int height;
   linked_list objs;
   int run_result;

   // Relay error message if incorrect number of arguments were entered
   if (argc < MIN_ARGS)
   {
      // Subtract 1 from arguments entered, since 1st argument is the executable
      fprintf(stderr, "Incorrect number of arguments. You entered %d. You must enter 4. (err no. %d)\n", argc - 1, INPUT_INVALID);

      // Return error code
      return RUN_FAIL;
   }
   // If correct number of arguments supplied, validate the remaining arguments
   else
   {
      // Start by parsing file input
      parse(&objs, argv[3], &run_result);

      // Raycast objects if parse was successful
      if (run_result == RUN_SUCCESS)
      {
         obj_node *traverser = objs.first;
         for (int i = 0; i < objs.size; i++)
         {
            if (traverser->obj_ref.type == 0)
            {
               printf("%d width: %f, height: %f\n", traverser->obj_ref.type, traverser->obj_ref.width, traverser->obj_ref.height);
            }
            else if (traverser->obj_ref.type == 1)
            {
               printf("%d rgb: %f %f %f, pos: %f %f %f, radius:%f\n", traverser->obj_ref.type, traverser->obj_ref.color.r, traverser->obj_ref.color.g, traverser->obj_ref.color.b, traverser->obj_ref.position.x, traverser->obj_ref.position.y, traverser->obj_ref.position.z, traverser->obj_ref.radius);
            }
            else
            {
               printf("%d rgb: %f %f %f, pos: %f %f %f, normal:%f %f %f\n", traverser->obj_ref.type, traverser->obj_ref.color.r, traverser->obj_ref.color.g, traverser->obj_ref.color.b, traverser->obj_ref.position.x, traverser->obj_ref.position.y, traverser->obj_ref.position.z, traverser->obj_ref.normal.x, traverser->obj_ref.normal.y, traverser->obj_ref.normal.z);
            }

            traverser = traverser->next;
         }
      }
      // If parse was unsuccessful, display error message and code
      else
      {
         fprintf(stderr, "There was a problem parsing your input file. Please correct the file and try again. (err no. %d)\n", run_result);
      }
   }
   
   return RUN_SUCCESS;
}

// Used to render the scene given parsed objects
void render(int *width, int *height, linked_list *objs)
{
   // Variable declarations
   int rows;
   int cols;
   ib_v3 rd;
   rgb cur_rgb;
   float cam_width = objs->main_camera->obj_ref.width;
   float cam_height = objs->main_camera->obj_ref.height;
   double px_width = cam_width / *width;
   double px_height = cam_height / *width;
   ib_v3 r0 = { 0.0, 0.0, 0.0 }; // Initialize camera position
   int pz = 1; // Given distance from camera to viewport
   int py;
   int px;

   // Loop for as many columns in image
   for (cols = 0; cols < *height; cols++)
   {
      // Calculate py first
      py = CENTER_XY - cam_height  / 2.0 + px_height * (cols + 0.5);

      // Loop for as many rows as possible
      for (rows = 0; rows < *width; rows++)
      {
         // Calculate px
         px = CENTER_XY - cam_width / 2.0 + px_width * (rows + 0.5);

         // Combine variables into rd vector
         rd.x = px;
         rd.y = py;
         rd.z = pz;

         // Normalize the vector
         ib_v3_normalize(&rd);

         // Caycast with value         
         raycast(&r0, &rd, &cur_rgb);
      }
   }
}

// Method used to raycast given rd and set of objects
void raycast(ib_v3 *r0, ib_v3 *rd, rgb *cur_rgb)
{
   float a = 5.0;
}

// Method used to find sphere intersection
void sphere_intersection(ib_v3 *r0, ib_v3 *rd, obj *cur_obj, float *t)
{
   // Variable declarations
   float a;
   float b;
   float c;
   float d;
   float t0;
   float t1;
   
   // Calculate a, b, and c values
   a = (rd->x * rd->x) + (rd->y * rd->y) + (rd->z * rd->z);
   b = 2 * (rd->x * (r0->x - cur_obj->position.x) + rd->y * (r0->y - cur_obj->position.y) + rd->z * (r0->z - cur_obj->position.z));
   c = ((r0->x - cur_obj->position.x) * (r0->x - cur_obj->position.x) + 
        (r0->y - cur_obj->position.y) * (r0->y - cur_obj->position.y) + 
        (r0->z - cur_obj->position.z) * (r0->z - cur_obj->position.z)) - (cur_obj->radius * cur_obj->radius);

   // Calculate descriminate value
   d = (b * b - 4 * a * c);

   // Only if descriminate is positive do we calculate intersection
   if (d > 0)
   {
      // Calculate both t values
      t0 = (-b + sqrtf(b * b - 4 * c * a)) / (2 * a);
      t1 = (-b - sqrtf(b * b - 4 * c * a)) / (2 * a);

      // Determine which t value to return
      if (t1 > 0)
      {
         *t = t1;
      }
      else if (t0 > 0)
      {
         *t = t0;
      }
   }
}

// Method used to find plane intersection
void plane_intersection(ib_v3 *r0, ib_v3 *rd, obj *cur_obj, float *t)
{
   // Variable declarations
   float a;
   float b;
   float c;
   float dist;
   float den;
   
   // Normalize normal value and assign a, b, and c (for readability)
   ib_v3_normalize(&(cur_obj->normal));
   a = cur_obj->normal.x;
   b = cur_obj->normal.y;
   c = cur_obj->normal.z; 

   // Calculate dist and den values
   dist = -(a * cur_obj->position.x + b * cur_obj->position.y + c * cur_obj->position.z);
   den = (a * rd->x + b * rd->y + c * rd->z);

   // Set the new t value
   *t = -(a * r0->x + b * r0->y + c * r0->z) / den;
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
      
      // Set node data
      list->first->obj_ref = *data;
      
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

