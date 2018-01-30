//程序
/*--------------------------------main.c-------------------------------------------*/
/*---------------------------------------------------------------------------------
头文件
-----------------------------------------------------------------------------------*/
#include "cpu_reg.h"
/*----------------------------------------------------------------------------------
宏定义
------------------------------------------------------------------------------------*/
//七音的半个周期，单位0.01ms
#define DO 15000
#define RE 14000
#define MI 13000
#define FA 12000
#define SO 11000
#define LA 10000
#define XI 9000
#define _DO 8000
#define _RE 7000
#define _MI 6000
#define _FA 5200
#define _SO 4000
#define _LA 3000
#define _XI 2000
#define WU 0
/*-----------------------------------------------------------------------------------
global variable
-------------------------------------------------------------------------------------*/
int tt=2;         //length of each sound
unsigned int s; //timer time
int AMPL = 300;
int amp; //sound amplitude
unsigned char f=1;
unsigned char flag=0; //recent working state，0：single sound，1：music
unsigned int const music0[]={DO,RE,MI,FA,SO,LA,XI,_DO,_RE,_MI,_FA,_SO,_LA,_XI};
unsigned int const music1[]={DO,RE,RE,MI,SO,SO,RE,FA,FA,LA,XI,XI};
unsigned int const tian_f[]={ //the city of the sky
LA,XI,_DO,XI,_DO,_MI,XI,WU,     MI,LA,SO,LA,_DO,SO,WU,
FA,MI,FA,MI,FA,_DO,MI,WU, _DO,_DO,_DO,XI,FA,FA,XI,XI,WU,
LA,XI,_DO,XI,_DO,_MI,XI,WU,     MI,MI,LA,SO,LA,_DO,SO,WU,
MI,FA,_DO,XI,_DO,_RE,_MI,_DO,_DO,XI,LA,XI,SO,LA,WU,
_DO,_RE,_MI,_RE,_MI,_SO,_RE,WU,
SO,SO,_DO,XI,_DO,_MI,_MI,WU,
LA,XI,_DO,XI,_DO,_RE,_DO,SO,SO,WU,
_FA,_MI,_RE,_DO,_MI,
};
unsigned int const tian_t[]={ //beat of the city of the sky

2,2,3,1,2,2,2,2,           2,3,1,2,2,2,2,
1,1,3,1,2,2,2,             1,1,1,1,3,1,2,2,2,2,
1,1,3,1,2,2,2,1,           1,1,3,1,2,2,2,2,
2,2,1,1,2,2,1,1,1,1,2,2,2,2,1,
1,1,3,1,2,2,2,2,
1,1,3,1,2,2,4,1,
1,1,2,1,1,2,3,1,2,1,
2,2,2,2,6,
};


/*---------------------------------------------------------------------------------------
main function
-----------------------------------------------------------------------------------------*/
void main()
{
  unsigned int i,temp,m;
  clock_init(); //时钟初始化
  timer_init(); //定时器初始化
  McBsp0_init(); //串口初始化
  asm(" RSBX INTM "); /*开中断*/
  m=0; //press to play single sound，the beginning sound
  flag=0; //work state：press key to play single sound
  while(1)
  {
    temp = ScanKey(); //scan key
    if(temp == 0xfe && m<13)
      m++;              //change the serial number of sound by pressing keys
    else if(temp == 0xfd && m>0)
      m--;
      
    else if(temp == 0xfb)
    {
      flag^=1;        //change work state
      m=0;
      i=0;
    }
    
    else if(temp == 0xf7)   //amplitude down
    { 
      if( AMPL>100)
         AMPL -= 100;
    } 
    else if(temp == 0xef)   //amplitude up
    { 
      if(AMPL<500)
         AMPL += 100;
    }           
    
    else if(temp == 0xdf)
    {
      if(tt<3)
        tt+=1;
    }
    
    else if(temp == 0xbf)
    {
      if(tt>1)
        tt-=1;
    }
    
    
    //  choose the work state
    if(flag==0) //press key to put on sound
    {
      switch(m)
      {
      case 0:  sound(DO,2);  break;
      case 1:  sound(RE,2);  break;
      case 2:  sound(MI,2);  break;
      case 3:  sound(FA,2);  break;
      case 4:  sound(SO,2);  break;
      case 5:  sound(LA,2);  break;
      case 6:  sound(XI,2);  break;
      case 7:  sound(_DO,2); break;
      case 8:  sound(_RE,2); break;
      case 9:  sound(_MI,2); break;
      case 10: sound(_FA,2); break;
      case 11: sound(_SO,2); break;
      case 12: sound(_LA,2); break;
      case 13: sound(_XI,2); break;
      }
    }
    else if (flag == 1) //play the musics
    {
      m = sizeof tian_f /sizeof tian_f[0];
      i++;
      if(i==m) //loop，stop after playing for one time
      {
        i=0;
        s=0;
        while(s<1000/1.0);
      }
      sound(tian_f[i],tian_t[i]);
    }
  }
}

