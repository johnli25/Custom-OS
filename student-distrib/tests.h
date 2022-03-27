#ifndef TESTS_H
#define TESTS_H

#include "i8259.h"
#include "filesys.h"

// test launcher
int read_valid_file();
int read_valid_file2();
int read_nonexisting_file();
int read_invalid_large_file();
int read_file_index();
int read_file_index_invalid();
int ls_dir_test();

void launch_tests();

#endif /* TESTS_H */
