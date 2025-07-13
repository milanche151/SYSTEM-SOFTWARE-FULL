#include "linker.h"

#include "util.h"
#include <assert.h>

enum LinkerArgsState {
  LINKER_ARGS_STATE_INPUT,
  LINKER_ARGS_STATE_OUTPUT,
};

enum LinkerOutputType {
  LINKER_OUTPUT_ERROR,
  LINKER_OUTPUT_HEX,
  LINKER_OUTPUT_OBJ,
};

VECTOR_DECLARE(VecFILE, FILE *);
VECTOR_IMPLEMENT(VecFILE, FILE *);

int main(int argc, char** argv){
  
  enum LinkerArgsState state = LINKER_ARGS_STATE_INPUT;
  enum LinkerOutputType output_type = LINKER_OUTPUT_HEX;
  VecFILE input_files = VecFILECreate();
  FILE *output_file = NULL;
  VecSectionPlace section_places = VecSectionPlaceCreate();

  for(int i = 1; i < argc; i++){
    const char *curr_arg = argv[i];
    switch(state){
    case LINKER_ARGS_STATE_INPUT:
      if(strcmp(curr_arg, "-hex") == 0){
        output_type = LINKER_OUTPUT_HEX;
      }
      else if(strcmp(curr_arg, "-relocatable") == 0){
        output_type = LINKER_OUTPUT_OBJ;
      }
      else if(strncmp(curr_arg, "-place=", sizeof("-place=") - sizeof("")) == 0){
        size_t name_len = 0;
        const char *name_p = curr_arg + sizeof("-place=") - sizeof("");
        for(; name_p[name_len] != '@' && name_p[name_len] != 0; name_len++);

        // error
        if(name_p[name_len] == 0) continue;

        SectionPlace new_place = {
          .section_name = myMalloc(name_len),
          .start = 0,
        };

        strncpy(new_place.section_name, name_p, name_len);

        const char *val_p = name_p + name_len + sizeof("@") - sizeof("");
        sscanf(val_p, "0x%x", &new_place.start);

        VecSectionPlacePush(&section_places, new_place);
      }
      else if(strcmp(curr_arg, "-o") == 0){
        state = LINKER_ARGS_STATE_OUTPUT;
      }
      else {
        VecFILEPush(&input_files, fopen(curr_arg, "r"));
      }
      break;
    case LINKER_ARGS_STATE_OUTPUT:
      output_file = fopen(curr_arg, "w");
      state = LINKER_ARGS_STATE_INPUT;
      break;
    default: assert(0);
    }
  }

  for(size_t i = 0; i < input_files.size; i++){
    if(input_files.data[i] == NULL) {
      printf("Input file couldn't be open.\n");
      goto error;
    }
  }

  for(size_t i = 0; i < section_places.size; i++){
    for(size_t j = i + 1; j < section_places.size; j++){
      if(strcmp(section_places.data[i].section_name, section_places.data[j].section_name) == 0){
        printf("Section place for %s is specified multiple times.\n", section_places.data[i].section_name);
        goto error;
      }
    }
  }

  if(output_type == LINKER_OUTPUT_HEX){
    Linker linker = LinkerCreate(
      input_files.data, input_files.size,
      section_places.data, section_places.size
    );

    if(output_file == NULL){
      printf("Output file invalid.\n");
      goto error;
    }

    if(linker.correct){
      LinkerPrintHexFile(&linker, output_file);
    }

    LinkerDestroy(&linker);
  }
  else if(output_type == LINKER_OUTPUT_OBJ){

    if(output_file == NULL){
      printf("Output file invalid.\n");
      goto error;
    }

    Linker linker = LinkerCreateRelocatable(
      input_files.data, input_files.size, output_file
    );

    LinkerDestroy(&linker);
  }
  else {
    printf("Output type not specified.\n");
    goto error;
  }

  VecFILEDestroy(&input_files);
  VecSectionPlaceDestroy(&section_places);
  return 0;

error:
  VecFILEDestroy(&input_files);
  VecSectionPlaceDestroy(&section_places);
  return -1;
}