#ifndef ORB_H
#define ORB_H

//---------- custom types ----------

typedef unsigned short int uint16_t;
typedef unsigned char uint8_t;

//---------- constants ----------

#define DEF_PARTITION_SIZE 128
#define DEF_VAR_NAME_SIZE 8
#define DEF_LABEL_NAME_SIZE 8
#define DEF_VAR_BUF_SIZE 266

#define DEBUG

//---------- global variables ----------

// Define a patch area which allows us to patch the code later
static char patch[] = {
    '[', 'P', 'A', 'T', 'C', 'H', DEF_PARTITION_SIZE % 256, DEF_PARTITION_SIZE / 256, ']'};

// Define a couple variable buffers
static char vBuf1[DEF_VAR_BUF_SIZE];
static char vBuf2[DEF_VAR_BUF_SIZE];

// Define a z-string buffer
static char zBuf[256];

// Define a c-string buffer
static char cBuf[256];

static uint16_t pSize;  // partition size

//----------- partition pointer variables ----------

static char *pStart;    // pointer to the start of the partition
static char *pEnd;      // pointer to the end of the partition

//---------- z-string functions ----------

// function to get a c-string from a z-string buffer
static char *get_zstring(char *zBuf, char* cBuf) {
    // get the size of the z-string buffer
    uint8_t size = *zBuf;
    // copy the c-string from the z-string buffer into the c-string buffer
    memcpy(cBuf, zBuf + 1, size);
    // set the end of the c-string buffer
    cBuf[size] = 0;
    // return a pointer to the c-string buffer
    return cBuf;
}

// function to set a c-string into a z-string buffer
static uint8_t set_zstring(char *zBuf, char size, char *s) {
    // check if the c-string is too long
    if (size > 255) {
        printf("Error: string '%s' is too long\n", s);
        return 2;
    }
    // set the size of the z-string buffer
    *zBuf = size;
    // copy the c-string into the z-string buffer
    memcpy(zBuf + 1, s, size);
    // return no error
    return 0;
}

//---------- variable functions ----------

// function to get the size of an element in the partition
static uint16_t get_element_size(char *p) {
    uint16_t size = *((uint16_t *)(p + 1));
    return size;
}

// function to check if a string is a valid variable name
static bool is_valid_var_name(char *name) {
    // check if the first character is a letter
    if (!isalpha(*name)) {
        return false;
    }
    // check if the remaining characters are letters or digits
    while (*++name) {
        if (!isalnum(*name)) {
            return false;
        }
    }
    // return true if the string is a valid variable name
    return true;
}

// function to get the size of a variable loaded into a variable buffer
static uint16_t get_var_size(char *vBuf) {
    uint8_t nameSize = *(vBuf + 1);
    uint8_t valueSize = *(vBuf + 2 + nameSize);
    return nameSize + valueSize + 3;
}

// function to get the type of a variable loaded into a variable buffer
static uint8_t get_var_type(char *vBuf) {
    return *vBuf;
}

// function to get the name of a variable loaded into a variable buffer
static char *get_var_name(char *vBuf) {
    return get_zstring(vBuf + 1, cBuf);
}

// function to get the value of a variable loaded into a variable buffer as a boolean
static bool get_var_bool(char *vBuf) {
    return *(vBuf + *(vBuf + 1) + 3);
}

// function to get the value of a variable loaded into a variable buffer as a char
static char get_var_char(char *vBuf) {
    return *(vBuf + *(vBuf + 1) + 3);
}

// function to get the value of a variable loaded into a variable buffer as an integer
static int get_var_int(char *vBuf) {
    return *((int *)(vBuf + *(vBuf + 1) + 3));
}

// function to get the value of a variable loaded into a variable buffer as a float
static float get_var_float(char *vBuf) {
    return *((float *)(vBuf + *(vBuf + 1) + 3));
}

// function to get the value of a variable loaded into a variable buffer as a z-string
static char *get_var_zstr(char *vBuf) {
    return vBuf + 2 + *(vBuf + 1);
}

