/*DOCUMENTATION
NAME : THEJASWINI T S
DATE : 04-03-23
DESCRIPTION : Car Black Box.
*/
#include<xc.h>
#include"clcd.h"
#include"matrix_keypad.h"
#include"password.h"
#include"uart.h"
#include"i2c.h"
#include"ds1307.h"
#include"adc.h"
#include"rtc.h"
#include"EEP.h"
#include"vlog.h"
#include"EEP.h"

int keypress(unsigned char);//function prototype

void init_config()
{
    //functio call
    init_clcd();
    init_uart();
    init_matrix_keypad();
    init_i2c();
    init_ds1307();
    init_adc();
}

//declaring variables and strings globally
unsigned char time[9];
unsigned char store[17];
unsigned char clock_reg[3];
int count1= 0,count2= 0,count3=0,count8=0,count9=0;
int scroll = 0,option= 0,success = 0,e=1,scroll1=0,inc = 0,value= 0;
unsigned char eve[3] = "ON";
unsigned char add = 0x00;
char log[5][16]={"VIEW LOG       ","DOWNLOAD LOG    ","CLEAR LOG      ","CHANGE PASSWORD","SET TIME       "};
char*line[2];
int pass = 0,yes=1;
extern unsigned char read;
unsigned char download1[17];
unsigned char clear[17];
extern unsigned char set[9];

void main()
{
    init_config();//function call

    //declaring the variables and strings
    unsigned char key;
    unsigned int adc;
    char speed[4] = "000";

    //input to the clcd
    clcd_print("TIME     EVE SPD",LINE1(0));
    clcd_print("00:00:00  ON 000",LINE2(0));

    //assigning null character to the last element of array
    download1[16] = '\0'; 
    clear[16] = '\0'; 
    store[16]='\0';

    //assigning values to the array elements
    line[0] = log[0];
    line[1] = log[1];
    store[12] = ' ';
    store[11]=eve[1];
    store[10]=eve[0];
    store[9]=' ';
    store[8]=' ';

    while(1)//super loop
    {

	//if address reaches 160,reseting it
	if ( add == 0xA0 )
	    add = 0x00;

	key = read_switches(LEVEL_CHANGE);//reading which key is pressed
	for ( int i = 0;i<1000;i++);//bloacking delay to overcome bouncing effect

	keypress(key);//function call to perform operations based on key press

	get_time();//function to get time from rtc

	adc = read_adc(CHANNEL4)/10.23;//function to get values from potentiometer, diving it with 10.23 gives range from 0-100

	//assinging the value to the last 3 elemets of store array which displays speed
	store[13] = (adc/100)+48;
	store[14] = (adc%100/10)+48;
	store[15] = (adc%10)+48;


	//for loop to copy the array elements from time array to first 8elements of store array
	for ( int i = 0; i < 8;i++ )
	{
	    store[i] = time[i];
	}

	if ( option )//if option is 1,then clcd display the vlog options,else clcd displays the time,gear pressed and speed.
	{
	    if ( !pass )//if pass is 0,display the operations to be performed in clcd
	    {
		clcd_putch('*',LINE1(0));
		clcd_print(line[0],LINE1(1));
		clcd_print(line[1],LINE2(0));
	    }
	    else//else, function call is done based on the operation selected and going again to previous mode.
	    {
		if (yes == 1)// if to call the function once.
		{
		    vlog();//function call

		    yes = 0;//reintialise variable
		    if ( value == 3 )//if value is 3,then reinitialise these variables
		    {
			pass = 0;
			scroll = 0;
			option = 0;
			count8 = 150;
		    }
		    if ( value == 4 )//if value is 4,then set time
		    {
			//operations to perform set time.
			unsigned char value = '0';
			//getting the first 2 elements from set element,performing the bitwise operations and converting it to bcd.then assiging the value to value
			value = ( ( ( (set[0]-'0') & 15 ) << 4) | ( (set[1]-'0') & 15 ) );
			write_ds1307(HOUR_ADDR,value);//function call to set the value to hr part

			value = '0';
			//getting the 3rd,4th elements from set element,performing the bitwise operations and converting it to bcd.then assiging the value to value
			value = ( ( ( (set[3]-'0') & 15 ) << 4) | ( (set[4]-'0') & 15 ) );
			write_ds1307(MIN_ADDR,value);//function call to set the value to min part

			value = '0';
			//getting the 7th and 8th elements from set element,performing the bitwise operations and converting it to bcd.then assiging the value to value
			value = ( ( ( (set[6]-'0') & 15 ) << 4) | ( (set[7]-'0') & 15 ) );
			write_ds1307(SEC_ADDR,value);//function call to set the value to sec part

			//reasinging values to variables and input to first line of clcd
			option = 0;
			pass = 0;
			clcd_print("TIME     EVE SPD",LINE1(0));
			scroll = 0;
		    }
		}
	    }
	}
	else
	{
	    clcd_print(store,LINE2(0));//input to 2nd line of clcd
	}
    }
}

