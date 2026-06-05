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

void rotate_leftToright(char *str, char line)
{
    char length = strlen(str);
    char line_base = (line == 1) ? 0x80 : 0xC0;
    // char target_mid = (16 - length) / 2;
    
    // Phase 1: Smooth Entry (The string slides in character by character from the left edge)
    for (char i = 1; i <= length; i++)
    {
        displayString("                ");
        for(volatile long d = 0; d <= 2500; d++);
        
        send_command(line_base); // Stay anchored at the first column of the line
        
        // Print only the end slice of the string that is slipping onto the screen
        displayString(str + (length - i)); 
        
        for(volatile long d = 0; d <= 200000; d++);
    }
    
    // Phase 2: Shift to Center (The whole string moves across the screen to its middle home)
    for (char scroll = 1; scroll <= 16; scroll++)
    {
        send_command(line_base);
        for(char i = 0; i<=scroll; i++)
        {
        displayString(" "); // Clear frame
        for(volatile long d = 0; d <= 2500; d++);
        }
        
        send_command(line_base + scroll); // Shift the absolute starting address forward
        displayString(str);               // Print the whole string
        
        for(volatile long d = 0; d <= 200000; d++);
    }
}
void rotate_rightToleft(char *str, char line)
{
    int length = strlen(str);
    char line_base = (line == 1) ? 0x80 : 0xC0;

    // Phase 1: Slide IN from the right side (16 down to 0)
    for(int rot = 16; rot >= 0; rot--)
    {
        // Clear the line briefly or overwrite carefully
        send_command(line_base);
        displayString("                "); // 16 spaces to clear
        
        // Move cursor to the starting position and print string
        send_command(line_base + rot);
        displayString(str);
        
        for(volatile long d = 0; d <= 200000; d++); // Delay
    }

    // Phase 2: Slide OUT smoothly to the left
    for(int depart = 1; depart <= length; depart++)
    {
        // 1. Reset cursor to the very beginning of the line
        send_command(line_base);
        
        // 2. Print the string starting from the 'depart' index
        //    (e.g., if str is "HELLO", str+1 is "ELLO")
        displayString(str + depart);
        
        // 3. Clear the trailing characters on the right so they don't ghost
        for(int i = 0; i < depart; i++) {
            displayString(" "); 
        }
        
        for(volatile long d = 0; d <= 200000; d++); // Delay
    }
    
    // Final cleanup: ensure the line is completely blank at the end
    send_command(line_base);
    displayString("                ");
}

void rotate_opposite(char *str1, char line_1, char *str2, char line_2)
{
    // will rotate left to right the first string 
    char length_1 = strlen(str1);
    char line_base_1 = (line_1 == 1) ? 0x80 : 0xC0;
    // char target_mid = (16 - length) / 2;
    
    // Phase 1: Smooth Entry (The string slides in character by character from the left edge)
    for (char i = 1; i <= length_1; i++)
    {
        displayString("                ");
        for(volatile long d = 0; d <= 2500; d++);
        
        send_command(line_base_1); // Stay anchored at the first column of the line
        
        // Print only the end slice of the string that is slipping onto the screen
        displayString(str1 + (length_1 - i)); 
        
        for(volatile long d = 0; d <= 200000; d++);
    }
    
    // Phase 2: Shift to Center (The whole string moves across the screen to its middle home)
    for (char scroll = 1; scroll <= 16; scroll++)
    {
        send_command(line_base_1);
        for(char i = 0; i<=scroll; i++)
        {
        displayString(" "); // Clear frame
        for(volatile long d = 0; d <= 2500; d++);
        }
        
        send_command(line_base_1 + scroll); // Shift the absolute starting address forward
        displayString(str1);               // Print the whole string
        
        for(volatile long d = 0; d <= 200000; d++);
    }

    // will rotate the second string right to left

     int length_2 = strlen(str2);
    char line_base_2 = (line_2 == 1) ? 0x80 : 0xC0;

    // Phase 1: Slide IN from the right side (16 down to 0)
    for(int rot = 16; rot >= 0; rot--)
    {
        // Clear the line briefly or overwrite carefully
        send_command(line_base_2);
        displayString("                "); // 16 spaces to clear
        
        // Move cursor to the starting position and print string
        send_command(line_base_2 + rot);
        displayString(str2);
        
        for(volatile long d = 0; d <= 200000; d++); // Delay
    }

    // Phase 2: Slide OUT smoothly to the left
    for(int depart = 1; depart <= length_2; depart++)
    {
        // 1. Reset cursor to the very beginning of the line
        send_command(line_base_2);
        
        // 2. Print the string starting from the 'depart' index
        //    (e.g., if str is "HELLO", str+1 is "ELLO")
        displayString(str2 + depart);
        
        // 3. Clear the trailing characters on the right so they don't ghost
        for(int i = 0; i < depart; i++) {
            displayString(" "); 
        }
        
        for(volatile long d = 0; d <= 200000; d++); // Delay
    }
    
    // Final cleanup: ensure the line is completely blank at the end
    send_command(line_base_2);
    displayString("                ");
}
int main()
{
    // setting port A and F as output ports
    ddra = 0xFF;
    ddrf = 0xFF;

    lcd_init();

   display_mid("Welcome To",1);

    while(1)
    {
        rotate_opposite("Project by",1,"Amitabh Pathak",2);
    }
    return 0;
}
