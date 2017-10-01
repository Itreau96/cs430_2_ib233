#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "ib_3dmath.h"

#define RUN_SUCCESS 0
#define RUN_FAIL 1
#define MIN_ARGS 5
#define TRUE 1
#define FALSE 0

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
void parse(linked_list *list, char *file_name, int *result);
void store_obj_properties(obj *cur_obj, FILE *file, char *c, int *result);
void get_next_word(char *word, char delim, int max_len, FILE *file, char *c);
void get_camera(obj *cur_obj, FILE *file, char *c, int *result);
void get_sphere(obj *cur_obj, FILE *file, char *c, int *result);
void get_plane(obj *cur_obj, FILE *file, char *c, int *result);

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
      // If parse was unsuccessful, display error
      else
      {
         printf("fail\n");
      }
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

// Method used to parse out objects in the input file
void parse(linked_list *list, char *file_name, int *result)
{
   // Variable declarations
   FILE *file;
   char c;
   obj cur_obj;

   // Start by attemping to open the file
   file = fopen(file_name, "r");

   // Check if file exists
   if (file)
   {
      // Read file character by character until file ends
      while ((c = fgetc(file)) != EOF)
      {
         // Store object properties
         store_obj_properties(&cur_obj, file, &c, result);

         // Confirm that store operation worked correctly
         if (*result == RUN_SUCCESS)
         {
            // Create node and append to list
            create_node(&cur_obj, list);
         }

         // If not, execution halts
      }

      // Close file
      fclose(file);
   }
   // If not, return error code
   else
   {
      *result = INPUT_INVALID;
   }
}

// Helper method used to store object properties 
void store_obj_properties(obj *cur_obj, FILE *file, char *c, int *result)
{
   // Variable declarations
   char obj_type[TYPE_LEN];

   // Start by retrieving object type
   get_next_word(obj_type, VALUE_SEP, TYPE_LEN, file, c);

   // Store data based on object type
   if (strcmp(obj_type, "camera") == 0)
   {
      // Store object type
      cur_obj->type = CAMERA;

      // Store object properties
      get_camera(cur_obj, file, c, result);
   }
   else if (strcmp(obj_type, "sphere") == 0)
   {
      // Store object type
      cur_obj->type = SPHERE;

      // Store object variables
      get_sphere(cur_obj, file, c, result);
   }
   else if (strcmp(obj_type, "plane") == 0)
   {
      // Store object type
      cur_obj->type = PLANE;

      // Store object variables
      get_plane(cur_obj, file, c, result);
   }
   else
   {
      *result = INPUT_INVALID;
   }   
}

// Helper method used to store camera object variables
void get_camera(obj *cur_obj, FILE *file, char *c, int *result)
{
   // Variable declarations
   char property[PROPERTY_LEN];
   char value[VALUE_LEN];
   bool width_found = FALSE;
   bool height_found = FALSE;
   int prop_count = 0;

   // Store object type
   cur_obj->type = CAMERA;

   // Store object variables
   while (*c != LINE_TERM && *c != EOF)
   {
      // Get next word in line
      get_next_word(property, PROP_SEP, PROPERTY_LEN, file, c);

      // Compare property value
      if (strcmp(property, "width") == 0)
      {
         // Get property value
         get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

         // Set property value
         cur_obj->width = atof(value);

         // Set boolean value
         width_found = TRUE;

         // Increment the prop count
         prop_count++;
      }
      else if (strcmp(property, "height") == 0)
      {
         // Get property value
         get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

         // Set property value
         cur_obj->height = atof(value);

         // Set boolean value
         height_found = TRUE;

         // Increment the prop count
         prop_count++;
      }
      else
      {
         *result = INPUT_INVALID;
      }

      // Clear values
      property[0] = STR_END;
      value[0] = STR_END;
   }

   // Return error code if height or width was not found
   if (width_found == TRUE && height_found == TRUE && *result != INPUT_INVALID && prop_count <= CAM_VAL_COUNT)
   {
      *result = RUN_SUCCESS;
   }
   else
   {
      *result = INPUT_INVALID;
   }
}

