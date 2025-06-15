#include "linker.h"

#include <assert.h>

#include "util.h"

VECTOR_IMPLEMENT(VecGlobalSym,GlobalSym);
VECTOR_IMPLEMENT(VecSectionPlace, SectionPlace);

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

SymTableRow* LinkerFindExternSymbol(Linker *linker, const char *symbol_name){
  for(size_t i = 0; i < linker->globalSyms.size; i++){
    GlobalSym global_id = linker->globalSyms.data[i];
    SymTableRow *curr_global_sym = &linker->files[global_id.file_index].symbols[global_id.symtbl_index];

    if(strcmp(curr_global_sym->name, symbol_name) == 0){
      return curr_global_sym;
    }
  }

  return NULL;
}

VecSectionPlace
LinkerCreateAllSectionPlaces(Linker *linker, SectionPlace *places, size_t n_places){

  VecSectionPlace all_section_places = VecSectionPlaceCreate();

  // add all explicit sections to the vector
  for(size_t i = 0; i < n_places; i++){
    const SectionPlace *curr_place_spec = &places[i];

    VecSectionPlacePush(&all_section_places, (SectionPlace){
      .section_name = curr_place_spec->section_name,
      .start = curr_place_spec->start,
      .end = curr_place_spec->start,
    });
  }

  // add all implicit sections to the vector
  for(size_t i = 0; i < linker->n_files; i++){
    const ObjFile *curr_file = &linker->files[i];
    for(size_t j = 0; j < curr_file->n_sections; j++){
      const ObjSection *curr_section = &curr_file->sections[j];
      const SymTableRow *curr_section_as_symbol = &curr_file->symbols[curr_section->symtab_index];

      bool found = false;
      for(size_t k = 0; k < all_section_places.size; k++){
        if(strcmp(all_section_places.data[k].section_name, curr_section_as_symbol->name) == 0){
          found = true;
          break;
        }
      }

      if(!found) VecSectionPlacePush(
        &all_section_places,
        (SectionPlace){
          .section_name = curr_section_as_symbol->name,
          .start = 0,
          .end = 0,
        }
      );
    }
  }

  // address for implicit sections to start
  CORE_ADDR current_start = 0;

  // set end for explicit sections
  for(size_t sec_idx = 0; sec_idx < n_places; sec_idx++){

    SectionPlace *curr_section_place = &all_section_places.data[sec_idx];

    for(size_t i = 0; i < linker->n_files; i++){
      const ObjFile *curr_file = &linker->files[i];
      for(size_t j = 0; j < curr_file->n_sections; j++){
        const ObjSection *curr_section = &curr_file->sections[j];
        SymTableRow *curr_section_as_symbol = &curr_file->symbols[curr_section->symtab_index];

        // check if curr section is named as current section place
        if(strcmp(curr_section_place->section_name, curr_section_as_symbol->name) != 0) continue;

        curr_section_as_symbol->value = curr_section_place->end;

        curr_section_place->end += curr_section->n_bytes;

        // update current start based on this section's end address
        if(curr_section_place->end > current_start) current_start = curr_section_place->end;
      }
    }

  }

  // set start and end for implicit sections
  for(size_t sec_idx = n_places; sec_idx < all_section_places.size; sec_idx++){

    SectionPlace *curr_section_place = &all_section_places.data[sec_idx];

    // set start address
    curr_section_place->start = current_start;
    curr_section_place->end = current_start;

    for(size_t i = 0; i < linker->n_files; i++){
      const ObjFile *curr_file = &linker->files[i];
      for(size_t j = 0; j < curr_file->n_sections; j++){
        const ObjSection *curr_section = &curr_file->sections[j];
        SymTableRow *curr_section_as_symbol = &curr_file->symbols[curr_section->symtab_index];

        // check if curr section is named as current section place
        if(strcmp(curr_section_place->section_name, curr_section_as_symbol->name) != 0) continue;

        curr_section_as_symbol->value = curr_section_place->end;
      
        curr_section_place->end += curr_section->n_bytes;
      }
    }

    // update current start based on this section's end address
    if(curr_section_place->end > current_start) current_start = curr_section_place->end;
  }

  return all_section_places;

}

