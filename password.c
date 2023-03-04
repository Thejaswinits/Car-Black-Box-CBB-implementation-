#include"clcd.h"
#include"matrix_keypad.h"
#include "password.h"
#include<xc.h>

extern int success;

char str[9] = "00001111";
int password(void)
{
								//declaring and intialsing variables and array
								unsigned char key;
								unsigned int flag = 0,i = 0,wait2 =0,wait1 = 0,wait = 0,wait3= 0;
								unsigned char try = '5';
								TRISB0 = 0;

								clcd_print("ENTER PASSWORD  ", LINE1(0));//input to firstline of clcd
								clcd_print("                ", LINE2(0));//input to firstline of clcd

								while(1)
								{
																if ( try > '0' )//if no of attempts is less than 0 then type password, else execute else part
																{
																								key = read_switches(STATE_CHANGE);//reading which key is pressed

																								for( int i = 2000; i--; );//delay to remove bouncing effect

																								if ( i < 8 )//getting inputs for 8 times, so i should be less than 8
																								{
																																//based on the delay,giving input ot 2nd line of clcd
																																if ( wait++ <= 500 )
																																{
																																								clcd_print("_",LINE2(i));
																																}
																																else
																																{
																																								if ( wait == 1000 )
																																								{
																																																wait = 0;
																																								}
																																								clcd_print(" ",LINE2(i));
																																}

																																//if switch 1 is pressed then 1 is entered, comparing it with the particular ith index of pass string
																																if ( key == MK_SW8 )
																																{
																																								clcd_print("*",LINE2(i));
																																								//if doesnt match, make flag to 1
																																								if ( str[i] != '1' )
																																																flag = 1;
																																								i++;//incrementing index i
																																}
																																//if switch 2 is pressed then 0 is entered, comparing it with the particular ith index of pass string
																																if ( key == MK_SW9 )
																																{
																																								clcd_print("*",LINE2(i));
																																								//if doesnt match, make flag to 1
																																								if ( str[i] != '0' )
																																																flag = 1;
																																								i++;//incrementing index i
																																}

																								}
																								else//based on flag value displaying success or failure or attempts left on clcd
																								{
																																if ( flag )//if flag is 0, decrement try upto try reaches 0
																																{
																																								if ( try > '1' )//if try is less than one decrement try, reset all values and flag
																																								{
																																																//giving input to clcd lines based on delay
																																																if ( wait1++ <= 1000 )
																																																{
																																																								clcd_print("    INCORRECT   ",LINE2(0));
																																																}
																																																else
																																																{
																																																								if ( wait2++ <= 500 ) 
																																																								{
																																																																clcd_print("ONLY ", LINE1(0));
																																																																clcd_putch(try-1,LINE1(5));
																																																																clcd_print(" ATTEMPTS", LINE1(6));
																																																																clcd_print("                ",LINE2(0));
																																																								}
																																																								else
																																																								{
																																																																if(wait2 == 1000 )
																																																																{
																																																																								wait1 = 0;
																																																																								wait2 = 0;
																																																																								flag = 0;
																																																																								try--;
																																																																								i = 0;
																																																																								RB0 = 0;
																																																																}
																																																																clcd_print("ENTER PASSWORD  ",LINE1(0));
																																																								}
																																																}
																																																//delay to blink led if password entered is wrong
																																																if ( wait3++ == 50 & i != 0 )
																																																{
																																																								RB0 = !(RB0);
																																																								wait3 = 0;
																																																}
																																								}
																																								else//if try is 1 and flag is 1 then make try to 0
																																								{
																																																try='0';
																																																flag = 1;
																																								}
																																}
																																else
																																								try = '0';//if flag is 0, make try as 0 


																								}
																}
																else
																{
																								if ( flag )//based on flag,dispalay either success or failure
																								{
																																clcd_print("     FAILURE    ",LINE2(0));
																																clcd_print("ENTER PASSWORD   ", LINE1(0));
																																RB0 = 1;
																																success = 0;
																																break;
																								}
																								else
																								{
																																clcd_print("    SUCCESS     ",LINE2(0));
																																clcd_print("ENTER PASSWORD   ", LINE1(0));
																																success = 1;
																																break;
																								}

																}
								}
}

