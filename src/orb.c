// ZX80 - Zmall Xtensible 80's language
// (c) 2023 by Marcelo Dantas
// Small interpreter for the ZX80 language in ANSI C
// This is a toy language, not intended for real use
// Should compile fine on any C compiler
//
// Usage:
//  zx80 <cmd>
//

// ZX80 implements a z-string, which is a string with a size byte
// at the beginning. The size byte is unsigned char.
// This string therefore can contain the 0x00 byte.
// The z-string is defined as follows:
//     <size> is unsigned char
//     <string> is a sequence of <size> characters
//

// ZX80 implements a z-variable, which is defined as follows:
//      <8 bit var_type> where:
//          0x00 - null
//          0x01 - boolean
//          0x02 - char
//          0x03 - int
//          0x04 - float
//          0x05 - double
//          0x06 - z-string
//      <8 bit name_size> is unsigned char
//      <name> is a sequence of <name_size> characters
//      <8 bit value_size> is unsigned char
//      <value> is the value of the variable
// Variable rules:
//      The variable name is case sensitive.
//      The variable name can contain letters, digits and the underscore character.
//      The variable name can't start with a digit.
//      The variable name can't be a reserved word.
//      The variable name can't be longer than 8 characters by default.
//      The size of the int and float variables is calculated by the compiler for portability.
//

// ZX80 implements a memory partition.
// The partition contains elements which can be a variable element, a code element, an empty element or a deleted element.
// The partition elements are defined as follows:
//      <8 bit element_type> where:
//          0x00 - empty area
//          0x01 - variable element
//          0x02 - code element
//          0xFF - deleted element
//      <16 bit size> is unsigned short
//      <element> is the element
//
// Partition rules:
//      The element size includes the element type and the size itself.
//      The partition occupied size is the sum of the sizes of all the elements in the partition.
//      The partition size is the size of the memory area allocated for the partition.
//      The partition free size is the partition size minus the partition occupied size.
//      The elements are added to the partition in the order they are defined.
// Inialiinig the partition:
//      The partition is initialized with an empty area of the partition size.
// Adding elements:
//      When an element is added to the partition, the partition free size is checked.
//      If the partition free size is less than the element size, the partition is compacted.
//      The partition is compacted by moving all the elements to the beginning of the partition.
//      When the partition is compacted, the deleted areas are removed.
//      If after the compaction the partition free size is still less than the element size, null is returned.
//      When an element is added, if there's a deleted area of the same size, the deleted area is replaced by the element.
//      When an element is added, if there's a deleted area of a bigger size, the deleted area is replaced by the element and the remaining area is added as a deleted area.
// Deleting elements:
//      When an element is deleted, the element is replaced by a deleted area.
//      When an element is deleted, if the deleted area is adjacent to an empty area, the two areas are merged.
//

// ZX80 command syntax:
//      c{all} <label> - Calls the function label
//      d{o} <cmd> - Executes the command
//      e{lse} <cmd> - Executes the command if the previous if was false
//      g{oto} <label> - Jumps to the label
//      i{f} <expr> <cmd> - Executes the command if the expression is true
//      k{ill} <var> - Kills the variable
//      q{uit} - Returns from the function
//      r{ead} <var> - Reads the variable from the keyboard
//      s{et} <var>=expr - Sets the variable to the expression
//      w{rite} expr - Writes the expression to the screen
//
// Expression syntax:	
//      <expr> = <expr> + <expr> - Addition
//      <expr> = <expr> - <expr> - Subtraction
//      <expr> = <expr> * <expr> - Multiplication
//      <expr> = <expr> / <expr> - Division
//
// Labels are defined with the following syntax:
//      <label>:
// Label rules:
//      The label name is case sensitive.
//      The label name can contain letters, digits and the underscore character.
//      The label name can't start with a digit.
//      The label name can't be a reserved word.
//      The label name can't be longer than 8 characters by default.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "orb.h"

// main program
int main(int argc, char *argv[]) {
    // Check if we have a command to run
    //if (argc < 2) {
    //    printf("Usage: orb <cmd>\n");
    //    return 1;
    //}

    // allocate the partition
    alloc_partition();

    // initialize the partition as a fully empty area
    init_partition();

    // print the partition size
    printf("Partition size: %d\n", pSize);

    // load a null variable into the variable buffer vBuf1 and add it to the partition
    load_null_var(vBuf1, "myNull");
    add_var(vBuf1);

    // load a boolean variable into the variable buffer vBuf1 and add it to the partition
    load_bool_var(vBuf1, "myBool", true);
    add_var(vBuf1);

    // load a char variable into the variable buffer vBuf1 and add it to the partition
    load_char_var(vBuf1, "myChar", 'a');
    add_var(vBuf1);

    // load an int variable into the variable buffer vBuf1 and add it to the partition
    load_int_var(vBuf1, "myInt", 123);
    add_var(vBuf1);

    // load a float variable into the variable buffer vBuf1 and add it to the partition
    load_float_var(vBuf1, "myFloat", 123.456);
    add_var(vBuf1);

    // load a string variable into the variable buffer vBuf1 and add it to the partition
    load_string_var(vBuf1, "myString", "Hello World!!!");
    add_var(vBuf1);

    // load another float variable into the variable buffer vBuf2 and add it to the partition
    load_float_var(vBuf1, "myFloat2", 32.768);
    add_var(vBuf1);

    // delete the variable myInt
    delete_var("myInt");

    // load an int variable into the variable buffer vBuf1 and add it to the partition
    load_int_var(vBuf1, "myInt", 666);
    add_var(vBuf1);

    // delete the variable myInt
    delete_var("myInt");

    // load a float variable into the variable buffer vBuf1 and add it to the partition
    load_float_var(vBuf1, "myFloat3", -31.337);
    add_var(vBuf1);

    // print the contents of the partition
    printf("Partition contents:\n");
    list_elements(pStart);

    // load an int variable into the variable buffer vBuf1 and add it to the partition
    load_int_var(vBuf1, "otherInt", 123);
    add_var(vBuf1);

}
