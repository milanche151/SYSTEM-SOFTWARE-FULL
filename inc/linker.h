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

typedef struct Linker{
  bool correct;
  
  size_t n_files;
  ObjFile *files;
} Linker;

Linker LinkerCreate(FILE *input_files[], size_t n_input_files);
void LinkerDestroy(Linker *linker);
void LinkerPrint(const Linker *linker);
void LinkerPrintHexFile(const Linker *linker, FILE *output_file);

#endif