/*--------------------------------------------------------------------------
function declaration
----------------------------------------------------------------------------*/
void clock_init();
void timer_init();
unsigned int ScanKey(void);
void sound(unsigned int pred, unsigned int t);
void write_subreg0(unsigned int addr,unsigned int val);
unsigned int read_subreg0(unsigned int addr);
void McBsp0_init();
void delay(unsigned int t);


/*--------------------------------------------------------------------------
function declaration
---------------------------------------------------------------------------*/
//--------------------------------------------------------------------------
// clock initialization
//--------------------------------------------------------------------------

void clock_init()
{
  asm(" STM #0000h,CLKMD ");
  while(*CLKMD & 0x01 );
  asm(" STM #40C7h,CLKMD "); //设置CPU 运行频率＝100M
  /* 40C7h:5*clkin =100M
  30c7h:4*clkin =80M
  20c7h:3*clkin =60M
  10C7h:2*clkin =40M
  */
  asm(" stm #4240h, SWWSR ");
}
//-----------------------------------------------------
// 定时器初始化
//-----------------------------------------------------
void timer_init()
{
  asm(" STM #0010h,TCR "); //关定时器
  asm(" STM #1869h,PRD "); //1ms
  asm(" STM #0C2fh,TCR "); //TCR=最后四位
  asm(" STM #0008h,IFR "); //清中断标志位
  asm(" ORM #0008h,*(IMR) "); /*开时间中断*/
}

unsigned int read_subreg0(unsigned int addr)
{
  *(SPSA0)=addr;
  return(*SPSD0);
}
void write_subreg0(unsigned int addr,unsigned int val)
{
  *(SPSA0)=addr;
  *(SPSD0)=val;
}
//-------------------------------------------------------------------------------------
// McBsp0 initialization
//-------------------------------------------------------------------------------------
void McBsp0_init()
{
  write_subreg0(SPCR1,0);
  write_subreg0(SPCR2,0);
  write_subreg0(SPCR1,SPCR10_VAL);
  write_subreg0(SPCR2,SPCR20_VAL);
  write_subreg0(PCR,PCR0_VAL);
  write_subreg0(RCR1,RCR10_VAL);
  write_subreg0(RCR2,RCR20_VAL);
  write_subreg0(XCR1,XCR10_VAL);
  write_subreg0(XCR2,XCR20_VAL);
  delay(10);
  *(DXR10)=0;
  /*now enable McBSP transmit and receive*/
  write_subreg0(SPCR1,SPCR10_VAL|1);
  write_subreg0(SPCR2,SPCR20_VAL|1);
  delay(10);

  *(IMR)|=0x0010; //开接收0 中断
}
//---------------------------------------------------------------------------------------
// scan key
//---------------------------------------------------------------------------------------
unsigned int ScanKey(void)
{
  unsigned int temp,temp1;
  temp = KeyReg;
  temp = temp & 0xff;
  if(temp==0xff)
  {
    return 0xff;
  }
  else
  {
    delay(5000);
    temp = KeyReg;
    temp = temp & 0xff;
    temp1 = temp;
    if (temp == 0xff)
    {
      return 0xff;
    }
    else
    {
      temp = KeyReg;
      temp = temp & 0xff;
      while( temp != 0xff)
      {
        temp = KeyReg;
        temp = temp & 0xff;
      }
    }
  }
return(temp1);
}
//---------------------------------------------------------------------------------------
// time interrupt function
//---------------------------------------------------------------------------------------
interrupt void timer0()
{
  s++;
}
//---------------------------------------------------------------------------------------
// McBsp0 interrupt function
//---------------------------------------------------------------------------------------

interrupt void codec_ch0_in() //接收0 中断
{
  int temp;
  temp=*DRR10;
  *DXR10 = amp;
}
//----------------------------------------------------------------------------------------
// delay
//----------------------------------------------------------------------------------------
void delay(unsigned int t)
{
  while(t--);
}
//---------------------------------------------------------------------------------------
// play single sound
//---------------------------------------------------------------------------------------
void sound(unsigned int pred, unsigned int t)
{
  unsigned int i;
  for(i=0; i<t*tt; i++)
  {
    if(pred)
    {
      s=0;
      while(s<250/1.0)
      {
        amp = AMPL;
        delay(pred);
        amp = -AMPL;
        delay(pred);
      }
    }
    else
    {
      s=0;
      while(s<250/1.0);
    }
  }
  s=0;
  while(s<30/1.0);
}



