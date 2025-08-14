#include <stdint.h>


/**************************************
* 
***************************************/
struct masDirectorySearch;
struct masFile;


/**************************************
* 
***************************************/
masDirectorySearch* mas_directory_search_create(const char* Path, bool EnableRecursiveSearch);
void                mas_directory_search_destroy(masDirectorySearch** DirectorySearch);
masFile*            mas_directory_search_find_file(masDirectorySearch* DirectorySearch, const char** Targets, int32_t TargetCount);
masFile*            mas_directory_search_next_file(masDirectorySearch* DirectorySearch);


/**************************************
* 
***************************************/
masFile*    mas_file_next(masFile* File);
const char* mas_file_name(masFile* File);
const char* mas_file_path(masFile* File);
const char* mas_file_extension(masFile* File);
uint64_t    mas_file_size(masFile* File);


/**************************************
* API TEST
***************************************/
int main()
{
    const char* AssetPath = "";

    masDirectorySearch* AssetSearchDirectory = mas_directory_search_create(AssetPath, true);

    masFile* File = mas_directory_search_find(AssetSearchDirectory, ".png");
    for(; !File; File = mas_directory_search_next_file(AssetSearchDirectory))
    {
        printf("FILE_PATH:      %s\n", mas_file_name(File));
        printf("FILE_NAME:      %s\n", mas_file_path(File));
        printf("FILE_EXTENSION: %s\n", mas_file_extension(File));
        printf("FILE_SIZE:      %u\n", mas_file_size(File));
    }

    mas_directory_search_destroy(&AssetSearch);
}