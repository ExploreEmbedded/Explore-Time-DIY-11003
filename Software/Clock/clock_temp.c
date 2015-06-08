#include "lcd.h"	
#include "util/delay.h"
#include "rtc.h"
//#include "tone.h"

void LCD_CreateCC(uint8_t *pattern, uint8_t);
void LCD_DisplayBigNum(uint8_t num, uint8_t pos);
void LCD_DisplayTime(uint8_t hr,uint8_t min, uint8_t sec);
void LCD_DisplayDate(uint8_t day, uint8_t month, uint8_t year);
uint8_t LCD_DisplayMenu(void);
void setTime(uint8_t hour, uint8_t min, uint8_t sec);
void setDate(uint8_t day, uint8_t month, uint8_t year);


uint8_t dec2bcd(uint8_t);
uint8_t bcd2dec(uint8_t);

void changeTime(void);
//void setAlarm(uint8_t alarm_hour, uint8_t alarm_min, uint8_t alarm_sec);
void setAlarm();
//void checkAlarm(uint8_t alarm_hour, uint8_t alarm_min, uint8_t alarm_sec);
void checkAlarm();

void setDisplayStyle();
void setTimeFormat();


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

uint8_t Display_Style;
uint8_t Time_Format;

unsigned char alarm_sec;
unsigned char alarm_min;
unsigned char alarm_hour; 

void main()
{
    unsigned char sec,min,hour,day,month,year;
	uint8_t option=0;
	
	#define Button PIND
    #define up 5
    #define down 4
	#define select 3
	#define cancel 2
	
	DDRD = 0XF0; //Switches as input, lcd dataline as output
	PORTD = 0XFF; //Internal pull up enabled for switches
	DDRB  |= (1<<buzzer_pin); // buzzer as output
	util_BitSet(buzzer_port,buzzer_pin);
	
//	uint8_t cnt =0;
	LCD_Init(4,2,16);
	LCD_Printf("Explore Time!");
	_delay_ms(100);
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
	
	while(1)
	{
		       
		if(util_IsBitCleared(Button,select))
		 {   
			while(util_IsBitCleared(Button,select));
			 
			do
			{
				option = LCD_DisplayMenu();
				switch(option)
				{
				 case 0: setTime(hour,min,sec);break;
				 case 1: setDate(day,month,year);break;
				// case 2: setAlarm(alarm_hour,alarm_min,alarm_sec); break;
				 case 2: setAlarm(); break;
				 case 3: setDisplayStyle(); break;
				 case 4: setTimeFormat(); break;
				 default: break;
				}
			} while(util_IsBitSet(Button,cancel));
			 
			LCD_Clear(); 	 
		 }
	
		RTC_GetTime(&hour,&min,&sec);      
		RTC_GetDate(&day, &month, &year);// it is only read for updating, not displayed				
//		LCD_DisplayTime(alarm_hour,alarm_min,alarm_sec);
		LCD_DisplayTime(hour, min, sec);
		LCD_DisplayDate(day, month, year);
		
		//checkAlarm(alarm_hour,alarm_min,alarm_sec);
		checkAlarm();
		
		//LCD_DisplayString(settings[2]);
		//LCD_GoToLine(1);
				
	}

}

