/*
The read system call reads data from the keyboard, a file, device (RTC), or directory.
 This call returns the number of bytes read. If the initial file position is at or beyond
  the end of file, 0 shall be returned (for normal files and the directory). In the case
   of the keyboard, read should return data from one line that has been terminated by 
   pressing Enter, or as much as fits in the buffer from one such line. The line returned 
   should include the line feed character.
*/

//call enabble irq with the irq number that corresponds to the keyboard

//set_IDT_Entry - IDT hex (keyboard) pass in the handler as present 

// handler has to deal with grabbing the specefic byte/data from the port - echo back to screen

//push all registers

//push all flags

//call hander name (.c) file

//after returns - have to restore registers

//iret

//i8259.c to take the interrupt correctly for the keyboard

void initialize_Keyboard();

void interrupt_keyboard();