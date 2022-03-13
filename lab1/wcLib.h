typedef struct{
    size_t size;
    char ** data;
}wcMainTable;
wcMainTable wc_createMainTable(size_t nEntries);
void removeBlock(wcMainTable * array, int id);
int wc_addBlock(wcMainTable*array, const char* path);


