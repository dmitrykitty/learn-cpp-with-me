#include <cstddef>

constexpr std::size_t ALLOC_SIZE = 1024;

static char allocbuf[ALLOC_SIZE];
static char* allocp = allocbuf;
static char* allocbufend = allocbuf + ALLOC_SIZE; 

char* alloc(std::size_t n) {
    if(n == 0 || allocbufend - allocp < n) {
        return nullptr; 
    }
    char* to_return = allocp; 
    allocp += n; 
    return to_return;
}
void afree(char* p) {
    if(p == nullptr) {
        return;
    }

    allocp = p; 
}