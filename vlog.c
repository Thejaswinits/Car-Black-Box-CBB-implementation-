#include<xc.h>
#include "vlog.h"
#include"EEP.h"
#include"clcd.h"
#include"uart.h"
#include"matrix_keypad.h"
#include"password.h"

//declaring variables globally and getting value of a varibales/string from other files using extern keyword
unsigned char read = 0x00;
extern int inc,option,pass,value,scroll;
extern char str[9];
extern int count8;
int cl = 0;
extern unsigned char download1[17],clear[17];
char display[17];
char display1[17]; 
int pre=2;
unsigned char set[9] = "00:00:00";
extern char log[5][16];
extern char*line[2];

void vlog()
{
    //if value is 1,then view log is done
    if ( value == 0 )
    {
	if ( pre == 2)//condition to intialise inc for first time
	{
	    inc = 1;
	    pre = 0;
	}

	if ( inc )//if inc is 1, then scroll down is done
	{
	    if ( read == 0xA0 )//reintialise read variable if it reaches 160
		read = 0x00;

	    //outer for loop to perform the operation twice,for storing the values to 2 array
	    for ( int i = 0; i<2;i++)
	    {
		//inner for loop to store the values in the eeprom to an array
		for ( int j = 0;j<16;j++ )
		{
		    if ( i == 0 )
			display[j] = read_EEP(read);
		    else
			display1[j] = read_EEP(read);
		    read++;
		    if ( read == 0xA0 && i == 0 )
			read = 0x00;
		}
		if ( i == 0 )
		{
		    display[16]='\0';
		}
		else
		{
		    display1[16]='\0';
		}
	    }
	    //reintilaise the variable once the storing operation is complete
	    if ( read != 0x00 )
		read = read - 0x10;
	}
	else//if inc is zero, then scroll up is done 
	{
	    if ( read == 0x00 )
		read = 0xA0;

	    //outer for loop to perform the operation twice,for storing the values to 2 array
	    for ( int i = 0; i<2;i++)
	    {
		//inner for loop to store the values in the eeprom to an array
		for ( int j = 15; j>=0 ;j-- )
		{
		    read--;
		    if ( i == 0 )
			display1[j] = read_EEP(read);
		    else
			display[j] = read_EEP(read);
		    if ( read == 0x00 && i == 0 )
			read = 0xA0;
		}
		if ( i == 0 )
		{
		    display1[16]='\0';
		}
		else
		{
		    display[16]='\0';
		}
	    }
	    //reintilaise the variable once the storing operation is complete
	    if ( read != 0xA0 )
		read = read+0x10;
	}

	//input to clcd 
	clcd_print(display,LINE1(0));
	clcd_print(display1,LINE2(0));
	//reintilaising line array elements
	line[0] = log[0];
	line[1] = log[1];
	scroll = 0;
    }
    //if value eqauls 1,then displayig the data in minicom is performed
    if ( value == 1 )
    {
	//input to clcd 
	clcd_print("DISPLAYING DATA ",LINE1(0));
	clcd_print("IN MINICOM      ",LINE2(0));
	if ( !cl )//cl is the variable which decides clear operation is performed or not, if not performed then dispalying last 10 datas
	{
	    //initialisng variable,array and function call
	    int i = 0;
	    init_uart();
	    unsigned char download[17];
	    download[16] = '\0';
	    read = 0x00;

	    //while loop runs for 10 times
	    while ( i++ < 10 )
	    {
		//for loop to upload data to the array and dispalying it to clcd
		for ( int i = 0 ;i< 16; i++ )
		{
		    download[i] = read_EEP(read);
		    read++;
		    if ( read == 0xA0)//reintialise if value of the variable reaches 160
			read = 0x00;
		}
		puts ( download );//input to minicom
		puts ( "\n\r" );//new line and moving firstline
	    }
	}
	else//if clear is done,then displayong the clear data and download data
	{
	    puts ( clear );//input to minicom which displays the clear time data
	    puts ( "\n\r");//new line and moving to firstindex
	}
	puts ( download1 );//input to minicom,which prints the downloaded data time
	puts ( "\n\r");

	for (long int i =0 ; i < 500000 ; i++ );//delay to resume clcd display

	//reinitialise values to array and variables
	pass = 0;
	line[0] = log[0];
	line[1] = log[1];
	scroll=0;
    }
    //if value is 2,then clear log is done
    if ( value == 2 )
    {
	//input to clcd
	clcd_print("  CLEAR LOG IS  ",LINE1(0));
	clcd_print("   SUCCESSFULL  ",LINE2(0));

	//storing the space as data upto 160 eeprom address
	for ( unsigned char i = 0x00; i < 0xA0; i++ )
	{
	    write_EEP(i,' ');
	}
	for (long int i =0 ; i < 350000 ; i++ );//delay to rsume the clcd display

	//reinitialise the variable ana array
	pass = 0;
	cl = 1;
	line[0] = log[0];
	line[1] = log[1];
	scroll=0;
    }

    //if value equals three then reset password is done 
    if ( value == 3 )
    {
	//declaring variables and array
	char password[9];
	password[8] = '\0';
	long int wait=0,wait1=0,wait2=0,wait3=0,i=0,flag =1,flag1 = 0,i=0;
	unsigned char key;

	//input to clcd 
	clcd_print(" ENTER NEW PASS ",LINE1(0));
	clcd_print("                ",LINE2(0));

	//function call
	init_clcd();
	init_matrix_keypad();

	while(1)//super loop
	{
	    key = read_switches(STATE_CHANGE);//reading which key is pressed
	    if ( key != ALL_RELEASED )
		for ( unsigned int i = 1000; i--;);
	    if ( i < 8 )//getting inputs for 8 times, so i should be less than 8
	    {
		if ( flag )
		{
		    clcd_print(" ENTER NEW PASS ",LINE1(0));
		    //based on the delay,giving input ot 2nd line of clcd
		    if ( wait++ <= 1000 )
		    {
			clcd_print("_",LINE2(i));
		    }
		    else
		    {
			if ( wait == 2000 )
			{
			    wait = 0;
			}
			clcd_print(" ",LINE2(i));
		    }

		    //if switch 1 is pressed then 1 is entered, comparing it with the particular ith index of pass string
		    if ( key == MK_SW8 )
		    {
			clcd_print("*",LINE2(i));
			password[i] = '1';
			i++;//incrementing index i
			wait = 0;
		    }
		    //if switch 2 is pressed then 0 is entered, comparing it with the particular ith index of pass string
		    if ( key == MK_SW9 )
		    {
			clcd_print("*",LINE2(i));
			password[i] = '0';
			i++;//incrementing index i
			wait = 0;
		    }
		}
		else
		{
		    clcd_print("CONFIRM PASSWORD",LINE1(0));
		    //based on the delay,giving input ot 2nd line of clcd
		    if ( wait1++ <= 1000 )
		    {
			clcd_print("_",LINE2(i));
		    }
		    else
		    {
			if ( wait1 == 2000 )
			{
			    wait1 = 0;
			}
			clcd_print(" ",LINE2(i));
		    }

		    //if switch 1 is pressed then 1 is entered, comparing it with the particular ith index of pass string
		    if ( key == MK_SW8 )
		    {
			clcd_print("*",LINE2(i));
			//if doesnt match, make flag to 1
			if ( password[i] != '1' )
			    flag1 = 1;
			i++;//incrementing index i
			wait1=0;
		    }
		    //if switch 2 is pressed then 0 is entered, comparing it with the particular ith index of pass string
		    if ( key == MK_SW9 )
		    {
			clcd_print("*",LINE2(i));
			//if doesnt match, make flag to 1
			if ( password[i] != '0' )
			    flag1 = 1;
			i++;//incrementing index i
			wait1 = 0;
		    }

		}
	    }
	    else
	    {
		if ( flag )//if flag value is 1,then after some delay reset some values and clearing the 2nd line of clcd
		{
		    if ( wait2++ == 2000)
		    {
			flag = 0;
			i = 0;
			wait2 = 0;
			clcd_print("                ",LINE2(0));
		    }
		}
		else//if flag value is zero,then based on flag1 value password reset is successfull or failure is identified
		{
		    if ( flag1 )//flag1 value indicates password reset is failure and try again 
		    {
			//input to clcd
			clcd_print("PASSWORD DOESNT ",LINE1(0));
			clcd_print("  MATCH RETRY   ",LINE2(0));

			//after particular delay reset all values and clear 2nd line of clcd
			if ( wait3++ == 1000 )
			{
			    flag = 1;
			    i = 0;
			    flag1 = 1;
			    wait1 =0;
			    wait = 0;
			    wait2 =0;
			    wait3 = 0;
			    clcd_print("               ",LINE2(0));
			}
		    }
		    else//flag1 value 0 indicates password reset is successfull
		    {
			//reintialising the str array element using newly getted input which is stores in password array
			for (int i = 0; i<9;i++ )
			{
			    str[i] = password[i];
			}

			//input to clcd
			clcd_print(" PASSWORD RESET ",LINE1(0));
			clcd_print(" IS SUCCESSFULL ",LINE2(0));
			for ( long int i = 0;i<100000;i++ );//delay to resume the clcd display
			break;//come out from superloop
		    }
		}

	    }

	}
	scroll=0;
    }

    //if vlaue equals 4,then set time is done
    if ( value == 4 )
    {
	//declaring variables and array
	char space[5] = "    ";
	set[8] ='\0';
	unsigned char key;
	int min =0,sec=0,hr=0,flag = 0,wait = 0,count11=0,count12=0,count9 = 0;

	//input to clcd
	clcd_print("   SET - TIME   ",LINE1(0));
	clcd_print(space,LINE2(0));
	clcd_print(set,LINE2(4));
	clcd_print(space,LINE2(12));
	init_matrix_keypad();//function call

	while ( 1 )//superloop
	{
	    key = read_switches(LEVEL_CHANGE);//read which key is pressed
	    for ( int i =0; i<1000;i++ );//delay to remove the bouncing effect

	    if ( key == MK_SW11 )//sw11 to increment te particular field
	    {
		if (count11++ == 200 )
		{
		    count11 = 100;
		}
	    }
	    else if ( count11 <= 200 && count11 != 0 )
	    {
		//based on flag value increment the particular filed is choosed, flag 0 is for setting ec index, flag 1 is for setting minute index, and flag 2 is for setting hour index

		if ( flag == 0 )
		{
		    sec++;
		    if ( sec == 60 )
			sec = 0;
		    set[7] = (sec%10)+48;
		    set[6] = (sec/10)+48;

		}
		if ( flag == 1 )
		{
		    min++;
		    if ( min == 60)
			min = 0;
		    set[4] = (min%10)+48;
		    set[3] = (min/10)+48;
		}
		if ( flag == 2 )
		{
		    hr++;
		    if ( hr == 24 )
			hr = 0;
		    set[1] = (hr%10)+48;
		    set[0] = (hr/10)+48;
		}
		count11 = 0;
	    }
	    else
		count11 = 0;

	    //key12 is to choose and set the particular field
	    if ( key == MK_SW12 )
	    {
		if ( count12++ == 200 )
		    count12 = 100;
	    }
	    else if ( count12 < 200 && count12 != 0 )
	    {
		wait = 60;
		flag++;
		if ( flag == 3 )
		    flag = 0;
		count12 =0;
	    }
	    else
		count12 = 0;

	    //key 9 is to set the value
	    if ( key == MK_SW9 )
	    {
		count9++;
	    }
	    else if ( count9 > 200 )
	    {
		count9 =0;
		break;
	    }
	    else
		count9 = 0;

	    //based on the wait value input to clcd is given
	    if ( wait++ < 50 )
	    {
		if ( flag == 0 )
		{
		    clcd_print("  ",LINE2(10));
		}
		else if ( flag == 1 )
		{
		    clcd_print("  ",LINE2(7));
		}
		else
		    clcd_print("  ",LINE2(4));
	    }
	    else
	    {
		if ( wait == 100 )
		    wait = 0;
		clcd_print(set,LINE2(4));
	    }
	}

    }
}

