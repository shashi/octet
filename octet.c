#include <msp430F1611.h>
/*------]]]]-------------------------------------------
                        MSP430G2553
                      -----------------
            3.3 V -->|Vcc          Gnd |<-- Ground       
                     |                 |
          Unused  -->|P2.0         P2.7|--> Unused
                     |                 |
               Rx -->|P2.1         P2.6|--> Unused
                     |                 |
               Tx <--|P2.2         TST |--> Open
                     |                 |
          BLANK   <--|P2.3         RST |<-- Pulled high
                     |                 |
          GSLK    <--|P2.4         P2.7|--> SIN
                     |                 |
          SCLK    <--|P2.5         P2.6|--> XLAT
                     |                 |
          sin_595 <--|P2.0         P2.5|<-- Button6         
                     |                 |
         sclk_595 <--|P2.1         P2.4|<-- Button5
                     |                 |
        latch_595 <--|P2.2         P2.3|<-- Button4           
                      -----------------
  -------------------------------------------------*/


#define SIN        BIT7
#define GSCLK 	   BIT4
#define XLAT       BIT6
#define BLANK 	   BIT3
#define SCLK  	   BIT5
#define TX 	  	   BIT2	

#define SIN595    BIT0
#define SCLK595   BIT1
#define LTCH595   BIT2

#define N_col    16
#define N_row 	 8
#define N_layer  8
#define N_level_bits 2   //0,25%,50%,100%
#define arr_collsize N_col*N_level_bits/8
//flag defines
#define COL_written   0x01
#define MAT_written	  0x02
#define RX_occured    0x04


unsigned char strobe=0;
char stat_read=0;
char rx_x=0,rx_y=0;
char button_stat=0x00;//button status variable reset on reading 'K'
/*
void conv_ascii(int val,char *asc_val)//function to convert in to a set of ascii char to be read by >> in Cpp program(max val=1024)
{
 int i=0;
 for(i=0;val>=1000;i++)
       val-=1000;
 asc_val[0]=(char)(i+48);
 for(i=0;val>=100;i++)
       val-=100;
 asc_val[1]=(char)(i+48);
 for(i=0;val>=10;i++)
       val-=10;
 asc_val[2]=(char)(i+48); 
 for(i=0;val>=1;i++)
       val-=1;
 asc_val[3]=(char)(i+48);
}
void serial_println_8(char val)
{
 while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
  UCA0TXBUF = val;
 while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
  UCA0TXBUF = '\r';
 while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
  UCA0TXBUF = '\n';
}
*/

#define CLOCK_PERIOD     4      //time period for a PWM pulse
int PWMCount = 0; // Counter for GSCLK pulse
char rx_temp;
unsigned char disp_arr[N_layer][2][N_row];
unsigned char flag=0;
void ConfigureTimer(void);
void InitializeClocks(void);
void SendData(char col_no);

void main(void)

{
    WDTCTL = WDTPW + WDTHOLD;
    char i=0,j=0,rx_i=0,rx_j=0,layer_no,row_no=0;
    rx_temp=0;
    flag=0;	 

	for(j=0;j<N_layer;++j)
		for(i=0;i<arr_collsize;++i)
				   {disp_arr[j][i][0]=0xFF;
					disp_arr[j][i][1]=0xAA;
					disp_arr[j][i][2]=0x55;
					disp_arr[j][i][3]=0x00;
					disp_arr[j][i][4]=0xFF;
					disp_arr[j][i][5]=0xAA;
					disp_arr[j][i][6]=0x55;
					disp_arr[j][i][7]=0x00;
					
}

	P1DIR=0xFF;
	P1OUT=0xFF;
	//TLC initialization
	P2DIR = 0;
    P2DIR |= (SIN + SCLK + XLAT + BLANK + GSCLK);
    P2OUT = (SIN + SCLK + XLAT + BLANK + GSCLK);
    P2OUT=0;


    //Shift 595 initialization
    P2DIR |= (SIN595 + SCLK595 + LTCH595);
    P2OUT = 0;
    //UART

	P3DIR= 0X40;
    P3SEL |= 0xC0;                            // P3.6,7 = USART1 TXD/RXD
	ME2 |= UTXE1 + URXE1;                     // Enable USART1 TXD/RXD
	UCTL1 |= CHAR;                            // 8-bit character
	UTCTL1 |= SSEL0;                          // UCLK = smCLK
	UBR01 = 0x03;                             // 32k/9600 - 3.41
	UBR11 = 0x00;                             //
	UMCTL1 = 0x4A;                            // Modulation
	UCTL1 &= ~SWRST;                          // Initialize USART state machine
	IE2 |= URXIE1;                            // Enable USART1 RX interrupt

     //SendData(0); // Clear all output
    InitializeClocks(); // Setup clock
    ConfigureTimer(); // Setup Timer to generate GSCLK
//    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
    _bis_SR_register(GIE);
	while (1)
    {//for(i=0;i<wave_delay;++i)
    P1OUT=~(1<<layer_no);
    for(layer_no=0;layer_no<N_layer;layer_no++)
     {
    	SendData(layer_no); // Send current Step data to TLC5940
        _bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, enable interrupts
        P1OUT=~(1<<layer_no);
     }
//PUT MODS HERE
    /*
    if(strobe=='D')
        	{
        	rx_x=4;
        	rx_y=32;
        	serial_println_8('Z');
        	strobe=0;}
    if(strobe=='S')
    	{
    	rx_x=0;
    	rx_y=0;
    	serial_println_8('K');
    	strobe=0;
    	}
    }

*/
//    _delay_cycles(100000);
/*
    for(j=0;j<N_layer;++j)
      for(i=0;i<arr_collsize;++i)
	   {rx_temp=disp_arr[j][i][0];
		disp_arr[j][i][0]=disp_arr[j][i][1];
	    disp_arr[j][i][1]=disp_arr[j][i][2];
	    disp_arr[j][i][2]=disp_arr[j][i][3];
	    disp_arr[j][i][3]=disp_arr[j][i][4];
	    disp_arr[j][i][4]=disp_arr[j][i][5];
	    disp_arr[j][i][5]=disp_arr[j][i][6];
	    disp_arr[j][i][6]=disp_arr[j][i][7];
	    disp_arr[j][i][7]=rx_temp;
	    }*/
		}
}


