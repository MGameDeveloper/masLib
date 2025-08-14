#include <stdint.h>


/**************************************
* 
***************************************/
struct masDirectorySearch ;
struct masFoundFiles;
struct masFile
{
    const char* Path;
    const char* Name;
    const char* Extension;
};


/**************************************
* 
***************************************/
masDirectorySearch* mas_directory_search_create(const char* Path);
void                mas_directory_search_destroy(masDirectorySearch** DirectorySearch);
masFoundFiles*      mas_directory_search_run(masDirectorySearch* DirectorySearch, const char** Targets, int32_t TargetCount);
masFile*            mas_found_files_next(masFoundFiles* FoundFiles);