//Stores the segment pattern in LCD RAM
void LCD_CreateCC(uint8_t *pattern, uint8_t pos)
{
	uint8_t cnt =0;
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
//	float digit0=0, digit1=0, digit2=0;
//	uint8_t d_hour, d_min, d_sec;
	uint8_t digit0=0, digit1=0, digit2=0;
	
		
	if ( Display_Style == 1)	// both date and time are displayed in this style
	{
	/*	d_hour = bcd2dec(hr);
		d_min = bcd2dec(min);
		d_sec = bcd2dec(sec);
		LCD_GoToLine(1);
		LCD_Printf("  %2d : %2d : %2d",d_hour,d_min,d_sec);
	*/	
		digit0 = bcd2dec(hr);
		digit1 = bcd2dec(min);
		digit2 = bcd2dec(sec);
		LCD_GoToLine(1);
		
		if ( Time_Format == 1 )		// 12 hour format
		{
			if (digit0 >12)
			{
				digit0 -= 12;
				LCD_Printf("%2d : %2d : %2d PM",digit0,digit1,digit2);	
			}
			else
			{
				if ( digit0 == 0)	// time is 12 AM
				{
					digit0 = 12;
				}
				LCD_Printf("%2d : %2d : %2d AM",digit0,digit1,digit2);
			}
			
		}	
		else		// 24 hour format
		{
			LCD_Printf("  %2d : %2d : %2d",digit0,digit1,digit2);
		}
	}
	
	else		// only date is displayed, so display big numbers
	{
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
	
}

void LCD_DisplayDate(uint8_t day, uint8_t month, uint8_t year)
{
	unsigned char digit0=0, digit1=0, digit2=0;
		
	if ( Display_Style == 1)
	{
		LCD_GoToLine(2);
		digit0 = bcd2dec(day);
		digit1 = bcd2dec(month);
		digit2 = bcd2dec(year);
		LCD_Printf(" %2d / %2d / %4d",digit0,digit1,digit2);
	}
}

uint8_t LCD_DisplayMenu()
{ 
	int8_t keycount=0;
	LCD_Clear();
	LCD_GoToLine(1);
	LCD_DisplayString("    Settings    ");
	do
	{       
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
		
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
			//make menu index circular. i.e decrement from first option shows the last option on the menu
			keycount += menusize;   
		}			
		
		LCD_GoToLine(2);
		LCD_DisplayString("                "); //clear second line
		//set cursor at the center of of line 2 depending on string size. 
		LCD_SetCursor(2,(14-strlen(settings[keycount]))/2); 
		LCD_Printf("<%s>", settings[keycount]);
		_delay_ms(100);  //make menu visible or else will overwrite at fast speed
					 
      }while(util_IsBitSet(Button,select));
	  
	 do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));
	
//	while(util_IsBitCleared(Button, select));	//wait till button is released. 
	  
	return (keycount); 
}

void setTime(uint8_t hour, uint8_t min, uint8_t sec)
{  
	uint8_t d_hour, d_min, d_sec;
//	uint8_t cnt=0;
	
	d_hour = bcd2dec(hour);
	d_min = bcd2dec(min);
	d_sec = bcd2dec(sec);
	LCD_Clear();
	LCD_GoToLine(1);
	LCD_Printf("Hour  Min  Sec");
	LCD_GoToLine(2);
	LCD_Printf("<%2d>  <%2d> <%2d>" , d_hour, d_min, d_sec);

	do 
	{	
		LCD_SetCursor(2,1);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}

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
	
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));
	
//	while(util_IsBitCleared(Button, select));
		  
		//update min  
	
	do 
	{
		LCD_SetCursor(2,7);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}

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
		
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));
	
//	while(util_IsBitCleared(Button, select));
				
	do 
	{

		LCD_SetCursor(2,12);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
		
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
	
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));	
	
//	while(util_IsBitCleared(Button, select));
		
	hour = dec2bcd(d_hour);
	min = dec2bcd(d_min);
	sec = dec2bcd(d_sec);
	RTC_SetTime(hour, min, sec); 
	LCD_Clear();
	LCD_DisplayString("Time Updated");
	_delay_ms(200);
	  
}

void setDate(uint8_t day, uint8_t month, uint8_t year)
{  
	uint8_t d_day, d_month, d_year;
//	uint8_t cnt=0;
	
	d_day = bcd2dec(day);
	d_month = bcd2dec(month);
	d_year= bcd2dec(year);
	LCD_Clear();
	LCD_GoToLine(1);
	LCD_Printf(" DD    MM   YY");
	LCD_GoToLine(2);
	LCD_Printf("<%2d>  <%2d> <%2d>" , d_day, d_month, d_year);

	do 
	{	
		LCD_SetCursor(2,1);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}

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
		  
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));
	
//	while(util_IsBitCleared(Button, select));
	  
	//update month
	do 
	{
		LCD_SetCursor(2,7);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
		
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
		
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));
	