int keypress(unsigned char key )
{
    if ( key == MK_SW1)//key one to increment the gear from 1 to R
    {
	count1++;
	if ( count1 == 200 )

	{
	    count1 = 150;
	}
    }
    else if( count1 < 200 && count1 != 0 )
    {
	if ( !option )//assigning values to the store element only if the option value is 0
	{
	    //assigning values to store element based on the values in eve element
	    if ( eve[1] == '5' || eve[1] == 'R' )
	    {
		eve[1] = 'R';
		store[11] = eve[1];
	    }
	    else
	    {
		if ( e )//for first time initialising values to store element as G and 1, amd making e as 0.
		{
		    eve[0] = 'G';
		    eve[1] = '1';
		    store[10] = eve[0];
		    store[11] = eve[1];
		    e = 0;
		}
		else
		{
		    eve[1]++;
		    store[10] = eve[0];
		    store[11] = eve[1];
		}

	    }
	    count1 = 0;

	    //loop to store the array elements to external eeprom
	    for ( int i = 0; i<=15;i++ )
	    {
		write_EEP(add,store[i]);
		add++;
	    }
	}
    }
    else
	count1 = 0;

    if ( key == MK_SW2)//switch2 is to gear down from R to N
    {
	count2++;
	if ( count2 == 200 )//making count value to 150 is count value reaches 200
	{
	    count2 = 150;
	}
    }
    else if ( count2 < 200 && count2 != 0 )
    {
	if ( !option )
	{
	    if ( eve[1] == '1' || eve[1] == 'N' || eve[1] == 'C' )//baesd on the eve element value, assign values to the store element
	    {
		eve[0] = 'G';
		eve[1] = 'N';
		store[10] = eve[0];
		store[11] = eve[1];
		e = 1;
	    }
	    else 
	    {
		if ( eve[1] == 'R' )//if eve value is R storing 5 to eve element, else decrementing value in eve array element 
		{
		    eve[1] = '5';
		    store[11] = eve[1];
		}
		else
		{
		    eve[1]--;
		    store[11] = eve[1];
		}
	    }
	    count2 = 0;
	    //loop to store the values in the store element to external eeprom
	    for ( int i = 0; i<=15;i++ )
	    {
		write_EEP(add,store[i]);
		add++;
	    }
	}
    }
    else
	count2 = 0;


    //key 3 to make collison
    if ( key == MK_SW3)
    {
	count3++;
	if ( count3 == 200 )
	    count3 = 150;
    }
    else if ( count3 < 200 && count3 != 0 )
    {
	//if option value is 0, then uoating the value as C to store element
	if ( !option )
	{
	    eve[0] = ' ';
	    eve[1] = 'C';
	    store[10] = eve[0];
	    store[11] = eve[1];
	    count3 = 0;
	    e = 1;//making e to 1
	    //store values to external eeprom from store array element 
	    for ( int i = 0; i<=15;i++ )
	    {
		write_EEP(add,store[i]);
		add++;
	    }
	}
    }
    else
	count3 = 0;

    //for first time key 8 is used to check password to enter into vlog mode,then for scrolling up
    if ( key == MK_SW8)
    {
	count8++;
    }
    else if ( count8 < 200 && count8 != 0 )//if count is pressed for one time then scroll up is done
    {
	if ( option )
	{
	    if ( !pass )//pass is a variable which descides scrolling done for view log or display log
	    {
		value = 4-scroll;
		line[0] = log[4-scroll];
		if ( scroll == 0 )
		    line[1] = log[0];
		else
		    line[1] = log[4-scroll+1];
		scroll++;
		if ( scroll == 5 )
		    scroll = 0;
	    }
	    else
	    {
		yes = 1;
		inc = 0;
	    }
	}
	else
	{
	    //function call of password function for first time and based on the value of success variable desciding the value of option
	    for ( long int i = 0;i < 200000;i++ );
	    password();
	    for ( long int i = 0;i < 350000;i++ );
	    if ( success )
		option = 1;
	    else
		option = 0;
	}
	count8 = 0;
    }
    else if ( count8 > 200 )//if count pressed for long time,then the respective option is selected
    {
	if ( option )
	{
	    //reinitialise the vriablles
	    pass = 1;
	    yes = 1;

	    //based on the value of the vlue varibale perform the operations
	    if ( value == 1 || value == 2 )
	    {
		for ( int i =0; i<=15 ;i++ )
		{
		    if ( value == 1 )
		    {
			download1[i] = store[i];
			download1[10] = 'G';
			download1[11] = 'L';
		    }
		    if ( value == 2 )
		    {
			clear[i] = store[i];
			clear[10] = 'C';
			clear[11] = 'L';
		    }
		}
	    }
	}
	count8 = 0;
    }
    else
	count8 = 0;

    //key 9 is used to scroll up or return to previous menu
    if ( key == MK_SW9 )
    {
	count9++;
    }
    else if ( count9 < 300 && count9 != 0 )//if key pressed is for one time then scroll down is done
    {
	if ( option )
	{
	    if ( !pass )//pass is varibale that decides scroll down done for view log or display log
	    {
		scroll--;
		if ( scroll == -1 )
		    scroll = 4;
		if ( scroll == 0 )
		{
		    value = scroll;
		}
		else
		{
		    value = 4-scroll+1;
		}
		line[0] = log[value];
		if ( value == 4 )
		    line[1] = log[0];
		else
		    line[1] = log[value+1];

	    }
	    else
	    {
		yes = 1;
		inc = 1;
	    }
	}
	count9=0;
    }
    else if ( count9 > 300 )//if count pressed for long time then,go back to previous option is done
    {
	if ( option )
	{
	    if ( pass == 1 )
	    {
		yes = 0;
		pass = 0;
		line[0] = log[0];
		line[1] = log[1];
		scroll= 0;
		scroll1= 0;
	    }
	}
	count9 = 0;
    }
    else
	count9 = 0;

}