void LinkerCalculateAbsoluteValues(Linker *linker){
  for(size_t i = 0; i < linker->n_files; i++){
    ObjFile *curr_file = &linker->files[i];
    for(size_t j = 0; j < curr_file->n_syms; j++){
      SymTableRow *curr_symbol = &curr_file->symbols[j];

      if(curr_symbol->section != EXTERN_SECTION && curr_symbol->type != SYM_TBL_TYPE_SECTION){
        SymTableRow *section_as_symbol = &curr_file->symbols[curr_symbol->section];
        curr_symbol->value += section_as_symbol->value;
      }
    }
  }
}

void LinkerHandleRelocations(Linker *linker){
  for(size_t i = 0; i < linker->n_files; i++){
    ObjFile *curr_file = &linker->files[i];
    
    for(size_t j = 0; j < curr_file->n_sections; j++){
      ObjSection *curr_section = &curr_file->sections[j];

      for(size_t k = 0; k < curr_section->n_relocs; k++){
        Relocation *curr_reloc = &curr_section->relocs[k];

        if(curr_reloc->offset + 4 > curr_section->n_bytes){
          printf("Linker relocation entry offset %lu is exceeding section size.\n", curr_reloc->offset);
          linker->correct = false;
        }

        if(curr_reloc->symbolIndex == 0 || curr_reloc->symbolIndex >= curr_file->n_syms){
          printf("Linker relocation entry symbol %lu is exceeding symbol table size.\n", curr_reloc->symbolIndex);
          linker->correct = false;
        }

        switch(curr_reloc->type){
        case RELOCATION_TYPE_DATA32:

          // patch the value, little-endian
          const SymTableRow *symbol = &curr_file->symbols[curr_reloc->symbolIndex];
          if(symbol->section == EXTERN_SECTION) symbol = symbol->definition;

          uint32_t value = symbol->value + curr_reloc->addend;

          unsigned char *patch_bytes = curr_section->bytes + curr_reloc->offset;
          patch_bytes[0] = value >>  0;
          patch_bytes[1] = value >>  8;
          patch_bytes[2] = value >> 16;
          patch_bytes[3] = value >> 24;

          break;
        default: assert(0);
        }
      }
    }
  }
}

Linker LinkerCreate(FILE *input_files[], size_t n_input_files, SectionPlace* places, size_t n_places)
{
  Linker linker = {
    .correct = true,
    .n_files = n_input_files,
    .files = myMalloc(sizeof(*linker.files) * n_input_files),
    .globalSyms = VecGlobalSymCreate(),
    .all_section_places = VecSectionPlaceCreate(),
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

      // don't check for *UNDEF* symbol (its index is EXTERN SECTIONS)
      if(j != EXTERN_SECTION && curr_sym->section == EXTERN_SECTION){
        SymTableRow* definition =LinkerFindExternSymbol(&linker, curr_sym->name);
        if(definition == NULL){
          printf("Extern symbol %s is not defined.\n", curr_sym->name);
          linker.correct = false;
        }
        else{
          curr_sym->definition = definition;
        }
      }
    }
  }
  
  if(!linker.correct) return linker;

  // check if all specified section places have unique names
  for(size_t i = 0; i < n_places; i++){
    assert(places[i].section_name != NULL);
    for(size_t j = i + 1; j < n_places; j++){
      assert(places[j].section_name != NULL);

      if(strcmp(places[i].section_name, places[j].section_name) == 0){
        printf("Linker section place %s specified multiple times.\n", places[i].section_name);
        linker.correct = false;
      }
    }
  }
  if(!linker.correct) return linker;

  // destroy old section places vector
  VecSectionPlaceDestroy(&linker.all_section_places);

  // place all sections to absolute addresses
  linker.all_section_places = LinkerCreateAllSectionPlaces(&linker, places, n_places);

  //check for overlaps between sections

  for(size_t i = 0 ; i < linker.all_section_places.size; i++){
    SectionPlace *curr_place = &linker.all_section_places.data[i];
    for (size_t j = i+1; j < linker.all_section_places.size; j++){
      SectionPlace *other_place  = &linker.all_section_places.data[j];
      if(
        ((curr_place->start < other_place->start) && (other_place->start < curr_place->end))
        ||
        ((other_place->start < curr_place->start) && (curr_place->start < other_place->end))
      ){
        printf("Sections %s and %s overlap.\n", curr_place->section_name, other_place->section_name);
        linker.correct = false;
      }

      
    }
    
  }
  if(!linker.correct) return linker;

  LinkerCalculateAbsoluteValues(&linker);
  LinkerHandleRelocations(&linker);

  if(!linker.correct) return linker;

  return linker;
}