//	while(util_IsBitCleared(Button, select));
		
	//set year	
	do 
	{
		LCD_SetCursor(2,12);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}

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
	
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));
	
//	while(util_IsBitCleared(Button, select));
	
   day = dec2bcd(d_day);
   month = dec2bcd(d_month);
   year = dec2bcd(d_year);
   RTC_SetDate(day, month, year); 
   LCD_Clear();
   LCD_DisplayString("Date Updated");
   _delay_ms(200);
	  
}

//void setAlarm(uint8_t alarm_hour, uint8_t alarm_min, uint8_t alarm_sec)
void setAlarm()
{
 
	uint8_t d_hour, d_min, d_sec;
//	uint8_t cnt=0;
	
	d_hour = bcd2dec(alarm_hour);
	d_min = bcd2dec(alarm_min);
	d_sec = bcd2dec(alarm_sec);
	LCD_Clear();
	LCD_GoToLine(1);
	LCD_Printf("Hour  Min  Sec");
	LCD_GoToLine(2);
	LCD_Printf("<%2d>  <%2d> <%2d>" , d_hour, d_min, d_sec);

	do 
	{	
		LCD_SetCursor(2,1);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}

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
	
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));
	
//	while(util_IsBitCleared(Button, select));
		  
		//update min  
	
	do 
	{
		LCD_SetCursor(2,7);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}

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
		
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));
	
//	while(util_IsBitCleared(Button, select));
				
	do 
	{

		LCD_SetCursor(2,12);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
		
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
	
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));	
	
//	while(util_IsBitCleared(Button, select));
		
	alarm_hour = dec2bcd(d_hour);
	alarm_min = dec2bcd(d_min);
	alarm_sec = dec2bcd(d_sec);
	LCD_Clear();
	LCD_DisplayString("Alarm Set");
	_delay_ms(200);
   
}

//void checkAlarm(uint8_t alarm_hour, uint8_t alarm_min, uint8_t alarm_sec)
void checkAlarm()
{
	uint8_t b_hour, b_min, b_sec;
//	uint8_t cnt=0;

	RTC_GetTime(&b_hour,&b_min,&b_sec);
	
	if ( (alarm_hour == b_hour) & ( alarm_min == b_min) & ( alarm_sec == b_sec) ) 
	{
		do
		{
			LCD_Clear();
			LCD_GoToLine(1);
			LCD_Printf("Press Cancel to stop alarm");
		}while(util_IsBitSet(Button, cancel));
		
		util_BitClear(buzzer_port,buzzer_pin);
	}
		
		
}


void setDisplayStyle()
{
	
	LCD_Clear();
	
/*	if(Display_Style == 1)
	{
		LCD_Printf("Time and Date");
	}
	else
	{
		LCD_Printf("Only Time      ");
	}
	
*/	do 
	{
		LCD_GoToLine(1);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
		
		if(util_IsBitCleared(Button, up))
		{
			Display_Style++;
		//	LCD_Printf("Time and Date");
			_delay_ms(200);
		}
		if(util_IsBitCleared(Button, down))
		{
			Display_Style--;
		//	LCD_Printf("Only Time ");
			_delay_ms(200);
		}
			
		if(Display_Style>=2)
		{
			Display_Style=0;
		}		

		if(Display_Style == 1)
		{
			LCD_Printf("Time and Date");
		}
		else
		{
			LCD_Printf("Only Time     ");
		}		
	} while (util_IsBitSet(Button, select));
	
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));	
	
}


void setTimeFormat()
{
	LCD_Clear();
	
	do 
	{
		LCD_GoToLine(1);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
		
		if(util_IsBitCleared(Button, up))
		{
			Time_Format++;		
			_delay_ms(200);
		}
		if(util_IsBitCleared(Button, down))
		{
			Time_Format--;
		//	LCD_Printf("Only Time ");
			_delay_ms(200);
		}
			
		if(Time_Format>=2)
		{
			Time_Format=0;
		}		

		if( Time_Format == 1 )
		{
			LCD_Printf("12 Hour Format");
		}
		else
		{
			LCD_Printf("24 Hour Format");
		}		
	} while (util_IsBitSet(Button, select));
	
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));
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