void SendData(char layer)
{
    char i=0,temp,col_no=0;
    P2OUT &= ~(SCLK + XLAT + BLANK);
    for(col_no=0;col_no<8;++col_no)
    for (i = 0; i < N_row; i++)
    {   // Send data for each channel, CH15 first
        temp=((disp_arr[layer][col_no>>2][i])<<((col_no&0x03)<<1));
    	P2OUT&=~(SIN);
  		P2OUT|=SCLK;		//11
		P2OUT&=~(SCLK);
		P2OUT|=SCLK;		//10
		P2OUT&=~(SCLK);
		P2OUT|=SCLK;		//9
	    P2OUT&=~(SCLK);
		P2OUT|=SCLK;		//8
		P2OUT&=~(SCLK);
		P2OUT|=SCLK;		//7
		P2OUT&=~(SCLK);
		P2OUT|=SCLK;		//6
  		P2OUT&=~(SCLK);
		P2OUT|=SCLK;		//5
  		P2OUT&=~(SCLK);
	    P2OUT|=SCLK;		//4
	    P2OUT&=~(SCLK);//8th bit onwards
  		P2OUT|=SCLK;		//3
  		P2OUT&=~(SCLK);		// Send duty bits, 12-bit, MSB first
  		P2OUT|=SCLK;		//2
  		P2OUT&=~(SCLK);
  		P2OUT|=SIN&temp;
		P2OUT|=SCLK;		//1
		P2OUT&=~(SCLK);
//		P2OUT|=SIN;			//test high
		P2OUT&=~(SIN);
  		P2OUT|=SIN&(temp<<1);
//		P2OUT|=SIN;			//test high
	    P2OUT|=SCLK;		//0
	    P2OUT&=~(SCLK);
        }
flag|=COL_written;
}

void InitializeClocks(void)
{
  ;//BCSCTL1|= RSEL0+RSEL1+RSEL2;
  //DCOCTL|=  DCO0+DCO1+DCO2;
}

void ConfigureTimer(void)
{
  TACTL = TASSEL_2 | MC_0 | TACLR;  // TACLK = SMCLK, Up mode, reset count
  TACCR0 = 80;                        // Timer Frequency
 // TACCTL0 = CCIE;                    // Trigger Interrupt whenever Timer reach TACCR0
  TACTL = TASSEL_2 | MC_1+TAIE;          // TACLK = SMCLK, Up mode
}


#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A1(void)
{
    // Pulse GSCLK
    P2OUT |= GSCLK;
    P2OUT &= ~GSCLK;
    // pulse BLANK to restart PWM cycle when GSCLK pulse reach max count
    if (++PWMCount >= CLOCK_PERIOD)
    {
        if(flag&COL_written)
      	{P2OUT |= BLANK;
	     P2OUT |= XLAT;
	     P2OUT &= ~(XLAT);
	     P2OUT &= ~(BLANK);
	     P2OUT |= SCLK;
         P2OUT &= ~SCLK;
         flag&=~(COL_written);
       	 _bic_SR_register_on_exit(LPM0_bits);}  
        else
        {P2OUT |= BLANK;
         P2OUT &= ~(BLANK);}
         PWMCount = 0;
    }
    TA0CTL &= ~TAIFG;
}
/*
void send_uart(char command)
{
 while (!(IFG2 & UTXIFG1));                // USART1 TX buffer ready?
   TXBUF1 = command;                          // command to TXBUF0
}
void recieve()
{   send_uart('0');
	stat_read=0;
	_BIS_SR(CPUOFF + GIE);                 // Enter LPM3 w/ interrupt
    }
*/


#pragma vector=USART1RX_VECTOR
__interrupt void usart1_rx (void)
{ flag=0;
  stat_read=RXBUF1;
  __bic_SR_register_on_exit(CPUOFF);
 }
