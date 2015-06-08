/*
 *  Author: xplore_labz
 *  note: Refer lcd.c(lcd_4_bit.c/lcd_8_bit.c) file for Pin connections
 */ 

/* io.h contains the defnition of all ports and SFRs 
   delay.h contains the in built delay routines(us and ms routines)*/
#include <avr\io.h> 
#include <util\delay.h> 
#include "lcd.h"  // User defined library containing LCD routines.

void LCD_CreateCC(uint8_t *pattern, uint8_t);
void LCD_DisplayBigNum(uint8_t num, uint8_t pos);
uint8_t LCD_DisplayMenu(void);
void LCD_DisplayDistance(uint16_t distanc);
unsigned int measure_Distance();
void set_DistanceUnit();
void set_DisplayStyle();

#define Trigger_pin			2
#define Echo_pin			3
#define UltraSendHighSignal()  (PORTC |= (1 << Trigger_pin))
#define UltraSendLowSignal()  (PORTC &= ~(1 << Trigger_pin))

#define UltraGetEchoSignal()  ((PINC & (1 << Echo_pin))!=0)

#define menusize 2
#define nevigatn_buton_cnt 2

char settings[5][16]={" Distance Unit"," Display Style"};

uint8_t Distance_Unit = 1;
uint8_t Display_Style;


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



