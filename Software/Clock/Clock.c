#include "lcd.h"	
#include "util/delay.h"
#include "rtc.h"
//#include "tone.h"

void LCD_CreateCC(uint8_t *pattern, uint8_t);
void LCD_DisplayBigNum(uint8_t num, uint8_t pos);
void LCD_DisplayTime(uint8_t hr,uint8_t min, uint8_t sec);
uint8_t LCD_DisplayMenu(void);
void setTime(uint8_t hour, uint8_t min, uint8_t sec);
void setDate(uint8_t day, uint8_t month, uint8_t year);
void setAlarm();
uint8_t dec2bcd(uint8_t);
uint8_t bcd2dec(uint8_t);

uint8_t cnt =0;


//Segment Patterns for displaying large fonts
uint8_t LT[8] = 
{
  0b00111,
  0b01111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
uint8_t UB[8] =
{
  0b11111,
  0b11111,
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
uint8_t RT[8] =
{
  0b11100,
  0b11110,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111
};
uint8_t LL[8] =
{
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b01111,
  0b00111
};
uint8_t LB[8] =
{
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111
};
uint8_t LR[8] =
{
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11110,
  0b11100
};
uint8_t UMB[8] =
{
  0b11111,
  0b11111,
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111
};
uint8_t LMB[8] =
{
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111
};

char settings[5][16]={"Time","Date","Alarm","Display Style","Time Format"};
//only first three options supported currently. 
#define menusize 5
#define buzzer_pin 3
#define buzzer_port PORTB
uint8_t upcount=0, option=0;



void main()
{
    unsigned char sec,min,hour,day,month,year;
	#define Button PIND
    #define up 5
    #define down 4
	#define select 3
	#define cancel 2
	
	DDRD = 0XF0; //Switches as input, lcd dataline as output
	PORTD = 0XFF; //Internal pull up enabled for switches
	DDRB  |= (1<<buzzer_pin); // buzzer as output
	util_BitSet(buzzer_port,buzzer_pin);
	
	uint8_t cnt =0;
	LCD_Init(4,2,16);
	LCD_Printf("Explore Time!");
	_delay_ms(1000);
    RTC_Init();
	
	//segments for creating large font stored in RAM.
	
	LCD_CreateCC(LT,0);
	LCD_CreateCC(UB,1);
	LCD_CreateCC(RT,2);
	LCD_CreateCC(LL,3);
	LCD_CreateCC(LB,4);
	LCD_CreateCC(LR,5);
	LCD_CreateCC(UMB,6);
	LCD_CreateCC(LMB,7);


	/*Set the time and Date only once */
	//RTC_SetTime(0x12,0x57,0x20);  //  10:40:20 am
	//RTC_SetDate(0x03,0x03,0x15);  //  14th Nov 2012
	
	while(1){
		       
			  	if(util_IsBitCleared(Button,select))
				 {   
					 while(util_IsBitCleared(Button,select));
					 option = LCD_DisplayMenu();
					 switch(option)
					 {
						 case 0: setTime(hour,min,sec);break;
						 case 1: setDate(day,month,year);break;
						 case 2: setAlarm(); break;
						 default: break;
					 }
					 
				 	LCD_Clear(); 	 
				 }
			
			    RTC_GetTime(&hour,&min,&sec);      
                RTC_GetDate(&day, &month, &year);// it is only read for updating not displayed				
				LCD_DisplayTime(hour,min,sec);
				
				//LCD_DisplayString(settings[2]);
				//LCD_GoToLine(1);
				
			}

}

//Stores the segment pattern in LCD RAM
void LCD_CreateCC(uint8_t *pattern, uint8_t pos)
{
	 LCD_CmdWrite(0x40+(pos*8));
		for(cnt=0; cnt<8; cnt++)
		{
			lcd_DataWrite(*(pattern+cnt));
		}
}

void LCD_DisplayBigNum(uint8_t num, uint8_t pos)
{
	switch(num)
	{	
		case 0:
		LCD_SetCursor(1,pos);
		lcd_DataWrite(0);
		lcd_DataWrite(1);
		lcd_DataWrite(2);;
		LCD_SetCursor(2,pos);
		lcd_DataWrite(3);
		lcd_DataWrite(4);
		lcd_DataWrite(5);
		break;
		
		case 1:
		LCD_SetCursor(1,pos);
		lcd_DataWrite(1);
		lcd_DataWrite(2);
		lcd_DataWrite(' ');
		LCD_SetCursor(2,pos);
		lcd_DataWrite(' ');
		lcd_DataWrite(255);
		lcd_DataWrite(' ');
		break;
		
		case 2:
		LCD_SetCursor(1,pos);
		lcd_DataWrite(6);
		lcd_DataWrite(6);
		lcd_DataWrite(2);;
		LCD_SetCursor(2,pos);
		lcd_DataWrite(3);
		lcd_DataWrite(7);
		lcd_DataWrite(7);
		break;
		
		case 3:
		LCD_SetCursor(1,pos);
		lcd_DataWrite(6);
		lcd_DataWrite(6);
		lcd_DataWrite(2);;
		LCD_SetCursor(2,pos);
		lcd_DataWrite(7);
		lcd_DataWrite(7);
		lcd_DataWrite(5);
		break;
		
		case 4:
		LCD_SetCursor(1,pos);
		lcd_DataWrite(3);
		lcd_DataWrite(4);
		lcd_DataWrite(2);;
		LCD_SetCursor(2,pos);
		lcd_DataWrite(' ');
		lcd_DataWrite(' ');
		lcd_DataWrite(255);
		break;
		
		case 5:
		LCD_SetCursor(1,pos);
		lcd_DataWrite(255);
		lcd_DataWrite(6);
		lcd_DataWrite(6);;
		LCD_SetCursor(2,pos);
		lcd_DataWrite(7);
		lcd_DataWrite(7);
		lcd_DataWrite(5);
		break;
		
		case 6:
		LCD_SetCursor(1,pos);
		lcd_DataWrite(8);
		lcd_DataWrite(6);
		lcd_DataWrite(6);;
		LCD_SetCursor(2,pos);
		lcd_DataWrite(3);
		lcd_DataWrite(7);
		lcd_DataWrite(5);
		break;
		
		case 7:
		LCD_SetCursor(1,pos);
		lcd_DataWrite(1);
		lcd_DataWrite(1);
		lcd_DataWrite(2);;
		LCD_SetCursor(2,pos);
		lcd_DataWrite(' ');
		lcd_DataWrite(8);
		lcd_DataWrite(' ');
		break;
		
		case 8:
		  LCD_SetCursor(1,pos); 
		  lcd_DataWrite(0);  
		  lcd_DataWrite(6); 
		  lcd_DataWrite(2);;
		  LCD_SetCursor(2,pos);
		  lcd_DataWrite(3);  
		  lcd_DataWrite(7);  
		  lcd_DataWrite(5);
		  break; 	  
		  case 9:
		  LCD_SetCursor(1,pos);
		  lcd_DataWrite(8);
		  lcd_DataWrite(6);
		  lcd_DataWrite(2);
		  LCD_SetCursor(2,pos);
		  lcd_DataWrite(' ');
		  lcd_DataWrite(' ');
		  lcd_DataWrite(255);	
		  break;
		default:break; 		
	}	
}

void LCD_DisplayTime(unsigned char hr,unsigned char min, unsigned char sec)
{
	float digit0=0, digit1=0;
	
	digit1 = hr/0x10;
	digit0= hr%0x10;
	LCD_DisplayBigNum(digit1,0);  //display hour
	LCD_DisplayBigNum(digit0,3);
	
	// set colon
	LCD_SetCursor(1,6);
	lcd_DataWrite(0xA5);
	LCD_SetCursor(2,6);
	lcd_DataWrite(0xA5);
	  
	//display min  
	digit1 = min/0x10;
	digit0= min%0x10;
	LCD_DisplayBigNum(digit1,7);  
	LCD_DisplayBigNum(digit0,10);
	
	//display sec
	// LCD_SetCursor(1,14);
	//LCD_DisplayString("pm");
	 LCD_SetCursor(2,14);
	 LCD_Printf("%2x",sec);	
}


uint8_t LCD_DisplayMenu()
{ 
	int8_t keycount=0;
	LCD_Clear();
	LCD_GoToLine(1);
	LCD_DisplayString("    Settings    ");
	do{       
			if(util_IsBitCleared(Button, up))
			{
				keycount++;
				_delay_ms(100);
			}
			
			if(util_IsBitCleared(Button, down))
			{
				keycount--;
				_delay_ms(100);
			}
		
			if((keycount>=menusize)||(keycount <= -menusize))
			{
				keycount=0;
			}
			
			if(keycount<0)
			{
				keycount += menusize;   //make menu index circular. i.e decrement from first option shows the last option on the menu
			}			
			
     		LCD_GoToLine(2);
			LCD_DisplayString("                "); //clear second line
			LCD_SetCursor(2,(14-strlen(settings[keycount]))/2); //set cursor at the center of of line 2 depending on string size. 
			LCD_Printf("<%s>", settings[keycount]);
		    _delay_ms(100);  //make menu visible or else will overwrite at fast speed
			    		 
      }while(util_IsBitSet(Button,select));
	  while(util_IsBitCleared(Button,select)); //wait till button is released. 
	  return keycount;
}

void setTime(uint8_t hour, uint8_t min, uint8_t sec)
{  
	uint8_t d_hour, d_min, d_sec;
	uint8_t cnt=0;
	
	d_hour = bcd2dec(hour);
	d_min = bcd2dec(min);
	d_sec = bcd2dec(sec);
	LCD_Clear();
	LCD_GoToLine(1);
	LCD_Printf("Hour  Min  sec");
	LCD_GoToLine(2);
	LCD_Printf("<%2d>  <%2d> <%2d>" , d_hour, d_min, d_sec);

			
		do {	
			    LCD_SetCursor(2,1);
				_delay_ms(100);
				if(util_IsBitCleared(Button, up))
				{
					d_hour++;
					_delay_ms(200);
				}
				if(util_IsBitCleared(Button, down))
				{
					d_hour--;
					_delay_ms(200);
				}
				
				if(d_hour>=24)
				{
					d_hour=0;
				}
				
				LCD_Printf("%2d",d_hour);
				
		  } while (util_IsBitSet(Button, select));
		  while(util_IsBitCleared(Button,select));
		  
		  
		//update min  
		
		do {
			LCD_SetCursor(2,7);
			_delay_ms(100);
			if(util_IsBitCleared(Button, up))
			{
				d_min++;
				_delay_ms(200);
			}
			if(util_IsBitCleared(Button, down))
			{
				d_min--;
				_delay_ms(200);
			}
			  	
			if(d_min>=60)
			{
				d_min=0;
			}
			  	
			LCD_Printf("%2d",d_min);
			  	
			  	
		} while (util_IsBitSet(Button, select));
		while(util_IsBitCleared(Button,select));
		
		
		
		do {
			LCD_SetCursor(2,12);
			_delay_ms(100);
			if(util_IsBitCleared(Button, up))
			{
				d_sec++;
				_delay_ms(200);
			}
			if(util_IsBitCleared(Button, down))
			{
				d_sec--;
				_delay_ms(200);
			}
				
			if(d_sec>=60)
			{
				d_sec=0;
			}
				
			LCD_Printf("%2d",d_sec);
				
				
		} while (util_IsBitSet(Button, select));
		while(util_IsBitCleared(Button,select));
	   hour = dec2bcd(d_hour);
	   min = dec2bcd(d_min);
	   sec = dec2bcd(d_sec);
	   RTC_SetTime(hour, min, sec); 
	   LCD_Clear();
	   LCD_DisplayString("Time Updated");
	   _delay_ms(2000);
	  
}

void setDate(uint8_t day, uint8_t month, uint8_t year)
{  
	uint8_t d_day, d_month, d_year;
	uint8_t cnt=0;
	
	d_day = bcd2dec(day);
	d_month = bcd2dec(month);
	d_year= bcd2dec(year);
	LCD_Clear();
	LCD_GoToLine(1);
	LCD_Printf(" DD    MM   YY");
	LCD_GoToLine(2);
	LCD_Printf("<%2d>  <%2d> <%2d>" , d_day, d_month, d_year);

			
		do {	
			    LCD_SetCursor(2,1);
				_delay_ms(100);
				if(util_IsBitCleared(Button, up))
				{
					d_day++;
					_delay_ms(200);
				}
				if(util_IsBitCleared(Button, down))
				{
					d_day--;
					_delay_ms(200);
				}
				
				if(d_day>31)
				{
					d_day=0;
				}
				
				LCD_Printf("%2d",d_day);
				
		  } while (util_IsBitSet(Button, select));
		  while(util_IsBitCleared(Button,select));
		  
		  
		//update month
		
		do {
			LCD_SetCursor(2,7);
			_delay_ms(100);
			if(util_IsBitCleared(Button, up))
			{
				d_month++;
				_delay_ms(200);
			}
			if(util_IsBitCleared(Button, down))
			{
				d_month--;
				_delay_ms(200);
			}
			  	
			if(d_month>12)
			{
				d_month=0;
			}
			  	
			LCD_Printf("%2d",d_month);
			  	
			  	
		} while (util_IsBitSet(Button, select));
		while(util_IsBitCleared(Button,select));
		
		//set year
		
		do {
			LCD_SetCursor(2,12);
			_delay_ms(100);
			if(util_IsBitCleared(Button, up))
			{
				d_year++;
				_delay_ms(200);
			}
			if(util_IsBitCleared(Button, down))
			{
				d_year--;
				_delay_ms(200);
			}
				
			if(d_year>=99)
			{
				d_year=0;
			}
				
			LCD_Printf("%2d",d_year);
				
				
		} while (util_IsBitSet(Button, select));
		while(util_IsBitCleared(Button,select));
	   day = dec2bcd(d_day);
	   month = dec2bcd(d_month);
	   year = dec2bcd(d_year);
	   RTC_SetDate(day, month, year); 
	   LCD_Clear();
	   LCD_DisplayString("Date Updated");
	   _delay_ms(2000);
	  
}

void setAlarm()
{
 
  LCD_Clear();
  LCD_DisplayString("Turn Alarm ON");
  do{
       util_BitSet(buzzer_port,buzzer_pin);
  }while(util_IsBitSet(Button, select));
  while(util_IsBitCleared(Button,select));
  util_BitClear(buzzer_port,buzzer_pin);
   
}

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t dec2bcd(uint8_t num)
{
	return ((num/10 * 16) + (num % 10));
}
// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t bcd2dec(uint8_t num)
{
	return ((num/16 * 10) + (num % 16));
}