#include <string.h>

// for sending data through port A
#define ddra (*(volatile char*)0x21)
#define data (*(volatile char*)0x22)

// for controlling RS and Enable pin on port F
#define ddrf (*(volatile char*)0x30)
#define outf (*(volatile char*)0x31)

#define RS 0
#define E  1

void toggle_e()
{
    // set enable pin high for making ready for incoming data 

    outf |= (1<<E);
    // wait for 2 microseconds 
    for(volatile long d = 0; d<=10; d++);
    // set enable pin low for snatching data
    outf &= ~(1<<E);

}
// data sent will be used for displaying characters
void send_data(char str)
{
    outf |= (1<<RS);
    data = str;
    toggle_e();
    // delay for 100 microseconds
    for(volatile long d = 0; d<=1000; d++); 
}
// data sent will be used for using internal commands like  cursor blink,screen clear etc.
void send_command(char str)
{
    outf &= ~(1<<RS);
    data = str;
    toggle_e();
    // delay for 100 microseconds
    for(volatile long d = 0; d<=1000; d++);

}
void displayString(char *str)
{
    while(*str != '\0')
    {
        send_data(*str);
        str++;
    }
}
void displayRightJustified(char *str, char line)
 {
    char length = strlen(str);
    
    char line_base_add = (line==1)? 0x80 : 0xC0;

    char start_col = 16 - length;
    
    send_command(line_base_add + start_col);

    displayString(str);
}
void display_mid(char *str,char line)
{
    char length = strlen(str);
    
    char line_base_add = (line==1)? 0x80 : 0xC0;

    char start_col = (16 - length)/2;
    
    send_command(line_base_add + start_col);

    displayString(str);
}
void lcd_init()
{
    // Wait for hardware power stabilization at boot
    for(volatile long d = 0; d<=10000; d++); 
    
    // Step 2: Force Reset sequence 
    send_command(0x30);
    for(volatile long d = 0; d<=5000; d++);
    send_command(0x30);
    send_command(0x30);
    
    send_command(0x38); // Function set: 8-bit, 2-lines, 5x8 font
    send_command(0x0C); // Display ON, Cursor OFF
    
    send_command(0x01); // Clear Screen
    for(volatile long d = 0; d<=2500; d++); // Extra long delay for hardware clear (~2ms)
    
    send_command(0x06); // Entry mode: Increment cursor right
}
void scrollRight(char *str, char line)
{
    char length = strlen(str);
    char line_add = (line==1)? 0x80 : 0xC0 ;
    
    for(char scroll = 16; scroll >= 0; scroll--)
    {
        send_command(0x01); // clears screen 
        for(volatile long d = 0; d <= 2000; d++);
        send_command(line_add + scroll);
        displayString(str);
        for(volatile long d = 0; d <= 200000; d++);
    }
}
void scrollLeft(char *str, char line)
{
    char length = strlen(str);
    char line_add = (line==1)? 0x80 : 0xC0 ;
    
    for(char scroll = 0; scroll <= 16-length; scroll++)
    {
        send_command(0x01); // clears screen 
        for(volatile long d = 0; d <= 2000; d++);
        send_command(line_add + scroll);
        displayString(str);
        for(volatile long d = 0; d <= 200000; d++);
    }
}

void scroll_Left_Mid(char *str, char line)
{
    char length = strlen(str);
    char line_base = (line == 1) ? 0x80 : 0xC0;
    char target_mid = (16 - length) / 2;
    
    // Phase 1: Smooth Entry (The string slides in character by character from the left edge)
    for (char i = 1; i <= length; i++)
    {
        send_command(0x01); // Clear frame
        for(volatile long d = 0; d <= 2500; d++);
        
        send_command(line_base); // Stay anchored at the first column of the line
        
        // Print only the end slice of the string that is slipping onto the screen
        displayString(str + (length - i)); 
        
        for(volatile long d = 0; d <= 200000; d++);
    }
    
    // Phase 2: Shift to Center (The whole string moves across the screen to its middle home)
    for (char scroll = 1; scroll <= target_mid; scroll++)
    {
        send_command(0x01); // Clear frame
        for(volatile long d = 0; d <= 2500; d++);
        
        send_command(line_base + scroll); // Shift the absolute starting address forward
        displayString(str);               // Print the whole string
        
        for(volatile long d = 0; d <= 200000; d++);
    }
}
void scroll_Right_Mid(char *str, char line)
{
    char length = strlen(str);
    char line_add = (line==1)? 0x80 : 0xC0 ;
    
    for(char scroll = 16; scroll >= (16-length)/2; scroll--)
    {
        send_command(line_add);
        displayString("                "); // clears screen 
        for(volatile long d = 0; d <= 2000; d++);
        send_command(line_add + scroll);
        displayString(str);
        for(volatile long d = 0; d <= 50000; d++);
    }
}
int main()
{
    // setting port A and F as output ports
    ddra = 0xFF;
    ddrf = 0xFF;

    lcd_init();

    scroll_Right_Mid("Welcome to my",1);
    for(volatile long d = 0; d <= 200000; d++);
    scroll_Right_Mid("16x2 LCD driver",2);
    for(volatile long d = 0; d <= 500000; d++);
 
    send_command(0x01);

    scroll_Right_Mid("Project by",1);
    for(volatile long d = 0; d <= 500000; d++);
    display_mid("Amitabh Pathak",2);
    for(volatile long d = 0; d <= 500000; d++);

    while(1)
    {
        // nothing to do 
    }
    return 0;
}