/* start the main program */
unsigned int main() 
{
    uint8_t dist;
	uint8_t option=0;
	
	#define Button	PIND
    #define up 		5
    #define down 	4
	#define select 	3
	#define cancel 	2
	 
	DDRD = 0XF0; //Switches as input, lcd dataline as output
	PORTD = 0XFF; //Internal pull up enabled for switches
	DDRC |= (1 << Trigger_pin); // Trigger_pin as output

  /* Initilize the lcd before displaying any thing on the lcd */
    LCD_Init(4,2,16);
	LCD_Printf("    Explore \n    Distance");
	_delay_ms(2000);
	LCD_Clear();
	
	//segments for creating large font stored in RAM.
	
	LCD_CreateCC(LT,0);
	LCD_CreateCC(UB,1);
	LCD_CreateCC(RT,2);
	LCD_CreateCC(LL,3);
	LCD_CreateCC(LB,4);
	LCD_CreateCC(LR,5);
	LCD_CreateCC(UMB,6);
	LCD_CreateCC(LMB,7);
	
	while(1)
    {
        // check if select button is pressed
		if(util_IsBitCleared(Button,select))
		 {  
			// wait till select button is released
			while(util_IsBitCleared(Button,select));
			 
			// show menu till  cancle button is pressed		
			do
			{
				option = LCD_DisplayMenu();
				switch(option)
				{
				 case 0: set_DistanceUnit(); break;
				 case 1: set_DisplayStyle(); break;
				 default: break;
				}
			} while(util_IsBitSet(Button,cancel));
			do
			{
								
			} while(util_IsBitSet(Button,cancel));	
			
			LCD_Clear(); 	 
		 }
		 
		 // measure the distance and display it on LCD
		 dist = measure_Distance();	
		 LCD_DisplayDistance(dist);
		 _delay_ms(150);
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
		lcd_DataWrite(' ');
		lcd_DataWrite(' ');
		lcd_DataWrite(255);
		LCD_SetCursor(2,pos);
		lcd_DataWrite(' ');
		lcd_DataWrite(' ');
		lcd_DataWrite(255);
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
		   return (-1); 
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
			//make menu index circular. i.e decrement from first 
			//option shows the last option on the menu
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
	
	return (keycount); 
}



void LCD_DisplayDistance(uint16_t dist)
{
	uint8_t digit=0;
	
	LCD_Clear();

	if (Display_Style == 1)	// display small numbers
	{
		LCD_Printf (" Dist = %3d.%1d ",(dist/10), (dist%10));
	}
	else		// display big numbers
	{	
		digit = dist / 1000;		// 10's place
		LCD_DisplayBigNum(digit,0);  //display distance
		
		digit = (dist / 100) % 10;	// units place		
		LCD_DisplayBigNum(digit,3);

		// display fraction
		digit = (dist / 10) % 10;
		LCD_DisplayBigNum(digit,6);  
		
		// set decimal point
		LCD_SetCursor(2,9);
		lcd_DataWrite(0xA5);
		
		digit = dist % 10;
		LCD_DisplayBigNum(digit,10);		
	}	
	
	//display unit 
	LCD_SetCursor(1,14);	
	if (Distance_Unit == 1)
	{
		LCD_Printf("CM");	
	}
	else
	{
		LCD_Printf("In");
	}

}

unsigned int measure_Distance()   
{

	float distance,cnt;
    TCNT1H=0x00;              
	TCNT1L=0x00;
	TCCR1A=0x00;;
  
    UltraSendHighSignal();	
	_delay_us(5);
	UltraSendLowSignal();
   
    while(UltraGetEchoSignal() == 0);

   	TCCR1B=0x05;     // Timer mode with 1024 prescler
    
	while(UltraGetEchoSignal() == 1);
      	TCCR1B=0x00;
      
	  cnt = (TCNT1H<<8) + TCNT1L;

	 /* 
	    Distance = (Time * speed of sound)/2

	    time = timer count * tick_time = (cnt * 1024(prescalar))/(8 Mhz)
		speed of sound = 340 m/s= 340 * 100 cm/s

		distance = (((cnt * 1024)/8 Mhz) * 34000)/2
		         = (cnt * 1088000)/1 Mhz
				 =cnt * 2.176 cm 
				 = cnt * 0.8567 in
	 */
	  
	if ( Distance_Unit == 1)
	{
		distance = (cnt * 21.76);
	}  
	else
	{
		distance = (cnt * 8.567);
	}  
	
	return((unsigned int) distance);                 
 }

  
  void set_DistanceUnit()
{
	LCD_Clear();
	LCD_GoToLine(1);
	LCD_Printf("  Distance Unit");
	LCD_GoToLine(2);

	do 
	{	
		LCD_SetCursor(2,1);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
			return (-1);  
		}
		if(util_IsBitCleared(Button, up))
		{
			Distance_Unit++;
			_delay_ms(200);			
		}
		if(util_IsBitCleared(Button, down))
		{
			Distance_Unit--;
			_delay_ms(200);		
		}
		
		if((Distance_Unit >= 2)||(Distance_Unit <= -2))
		{
			Distance_Unit = 0;
		}		
		if(Distance_Unit < 0)
		{
			//make menu index circular. i.e decrement from first 
			//option shows the last option on the menu
			Distance_Unit += nevigatn_buton_cnt;   
		}	

		if(Distance_Unit == 1)
		{
			LCD_Printf("      <CM>     ");
		}
		else
		{
			LCD_Printf("    <Inches>");
		}
			
	} while (util_IsBitSet(Button, select));
	
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));	
	
	LCD_GoToLine(1);
	LCD_Printf("  Distance Unit \n    Updated     ");
	_delay_ms(1000);
	
}


 void set_DisplayStyle()
{
	LCD_Clear();
	LCD_GoToLine(1);
	LCD_Printf("  Display Style ");
	LCD_GoToLine(2);

	do 
	{	
		LCD_SetCursor(2,1);
		_delay_ms(100);
		
		if (util_IsBitCleared(Button, cancel))
		{
			return (-1);  
		}
		if(util_IsBitCleared(Button, up))
		{
			Display_Style++;
			_delay_ms(200);			
		}
		if(util_IsBitCleared(Button, down))
		{
			Display_Style--;
			_delay_ms(200);		
		}
		
		if((Display_Style >= 2)||(Display_Style <= -2))
		{
			Display_Style = 0;
		}		
		if(Display_Style < 0)
		{
			//make menu index circular. i.e decrement from first 
			//option shows the last option on the menu
			Display_Style += 2;   
		}	

		if(Display_Style == 1)
		{
			LCD_Printf("   < Big no.>   ");
		}
		else
		{
			LCD_Printf("  < Small no.> ");
		}
			
	} while (util_IsBitSet(Button, select));
	
	do 
	{
		if (util_IsBitCleared(Button, cancel))
		{
		   return (-1); ; 
		}
	}while(util_IsBitSet(Button, select));	
	
	LCD_GoToLine(1);
	LCD_Printf("  Display Style \n    Updated     ");
	_delay_ms(1000);
	
}


