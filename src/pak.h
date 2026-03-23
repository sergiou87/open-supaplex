// PAK System Public API Header

#ifndef PAK_H
#define PAK_H

// Magic number for PAK files
#define PAK_MAGIC 0x50414B00 // "PAK\0"

// Resource structure definition
typedef struct {
    char name[256]; // Resource name
    unsigned int size; // Size of the resource
    unsigned int offset; // Offset in the PAK file
} PAKResource;

// Function declarations
void pak_init(const char* filename);
void pak_close(void);
PAKResource* pak_get_resource(const char* name);
void pak_free_resource(PAKResource* resource);

#endif // PAK_H