// function to load a variable of the specified type, name, size and value onto a variable buffer
static uint8_t load_var(char *vBuf, char type, char *name, char size, char *value) {
    // declare a variable to store an error code
    uint8_t err = 0;
    // get the size of the variable name
    uint8_t nameSize = strlen(name);
    // check if the variable name is valid
    if (!is_valid_var_name(name)) {
        printf("Error: invalid variable name '%s'\n", name);
        return 3;
    }
    // set the type of the variable buffer
    *vBuf = type;
    // set the variable name in the variable buffer
    err = set_zstring(vBuf + 1, nameSize, name);
    if (err) {
        return err;
    }
    // set the variable value in the variable buffer
    err = set_zstring(vBuf + 2 + nameSize, size, value);
    return err;
}

// function to load a null variable of the specified name onto a variable buffer
static uint8_t load_null_var(char *vBuf, char *name) {
    return load_var(vBuf, 0x00, name, 0, "");
}

// function to load a boolean variable of the specified name and value onto a variable buffer
static uint8_t load_bool_var(char *vBuf, char *name, bool value) {
    return load_var(vBuf, 0x01, name, sizeof(bool), (char*)&value);
}

// function to load a char variable of the specified name and value onto a variable buffer
static uint8_t load_char_var(char *vBuf, char *name, char value) {
    return load_var(vBuf, 0x02, name, sizeof(char), &value);
}

// function to load an integer variable of the specified name and value onto a variable buffer
static uint8_t load_int_var(char *vBuf, char *name, int value) {
    return load_var(vBuf, 0x03, name, sizeof(int), (char*)&value);
}

// function to load a float variable of the specified name and value onto a variable buffer
static uint8_t load_float_var(char *vBuf, char *name, float value) {
    return load_var(vBuf, 0x04, name, sizeof(float), (char*)&value);
}

// function to load a string variable of the specified name and value onto a variable buffer
static uint8_t load_string_var(char *vBuf, char *name, char *value) {
    uint16_t size = strlen(value);
    return load_var(vBuf, 0x05, name, size, value);
}

//---------- partition functions ----------

// function to allocate a partition
static void alloc_partition(void) {
    // get the partition size from the patch area
    pSize = *((uint16_t *)&patch[6]);
    // allocate the partition and check if it was successful
    if ((pStart = malloc(pSize)) == NULL) {
        printf("Error: could not allocate partition of size %d\n", pSize);
        exit(1);
    }
}

// function to initialize an empty area
static char *init_empty_area(char *p, uint16_t size) {
    *p = 0x00;
    *((uint16_t *)(p + 1)) = size;
    return p;
}

// function to initialize the partition as a single empty area
static void init_partition(void) {
    pSize = *((uint16_t *)&patch[6]);
    pEnd = init_empty_area(pStart, pSize);
}

// function to add an element to the partition
static char *add_element(char *p, uint8_t type, char *eBuf, uint16_t size) {
    // set the element type
    *p = type;
    // set the element size
    *((uint16_t *)(p + 1)) = size + 3;
    // add the element to the partition position
    memcpy(p + 3, eBuf, size);
    // return the next partition position
    return p + size + 3;
}

// function to compact the partition
static bool compact_partition(char *p) {
    // if the partition is empty, return
    if (p == pEnd) {
        return true;
    }

    // get the type of the element
    uint8_t type = *p;
    // get the size of the element
    uint16_t size = get_element_size(p);

    // if the element is deleted, move the remainder of the partition over the deleted element
    if (type == 0xff) {
        // move the remainder of the partition over the deleted element
        memmove(p, p + size, pEnd - p - size);
        // set the end of the partition
        pEnd -= size;
        // compact the partition
        return compact_partition(p);
    }

    // move to the next element
    return compact_partition(p + size);
}