void LinkerDestroy(Linker *linker){
  VecSectionPlaceDestroy(&linker->all_section_places);
  VecGlobalSymDestroy(&linker->globalSyms);

  for(size_t i = 0; i < linker->n_files; i++){
    ObjFile *file = &linker->files[i];
    myFree(file->symbols);
    for(size_t j = 0; j < file->n_sections; j++){
      ObjSection *section = &file->sections[j];
      myFree(section->bytes);
      myFree(section->relocs);
    }
    myFree(file->sections);
  }
  myFree(linker->files);
}

typedef struct SectionSymTab{
  ObjSection* section;
  SymTableRow* symTab;
}SectionSymTab;

static int section_compare(const void *b1, const void *b2){
  const SectionSymTab *sect1 = b1;
  const SectionSymTab *sect2 = b2;

  return sect1->symTab[sect1->section->symtab_index].value
    > sect2->symTab[sect2->section->symtab_index].value
      ? +1
      : -1;

}

#define HEX_FILE_PADDING 8

static inline void PrintHexByte(FILE *output_file, size_t addr, unsigned char byte){
  if(addr % HEX_FILE_PADDING == 0) fprintf(output_file, "\n%04x: ", (unsigned)addr);
  fprintf(output_file, "%02x ", byte);
}

void LinkerPrintHexFile(const Linker *linker, FILE *output_file){
  
  size_t num_section = 0;
  for (size_t i = 0; i < linker->n_files; i++){
    const ObjFile* curr_file = &linker->files[i];
    num_section += curr_file->n_sections;
  }

  SectionSymTab* all_sections = myMalloc(num_section * sizeof(*all_sections));
  num_section = 0;
  for (size_t i = 0; i < linker->n_files; i++){
    const ObjFile* curr_file = &linker->files[i];
    for (size_t j = 0; j < curr_file->n_sections; j++){
      all_sections[num_section++] = (SectionSymTab){
        .section = &curr_file->sections[j],
        .symTab = curr_file->symbols
      };
    }  
  }
  
  qsort(all_sections, num_section, sizeof(*all_sections), section_compare);
  for(size_t i = 0; i < num_section; i++){
    SectionSymTab *curr_section_symtab = &all_sections[i];
    ObjSection *curr_section = curr_section_symtab->section;
    SymTableRow *curr_section_as_symbol = &curr_section_symtab->symTab[curr_section->symtab_index];

    printf("Section %s, start = %04x, end = %04x\n",
      curr_section_as_symbol->name,
      (CORE_ADDR)(curr_section_as_symbol->value),
      (CORE_ADDR)(curr_section_as_symbol->value + curr_section->n_bytes)
    );
  }

  // last address we have written to
  size_t lastAddr = 0;
  for(size_t i = 0; i < num_section; i++){
    SectionSymTab *curr_section_symtab = &all_sections[i];
    ObjSection *curr_section = curr_section_symtab->section;
    SymTableRow *curr_section_as_symbol = &curr_section_symtab->symTab[curr_section->symtab_index];
    
    // first address of the next section
    size_t currAddr = curr_section_as_symbol->value;
    size_t lastAddrLineLimit = (lastAddr + HEX_FILE_PADDING - 1) / HEX_FILE_PADDING * HEX_FILE_PADDING;
    size_t currAddrLineStart = currAddr / HEX_FILE_PADDING * HEX_FILE_PADDING;

    // fill end of lastAddr's line with 0s
    for(size_t j = lastAddr; j < (currAddr < lastAddrLineLimit ? currAddr : lastAddrLineLimit); j++){
      PrintHexByte(output_file, j, 0x00);
    }
    // fill beginning of currAddr's line with 0s
    for(size_t j = (currAddrLineStart > lastAddr ? currAddrLineStart : lastAddr); j < currAddr; j++){
      PrintHexByte(output_file, j, 0x00);
    }

    for(size_t j = 0; j < curr_section->n_bytes; j++){
      PrintHexByte(output_file, curr_section_as_symbol->value + j, curr_section->bytes[j]);
    }

    lastAddr = curr_section_as_symbol->value + curr_section->n_bytes;
  }

}
