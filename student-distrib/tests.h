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

//RTC tests 
int rtc_test_open() ;
int rtc_test_close();
int rtc_test_read_write_invalid_freq() ;
int rtc_test_read_write_invalid_size();
int rtc_test_read_write();

void launch_tests();

#endif /* TESTS_H */