// function to add a variable element to the partition
static uint8_t add_var(char *v) {
    // get the size of the variable in the buffer
    uint16_t vSize = get_var_size(v);
    // add 3 to the size of the variable in the buffer to account for the element type and size fields
    uint16_t eSize = vSize + 3;

    char *pPos;

    // if the free area at the end of the partition is large enough to hold the variable in the buffer, add the variable to the end of the partition
    if (eSize <= get_element_size(pEnd)) {
        // add the variable to the end of the partition
        pPos = add_element(pEnd, 0x01, v, vSize);
        // initialize the empty area at the end of the partition
        pEnd = init_empty_area(pPos, pSize - (pPos - pStart));
        // return no error
        return 0;
    }

    // scan each element in the partition to find a deleted element which size is of the same size as the variable in the buffer
    char *p = pStart;
    while (p < pEnd) {
        // get the type of the element
        uint8_t type = *p;
        // get the size of the element
        uint16_t size = get_element_size(p);

        // if the element is deleted and the size is the same as the variable in the buffer, add the variable to the deleted element
        if (type == 0xff && size == eSize) {
            // add the variable to the deleted element
            pPos = add_element(p, 0x01, v, vSize);
            // return no error            
            return 0;
        }

        // move to the next element
        p += size;
    }

    // compact the partition
    if (!compact_partition(pStart)) {
        printf("Error: compacting partition\n");
        return 4;
    }

    // if the free area at the end of the partition is still not large enough to hold the variable in the buffer, return false
    if (eSize > get_element_size(pEnd)) {
        printf("Error: partition full\n");
        return 5;
    }

    // add the variable to the end of the partition
    return add_var(v);
}

// function to delete a variable element from the partition
static bool delete_var(char *name) {
    // scan each element in the partition to find the variable
    char *p = pStart;
    while (p < pEnd) {
        // get the type of the element
        uint8_t type = *p;
        // get the size of the element
        uint16_t size = get_element_size(p);

        // if the element is a variable and the name is the same as the variable name, delete the variable
        if (type == 0x01 && strcmp(get_var_name(p + 3), name) == 0) {
            // set the element type to 0xff (deleted)
            *p = 0xff;
            // return true
            return true;
        }

        // move to the next element
        p += size;
    }

    // return false
    return false;
}

// function to print a variable
static void print_var(char *e) {
    // print the variable name
    printf("Name: %s ", get_var_name(e));
    // get the variable type
    uint8_t vType = get_var_type(e);
    // print the variable type as "null", "boolean", "char", "int", "float" or "string"
    switch (vType) {
    case 0x00:
        printf("Type: null ");
        break;
    case 0x01:
        printf("Type: boolean ");
        break;
    case 0x02:
        printf("Type: char ");
        break;
    case 0x03:
        printf("Type: int ");
        break;
    case 0x04:
        printf("Type: float ");
        break;
    case 0x05:
        printf("Type: string ");
        break;
    default:
        printf("Type: unknown ");
        break;
    }
    // print the variable value according to the variable type
    switch (vType) {
    case 0x00:
        printf("Value: null ");
        break;
    case 0x01:
        printf("Value: %s ", get_var_bool(e) ? "true" : "false");
        break;
    case 0x02:
        printf("Value: %d ", get_var_char(e));
        break;
    case 0x03:
        printf("Value: %d ", get_var_int(e));
        break;
    case 0x04:
        printf("Value: %f ", get_var_float(e));
        break;
    case 0x05:
        printf("Value: %s ", get_zstring(get_var_zstr(e), cBuf));
        break;
    default:
        printf("Value: unknown ");
        break;
    }
    printf("\n");
}

// function to print an element
static void print_element(char *p) {
    // get the type of the element
    uint8_t type = *p;
    // get the size of the element
    uint16_t size = get_element_size(p);

    // print the size of the element
    printf("Size: %d ", size);

    // print the type of the element as "var", "code", "empty" or "deleted"
    switch (type) {
    case 0x01:
        printf("Var: ");
        break;
    case 0x02:
        printf("Code: ");
        break;
    case 0x00:
        printf("Empty: ");
        break;
    case 0xff:
        printf("Deleted: ");
        break;
    default:
        printf("Unknown: ");
        break;
    }

    char *e = p + 3;

    // print the contents of the element depending on the type
    switch (type) {
    case 0x01:
        // print the variable
        print_var(e);
        break;
    default:
        // print the size of the unknown area
        printf("Size: %d ", size - 3);
        printf("\n");
        break;
    }
}

// function to list all the elements in the partition
static void list_elements(char *p) {
    // create a pointer to the element's content
    char *e;
    // loop through all the elements in the partition
    while (p < pEnd) {
        // get the size of the element
        uint16_t size = get_element_size(p);
        // print the element
        print_element(p);
        // move to the next element
        p += size;
    }
    printf("End of partition\n");
    // print the size of the empty area at the end of the partition
    printf("Empty area size: %d\n", get_element_size(pEnd));
}

#endif