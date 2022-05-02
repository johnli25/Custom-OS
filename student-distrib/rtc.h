#ifndef RTC_H
#define RTC_H
// Most used IRQ line connected to RTC on x86 
#define RTC_IRQ         8

// Most used command & data port on x86 machines
#define RTC_CMD         0x70
#define RTC_DATA        0x71
#define virtFreq        1024
#define nByteCheck      4
#define relativeFreq    5000


//RTC initialization
extern void initialize_RTC(void);
//RTC interrupt call 
extern void interrupt_RTC(void);
//RTC system calls
extern int32_t open_RTC (const uint8_t* filename);
extern int32_t read_RTC (int32_t fd, void* buf, int32_t nbytes);  
extern int32_t write_RTC (int32_t fd, const void* buf, int32_t nbytes);
extern int32_t close_RTC (int32_t fd);

#endif
