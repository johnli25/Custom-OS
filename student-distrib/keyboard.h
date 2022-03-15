/*
The read system call reads data from the keyboard, a file, device (RTC), or directory.
 This call returns the number of bytes read. If the initial file position is at or beyond
  the end of file, 0 shall be returned (for normal files and the directory). In the case
   of the keyboard, read should return data from one line that has been terminated by 
   pressing Enter, or as much as fits in the buffer from one such line. The line returned 
   should include the line feed character.
*/

