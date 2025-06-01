#include "linker.h"

#include "util.h"

VECTOR_IMPLEMENT(VecGlobalSym,GlobalSym);

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

    objFile.correct = fscanf(input_file, "%lu", &curr_section->n_relocs);
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


bool LinkerAddGlobalSymbol(Linker *linker, size_t file_idx, size_t sym_idx){
  const SymTableRow *curr_sym = &linker->files[file_idx].symbols[sym_idx];

  for(size_t i = 0; i < linker->globalSyms.size; i++){
    GlobalSym global_id = linker->globalSyms.data[i];
    SymTableRow *curr_global_sym = &linker->files[global_id.file_index].symbols[global_id.symtbl_index];

    if(strcmp(curr_global_sym->name, curr_sym->name) == 0){
      printf("Symbol %s defined multiple times.\n", curr_sym->name);
      return false;
    }
  }

  VecGlobalSymPush(
    &linker->globalSyms,
    (GlobalSym){
      .file_index = file_idx,
      .symtbl_index = sym_idx,
    }
  );

  return true;
}

bool LinkerFindExternSymbol(Linker *linker, const char *symbol_name){
  for(size_t i = 0; i < linker->globalSyms.size; i++){
    GlobalSym global_id = linker->globalSyms.data[i];
    SymTableRow *curr_global_sym = &linker->files[global_id.file_index].symbols[global_id.symtbl_index];

    if(strcmp(curr_global_sym->name, symbol_name) == 0){
      return true;
    }
  }

  return false;
}

Linker LinkerCreate(FILE *input_files[], size_t n_input_files)
{
  Linker linker = {
    .correct = true,
    .n_files = n_input_files,
    .files = myMalloc(sizeof(*linker.files) * n_input_files),
    .globalSyms = VecGlobalSymCreate(),
  };

  for(size_t i = 0; i < linker.n_files; i++){
    linker.files[i] = parseObjFile(input_files[i]);
    if(!linker.files[i].correct){
      printf("Linker input file %lu is NOT correct.\n", i);
      linker.correct = false;
      break;
    }
  }

  if(!linker.correct) return linker;

  // add all global symbols
  for(size_t i = 0; i < linker.n_files; i++){
    for(size_t j = 0; j < linker.files[i].n_syms; j++){
      SymTableRow *curr_sym = &linker.files[i].symbols[j];
      if(curr_sym->bind == BIND_TYPE_GLOBAL && curr_sym->section != EXTERN_SECTION){
        if(!LinkerAddGlobalSymbol(&linker, i, j)){
          printf("Global symbol %s defined multiple times.\n", curr_sym->name);
          linker.correct = false;
        }
      }
    }
    
  }

  // check if all extern symbols are defined
  for(size_t i = 0; i < linker.n_files; i++){
    for(size_t j = 0; j < linker.files[i].n_syms; j++){
      SymTableRow *curr_sym = &linker.files[i].symbols[j];
      if(curr_sym->bind == BIND_TYPE_GLOBAL && curr_sym->section == EXTERN_SECTION){
        if(!LinkerFindExternSymbol(&linker, curr_sym->name)){
          printf("Extern symbol %s is not defined.\n", curr_sym->name);
          linker.correct = false;
        }
      }
    }
  }
  
  if(!linker.correct) return linker;
}
