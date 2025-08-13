

struct masString;

masString* mas_string_create(const char* Text);
void       mas_string_destroy(masString** String);
masString* mas_string_create_substring(masString* String, int Offset, int Size);
void       mas_string_insert_first(masString** StringRef, const char* Text);
void       mas_string_insert_last(masString** StringRef, const char* Text);
void       mas_string_insert_at(masString** StringRef, int Offset, const char* Text);
int        mas_string_size(masString* String);

int        mas_string_find_first(masString* String, char Char);
int        mas_string_find_last(masString* String, char Char);
bool       mas_string_compare(masString* String1, masString* String2);






