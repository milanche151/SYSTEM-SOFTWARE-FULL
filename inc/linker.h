#ifndef _LINKER_H_
#define _LINKER_H_

#include "assembler.h"



typedef struct ObjSection {
  size_t symtab_index;
  
  // machine code
  size_t n_bytes;
  char *bytes;

  size_t n_relocs;
  Relocation *relocs;
} ObjSection;

typedef struct ObjFile {
  bool correct;

  size_t n_syms;
  SymTableRow *symbols;

  size_t n_sections;
  ObjSection *sections;

} ObjFile;

typedef struct GlobalSym{
  size_t file_index;
  size_t symtbl_index;
}GlobalSym;

typedef struct SectionPlace{
  CORE_ADDR start;
  CORE_ADDR end;
  const char* section_name;
}SectionPlace;

VECTOR_DECLARE(VecGlobalSym, GlobalSym);
VECTOR_DECLARE(VecSectionPlace, SectionPlace);

typedef struct Linker{
  bool correct;
  
  size_t n_files;
  ObjFile *files;

  VecGlobalSym globalSyms;
  VecSectionPlace all_section_places;
} Linker;

Linker LinkerCreate(FILE *input_files[], size_t n_input_files, SectionPlace* places, size_t n_places);
void LinkerDestroy(Linker *linker);
void LinkerPrint(const Linker *linker);
void LinkerPrintHexFile(const Linker *linker, FILE *output_file);

#endif