// Helper method used to store camera object variables
void get_sphere(obj *cur_obj, FILE *file, char *c, int *result)
{
   // Variable declarations
   char property[PROPERTY_LEN];
   char value[VALUE_LEN];
   bool r_found = FALSE;
   bool g_found = FALSE;
   bool b_found = FALSE;
   bool x_found = FALSE;
   bool y_found = FALSE;
   bool z_found = FALSE;
   bool radius_found = FALSE;
   int prop_count = 0;

   // Store object type
   cur_obj->type = SPHERE;

   // Store object variables
   while (*c != LINE_TERM && *c != EOF)
   {
      // Get next word in line
      get_next_word(property, PROP_SEP, PROPERTY_LEN, file, c);

      // Compare property value
      if (strcmp(property, "color") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the r color value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->color.r = atof(value);

            // Set boolean value
            r_found = TRUE;
         }

         // Get the g color value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->color.g = atof(value);

            // Set boolean value
            g_found = TRUE;
         }

         // Get the b color value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->color.b = atof(value);

            // Set boolean value
            b_found = TRUE;
         }

         // Make sure if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "position") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the x value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.x = atof(value);

            // Set boolean value
            x_found = TRUE;
         }

         // Get the y value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.y = atof(value);

            // Set boolean value
            y_found = TRUE;
         }

         // Get the z value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.z = atof(value);

            // Set boolean value
            z_found = TRUE;
         }

         // Make sure that if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "radius") == 0)
      {
         // Get property value
         get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

         // Set property value
         cur_obj->radius = atof(value);

         // Set boolean value
         radius_found = TRUE;

         // Increment the prop count
         prop_count++;
      }
      else
      {
         *result = INPUT_INVALID;
      }

      // Clear values
      property[0] = STR_END;
      value[0] = STR_END;
   }

   // Return error code if height or width was not found
   if (r_found == TRUE && g_found == TRUE && b_found == TRUE &&
       x_found == TRUE && y_found == TRUE && z_found == TRUE &&
       radius_found == TRUE && *result != INPUT_INVALID && prop_count <= SPHERE_VAL_COUNT)
   {
      *result = RUN_SUCCESS;
   }
   else
   {
      *result = INPUT_INVALID;
   }
}

// Helper method used to store plane object variables
void get_plane(obj *cur_obj, FILE *file, char *c, int *result)
{
   // Variable declarations
   char property[PROPERTY_LEN];
   char value[VALUE_LEN];
   bool r_found = FALSE;
   bool g_found = FALSE;
   bool b_found = FALSE;
   bool x_found = FALSE;
   bool y_found = FALSE;
   bool z_found = FALSE;
   bool n1_found = FALSE;
   bool n2_found = FALSE;
   bool n3_found = FALSE;
   int prop_count = 0;

   // Store object type
   cur_obj->type = PLANE;

   // Store object variables
   while (*c != LINE_TERM && *c != EOF)
   {
      // Get next word in line
      get_next_word(property, PROP_SEP, PROPERTY_LEN, file, c);

      // Compare property value
      if (strcmp(property, "color") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the r color value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->color.r = atof(value);

            // Set boolean value
            r_found = TRUE;
         }

         // Get the g color value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->color.g = atof(value);

            // Set boolean value
            g_found = TRUE;
         }

         // Get the b color value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->color.b = atof(value);

            // Set boolean value
            b_found = TRUE;
         }

         // Make sure if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "position") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the x value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.x = atof(value);

            // Set boolean value
            x_found = TRUE;
         }

         // Get the y value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.y = atof(value);

            // Set boolean value
            y_found = TRUE;
         }

         // Get the z value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->position.z = atof(value);

            // Set boolean value
            z_found = TRUE;
         }

         // Make sure that if ',' is next character, it moves past it
         if (*c == VALUE_SEP && *c != LINE_TERM)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else if (strcmp(property, "normal") == 0)
      {
         // Since value is property, get everything up to "["
         while (*c != LINE_TERM && isspace(*c))
         {
            *c = fgetc(file);
         }

         // Get the normal x value
         if (*c != EOF && *c == V3_START && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->normal.x = atof(value);

            // Set boolean value
            n1_found = TRUE;
         }

         // Get the normal y value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, VALUE_SEP, VALUE_LEN, file, c);

            // Set property value
            cur_obj->normal.y = atof(value);

            // Set boolean value
            n2_found = TRUE;
         }

         // Get the normal z value
         if (*c != EOF && *c != LINE_TERM)
         {
            *c = fgetc(file);
            
            // Get property value
            get_next_word(value, V3_END, VALUE_LEN, file, c);

            // Set property value
            cur_obj->normal.z = atof(value);

            // Set boolean value
            n3_found = TRUE;
         }

         // Make sure that if ',' is next character, it moves past it
         if (*c == VALUE_SEP)
         {
            *c = fgetc(file);
         }

         // Increment property count if all found
         prop_count++;
      }
      else
      {
         *result = INPUT_INVALID;
      }

      // Clear values
      property[0] = STR_END;
      value[0] = STR_END;
   }

   // Return error code if height or width was not found
   if (r_found == TRUE && g_found == TRUE && b_found == TRUE &&
       x_found == TRUE && y_found == TRUE && z_found == TRUE &&
       n1_found == TRUE && n2_found == TRUE && n3_found == TRUE &&
       *result != INPUT_INVALID && prop_count <= SPHERE_VAL_COUNT)
   {
      *result = RUN_SUCCESS;
   }
   else
   {
      *result = INPUT_INVALID;
   }
}

// Helper method used to retrieve word from file
void get_next_word(char *word, char delim, int max_len, FILE *file, char *c)
{
   // Variable declarations
   int index = 0;

   // Start by stripping white-space from front of word
   while (isspace(*c))
   {
      *c = fgetc(file);
   }

   // While deliminator is not reached, continue searching for word
   while (*c != delim && index < max_len && *c != LINE_TERM && *c != EOF)
   {
      word[index] = *c;
      *c = fgetc(file);
      index++;
   }

   // If character is end of file, return error code
   if (*c != EOF && *c != LINE_TERM)
   {
      *c = fgetc(file);
   }

   // Set ending character
   word[index] = STR_END;
}





