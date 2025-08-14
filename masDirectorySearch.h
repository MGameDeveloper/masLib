#include <stdint.h>


/**************************************
* 
***************************************/
struct masDirectorySearch ;
struct masFile;


/**************************************
* 
***************************************/
masDirectorySearch* mas_directory_search_create(const char* Path);
void                mas_directory_search_destroy(masDirectorySearch** DirectorySearch);
masFile*            mas_directory_search_find(masDirectorySearch* DirectorySearch, const char** Targets, int32_t TargetCount);
masFile*            mas_directory_search_find_next(masDirectorySearch* DirectorySearch);


/**************************************
* 
***************************************/
const char* mas_file_name(masFile* File);
const char* mas_file_path(masFile* File);
const char* mas_file_extension(masFile* File);
