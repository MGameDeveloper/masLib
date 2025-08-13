#include <stdint.h>


/**************************************
* API DATA
***************************************/
struct masFolder;
struct masFile;
struct masFileStream;


/**************************************
* FOLDER API
***************************************/
masFolder*  mas_folder_current();
const char* mas_folder_path(masFolder* Folder);
masFolder*  mas_folder_create(const char* FolderPath);
void        mas_folder_destroy(masFolder** Folder);
masFile*    mas_folder_search_for_file(masFolder* Folder, const char* Name, bool EnableRecursiveSearch);

/**************************************
* FOLDER CHANGES MONITOR API
***************************************/
//void mas_folder_watcher_add(masFolder* Folder);
//void mas_folder_watcher_remove(masFolder* Folder);

/**************************************
* FILE API
***************************************/
masFile*    mas_file_next(masFile* File);
const char* mas_file_name(masFile* File);
const char* mas_file_path(masFile* File);
const char* mas_file_type(masFile* File);
uint64_t    mas_file_size(masFile* File);


/**************************************
* FILE STREAM API
***************************************/
masFileStream* mas_file_open_stream(masFile* File);
void mas_file_close_stream(masFileStream** FileStream);




/**************************************
* API TEST
***************************************/
int main()
{
    masFolder* CWDFolder = mas_folder_current();

    masFile* File = mas_folder_search_for_file(CWDFolder, ".png", true);
    for(; !File; File = mas_file_next(File))
    {
        printf("FILE_PATH: %s\n", mas_file_name(File));
        printf("FILE_NAME: %s\n", mas_file_path(File));
        printf("FILE_TYPE: %s\n", mas_file_type(File));
        printf("FILE_SIZE: %u\n", mas_file_size(File));
    }
}