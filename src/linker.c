#include "linker.h"

#include "util.h"

static ObjFile parseObjFile(FILE *input_file){
  ObjFile objFile = { .correct = true };

  objFile.correct = fscanf(input_file, "%lu", &objFile.n_syms) == 1;
  if(!objFile.correct) return objFile;

  objFile.symbols = myMalloc(sizeof(*objFile.symbols) * objFile.n_syms);
  for(size_t i = 0; i < objFile.n_syms; i++){
    SymTableRow *curr_symbol = &objFile.symbols[i];

    unsigned is_defined = false;

    objFile.correct = fscanf(input_file, "%ms%lu%u%u%u%u",
      &curr_symbol->name,
      &curr_symbol->section,
      &curr_symbol->value,
      &curr_symbol->type,
      &is_defined,
      &curr_symbol->bind
    ) == 6;
    if(!objFile.correct) return objFile;

    curr_symbol->defined = is_defined;

  }

  objFile.correct = fscanf(input_file, "%lu",&objFile.n_sections)==1;
  if(!objFile.correct) return objFile;

  objFile.sections = myMalloc(sizeof(*objFile.sections)*objFile.n_sections);
  for(size_t i = 0; i < objFile.n_sections; i++){
    ObjSection *curr_section = &objFile.sections[i];

    objFile.correct = fscanf(input_file,"%lu",&curr_section->symtab_index)==1;
    if(!objFile.correct) return objFile;
    
    objFile.correct = fscanf(input_file,"%lu",&curr_section->n_bytes)==1;
    if(!objFile.correct) return objFile;

    curr_section->bytes = myMalloc(sizeof(*curr_section->bytes) * curr_section->n_bytes);
    for(size_t j = 0; j < curr_section->n_bytes; j++){
      unsigned byte;
      
      objFile.correct = fscanf(input_file,"%x",&byte) == 1;
      if(!objFile.correct) return objFile;

      curr_section->bytes[j]=byte;
    }

    objFile.correct = fscanf(input_file, "%lu", curr_section->n_relocs);
    if(!objFile.correct) return objFile;

    curr_section->relocs = myMalloc(sizeof(*curr_section->relocs) * curr_section->n_relocs);
    for(size_t j = 0; j < curr_section->n_relocs; j++){
      Relocation *curr_reloc = &curr_section->relocs[j];

      objFile.correct = fscanf(input_file, "\n%lu%u%lu%u",
        &curr_reloc->offset,
        &curr_reloc->type,
        &curr_reloc->symbolIndex,
        &curr_reloc->addend
      ) == 4;
      if(!objFile.correct) return objFile;
    }
  }


  return objFile;
}

Linker LinkerCreate(FILE *input_files[], size_t n_input_files)
{
  Linker linker = {
    .correct = true,
    .n_files = n_input_files,
    .files = myMalloc(sizeof(*linker.files) * n_input_files),
  };

  for(size_t i = 0; i < input_files; i++){
    linker.files[i] = parseObjFile(input_files[i]);
    if(!linker.files[i].correct){
      printf("Linker input file %llu is NOT correct.\n" , i);
      linker.correct = false;
      break;
    }
  }

  if(!linker.correct) return linker;
}
