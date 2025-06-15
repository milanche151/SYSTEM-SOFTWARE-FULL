#include "linker.h"

#include "util.h"

int main(int argc, char** argv){

  FILE* input_files[2] = {
    [0] = fopen("tests/file1.o", "r"),
    [1] = fopen("tests/file2.o", "r"),
  };

  for(size_t i = 0; i < arrlen(input_files); i++){
    if(input_files[i] == NULL){
      printf("Input file invalid.\n");
      return -1;
    }
  }

  SectionPlace section_places[2] = {
    [0] = { .start = 0x4000, .section_name = "text", },
    [1] = { .start = 0x1000, .section_name = "data", },
  };  

  Linker linker = LinkerCreate(
    input_files, arrlen(input_files),
    section_places, arrlen(section_places)
  );

  FILE *output_file = fopen("tests/a.hex", "w");
  if(output_file == NULL){
    printf("Output file invalid.\n");
    return -1;
  }

  if(linker.correct){
    LinkerPrintHexFile(&linker, output_file);
  }

  LinkerDestroy(&linker);

  return 0;
}