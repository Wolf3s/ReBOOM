// killough 10/98: new functions, to allow processing DEH files in-memory
// (e.g. from wads)

typedef struct {
    byte* inp, * lump; // Pointer to string or FILE
    int_64_t size;
    FILE* f;
} DEHFILE;

// Do this for a lookup--the pointer (loaded above) is cross-referenced
// to a string key that is the same as the define above.  We will use
// strdups to set these new values that we read from the file, orphaning
// the original value set above.

typedef struct {
    char** ppstr;  // doubly indirect pointer to string
    char* lookup;  // pointer to lookup string name
} deh_strs;

// Structure deh_block is used to hold the block names that can
// be encountered, and the routines to use to decipher them

typedef struct
{
    char* key;       // a mnemonic block code name
    void (* const fptr)(DEHFILE*, FILE*, char*); // handler
} deh_block;

typedef struct {
    actionf_t cptr;  // actual pointer to the subroutine
    char* lookup;  // mnemonic lookup string to be specified in BEX
} deh_bexptr;
