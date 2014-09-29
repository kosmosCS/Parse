
#include "LPC17xx.h"
#include "lpc_types.h"
#include "debug_frmwrk.h"
#include "inter_volatile.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_pinsel.h"
#include "L1754adc.h"
#include "stdio.h"
#include "lpc17xx_mcpwm.h"
#include "Application.h"
#include "string.h"


void receivingHandler(void);

void _sbrk_r(void){

}


#define		N 	50 // amount of commands
#define		L 	25 // length of command

//#define true	1
//#define false	0

#define MB_ADDRESS_SLAVE_1    '0'
#define MB_ADDRESS_SLAVE_2 	  '1'
#define MB_ADDRESS_SLAVE    1
typedef unsigned char UCHAR;

volatile char ch, Buf[255];
volatile uint16_t n=0;

static uint16_t mainPointer = 0;


typedef struct
{
	char number;
	char flag;
    char name[L];
    char length;
    char argFlag;
}myCom;

myCom CommList[N] =
{
/*  +-------+--------+--------------------+---------+----------------------------------------------
 *  |number	| flag	 |	str of function	  |  length | flag for arg = 0b0000000
 * 	+-------+--------+--------------------+---------+------------------+++++++---------------------
 * 	|		|		 |					  |				               |||||||
 * 	|		|		 |					  |				               ||||||+------- without argument 		0x01
 * 	|		|		 |					  |				               |||||+-------- only numbers			0x02
 * 	|		|		 |					  |				               ||||+--------- alphabetical symbols	0x04
 * 	|		|		 |					  |				               |||+----------
 * 	|		|		 |					  |				               ||+-----------
 * 	|		|		 |					  |				               |+------------
 * 	|		|		 |					  |				               +-------------
 */
	  {0, 	  0,		"0000",				0,						0	},
	  {1,	  0, 		"ADDR",				4,						0x02},
      {2,	  0, 		"B",				1,						0x02},
      {3,	  0, 		"BAUD",				4,						0x02},
      {4,	  0, 		"BO",				2,						0x02},
      {5,	  0, 		"BS",				2,						0x02},
      {6,	  0, 		"CONF",				4,						0x02},
      {7,	  0, 		"D",				1,						0x04},
      {8, 	  0,		"FLASH",			5,						0x02},
      {9, 	  0,		"INT",				3,						0x02},
      {10, 	  0,		"O",				1,						0x02},
      {11, 	  0,		"PT",				2,						0x02},
      {12, 	  0,		"RST",				3,						0x01},
      {13, 	  0,		"RST/C",			5,						0x01},
      {14, 	  0,		"S",				1,						0x02},
      {15, 	  0,		"WRITE",			5,						0x01},
      {16, 	  0,		"RESETTOFACTORY",	14,						0x01}


};


myCom *pCom = &CommList[0];								//testing

enum{false,true};
volatile union fltype
{
	volatile struct bites
	{
		 unsigned start:		1;
		 unsigned adrDone:		1;
		 unsigned comDone:		1;
		 unsigned allEnd: 		1;

		 unsigned secondS:       1;
		 unsigned secondN:       1;
		 unsigned Restart:      1;
	}bits;
	char allFlags;
}byte;

volatile struct bites *pbit = &byte.bits;

typedef enum
{
	doNothing,
	clearing,
	checkingAddress,
	iniSecondsS,
	checkingCommand,
	checkingArgument,
	executing
}Stat;
volatile Stat myStat;




void
UART0_IRQHandler(void)
{
	unsigned long iir;

	iir = LPC_UART0->IIR;

	if ((iir & 0x4) || (iir & 0xC)) {            // RDA or CTI pending
//	    while (LPC_UART0->LSR & 0x01) {                 // Rx FIFO is not empty
//	      SER_BUF_WR(ser_in, LPC_UART0->RBR);           // Read Rx FIFO to buffer
//	    }
		receivingHandler();
		//LPC_GPIO0->FIOSET0 |= (1<<1);
	//	_db_msg(LPC_UART0, "\r\nYES");

	  }
	  if ((iir & 0x2)) {                           // TXMIS pending

	  }
}

//inline

//void addrCheck(volatile char* pBuf, uint16_t *nSymFunc)
//{
//	/* Необходим спецификатор static, так как ждем прихода
//	 * алфавитного символа, чтобы проверить последние символы в буфере. */
//	static UCHAR address = 0;
//	char str[15];
//	 /* Allow only numbers. */
//	if (*(pBuf) >= '0' && *(pBuf) <= '9')
//	{
//		 /* Converting address, taking only last 2 symbols from pBuf. */
//		address = 10*(*(pBuf - 1) - '0') + (*(pBuf) - '0');
//	}
//	else
//	{
//		 /* Checking address. */
//		if (address == MB_ADDRESS_SLAVE)
//		{
//			_db_msg(LPC_UART0, "address is good\r\n");
//
//			 /* Needed operation. */
//			*nSymFunc = mainPointer + 1;
//
//			 /* ~Protection from typing s symbol. */
//			pbit->adrDone = true;
//			myStat = checkingCommand;
//		}
//		else
//		{
//			myStat = doNothing;
//
//			pbit->start = false;
//			_db_msg(LPC_UART0, " Address is invalid\r\n");
//		}
//	}
//}

void
addrCheck(volatile char* pBuf)
{
	switch (n)
	{
	case 1:
		if (*pBuf != MB_ADDRESS_SLAVE_1)
			{
			/* Можно и doNothing, хотя ...*/
				myStat = clearing;
				pbit->start = false;
				_db_msg(LPC_UART0," Address is invalid\r\n");
			}
		break;

	case 2:
		if (*(pBuf + 1) == MB_ADDRESS_SLAVE_2)
		{
			_db_msg(LPC_UART0, "address is good\r\n");

			 /* Needed operation. */
			mainPointer = 3;

			 /* ~Protection from typing s symbol. */
			pbit->adrDone = true;
			myStat = checkingCommand;
		}
		else
		{
			/* Можно и doNothing, хотя ...*/
			myStat = clearing;

			pbit->start = false;
			_db_msg(LPC_UART0," Address is invalid\r\n");
		}
		break;
	default:
		break;
	}
}

inline
UCHAR MBCHAR2BIN()
{
	return 10*(Buf[0] -'0') + (Buf[1] -'0');
}

void
receivingHandler(void)
{
	ch = UARTGetChar(LPC_UART0);
	switch (ch)
	{
		case 'S':
			/* break;
			 * Специально закоментированный break. */
		case 's':
			if (!(pbit->adrDone))
			{
				byte.allFlags = 0;
				/* Специально подчеркиваю что это начало.
				 * Можно byte.allFlags = 1. */
				pbit->start = true;
				pCom = &CommList[0];
				n=0;
				mainPointer = 0;
				myStat = checkingAddress;
				_db_msg(LPC_UART0, "session_started\r\n");
			}
			else
			{
				/* Защита от второго начала команды s01. */
				pbit->secondS = true;
				/* Защита от второго начала команды s01. */

				_db_msg(LPC_UART0, "\r\nSecondSSS \r\n");
				if (ch >= 'a' && ch <='z' && !pbit->comDone) ch = ch - 32;
				Buf[n++] = ch;
			}
			break;
		case '\r':
			if (pbit->start)
				{
					_db_msg(LPC_UART0, "\r\nsession_finished\r\n");
					byte.allFlags = 0;
					myStat = executing;
				}
			break;
		default:
			if (pbit->start)
			{
				if (ch >= 'a' && ch <='z' && !pbit->comDone)  ch = ch - 32;


				/* Защита от второго начала команды s01. */
				if ((!(ch >= '0' && ch <= '9') && pbit->secondS))
				{
					pbit->secondN = true;
					pbit->secondS = false;
				}
				else if (ch == MB_ADDRESS_SLAVE_1 && !pbit->secondN && pbit->secondS)
					{
						//pbit->secondN = true;
						_db_msg(LPC_UART0, "\r\nSSS - First \r\n");
					}
					else if (ch == MB_ADDRESS_SLAVE_2 && !pbit->secondN && pbit->secondS)
					{
						byte.allFlags = 0;
						/* Специально подчеркиваю что это начало.
						 * Можно byte.allFlags = 1. */
						pbit->start = true;
						n = 1;
						pbit->secondN = true;
						pbit->secondS = false;
						myStat = iniSecondsS;
						_db_msg(LPC_UART0, "\r\nSSS - Second \r\n");
					}
					else
					{
						pbit->secondS = false;
						pbit->secondN = true;
					}
				 /* Защита от второго начала команды s01. */


				Buf[n++] = ch;
			}
			break;
	}
}


void Errors (void)
{
	_db_msg(LPC_UART0, "\r\nError \r\n");
}

uint16_t
Parse (volatile char* pBuf, uint16_t nChCom, uint16_t *nMatchCommand)
{
	uint16_t nMatches = 0;
	/* Обязательно ставим 1, так как 0-ой необходим для указания ошибки. */
	uint16_t iStr = 1;	//var to brute

	while(iStr < N)  //brute force all commands by 1 character								//changed N to 3
	{

		if (*pBuf == CommList[iStr].name[nChCom] && CommList[iStr].flag == nChCom)

		{
			CommList[iStr].flag = nChCom + 1;

			*(nMatchCommand + nMatches) = iStr;
			nMatches++;
		}
		iStr++;
	}
	/* Анализируем количество. */
	if (nMatches != 0)
	{
		iStr = 0;
		while(iStr < nMatches)
		{

			if ((nChCom + 1) == (uint16_t) CommList[*nMatchCommand + iStr].length)
			{
				pCom = &CommList[*(nMatchCommand + iStr)];
				break;
			}
			else pCom = &CommList[0];
			iStr++;
		}
	}
	else pCom = &CommList[0];

	return nMatches;
}


void
Poll ()
{
	int i=0;
	static char value[N];
	/* Статичность обязательна, так как содержит номер начального символа команды и аргумента в pBuf*/
	static uint16_t nSymArg = 0;
	volatile char *pBuf;  //pointer to UART buffer
	static uint16_t nMatches = 0; //amount of matches
	uint16_t nMatchCommand[N];	//num of match

	pBuf = Buf;
	switch(myStat)
	{
		case doNothing:

			break;
		case clearing:
			pCom = &CommList[0];
			i = 1;
			while(i < N)  //brute force all commands by 1 character
			{
				CommList[i].flag = 0;
				nMatchCommand[i] = 0;
				value[i] = '\0';
				i++;
			}
			n = 0;
			mainPointer = 0;
			byte.allFlags = 0;
			value[0] = '\0';
			myStat = doNothing;
			break;
		case checkingAddress:
				addrCheck(pBuf);

			break;
		case iniSecondsS:
			mainPointer = 3;
			i = 1;
			pCom = &CommList[0];
			while(i < N)  //brute force all commands by 1 character
			{
					CommList[i].flag = 0;
					nMatchCommand[i] = 0;
					value[i] = '\0';
					i++;
			}

			value[0] = '\0';
			myStat = checkingCommand;
			break;
		case checkingCommand:

			/* Checking if Pointer less then current number of symbols. */
			if (mainPointer <= n && n >= 3)
			{
				/* Allow only alphabetical symbols. */
				if (!(*(pBuf + mainPointer - 1) >= '0' && *(pBuf + mainPointer - 1) <= '9') && *(pBuf + mainPointer - 1) != '-')
				{
					/* Bias to next symbol of command. */
					pBuf = &Buf[mainPointer - 1];

					/* Отправляем буфер с первого символа команды, номер символа в команде,
					 * номер команды которая совпала.
					 * Получаем количество совпавших команд. */
					nMatches = Parse(pBuf, (mainPointer - 3), &nMatchCommand[0]);


					/* "Защита от второго начала команды s01".
					 * Из-за нее пришлось перестроить*/
					if (pCom->argFlag == 0x04)
						{
							myStat = checkingArgument;
							value[0] = '\0';
							pbit->comDone = true;
						}
					/* Защита от второго начала команды s01. */


					/* Смещение на 1 символ. */
					mainPointer++;
				}
				else
				{
					if (nMatches == 0)
						{
							_db_msg(LPC_UART0, "\r\nNot found matches\r\n");
							myStat = doNothing;
						}
					//	myStat = clearing;
					else
					{
						value[0] = '\0';
						pbit->comDone = true;
						myStat = checkingArgument;
					}
				}
				nSymArg = mainPointer;
			}
			break;
		case checkingArgument:

			switch (pCom->argFlag)
			{
				case 1:
					pbit->allEnd = true;
					myStat = executing;
					break;
				case 2:
					if (mainPointer <= n)
					{
						if ( ( *(pBuf + mainPointer - 1) >= '0' &&*(pBuf + mainPointer - 1) <= '9') ||
								 *(pBuf + mainPointer - 1) =='-')
						{
							value[(mainPointer - nSymArg)] = *(pBuf + mainPointer - 1);
						//	_db_msg(LPC_UART0, "\r\nIn argument");
						}
						else
						{
							Errors();
							myStat = clearing;
						}
						mainPointer++;
					}

					break;
				case 4:
					if (mainPointer <= n)
					{
						value[(mainPointer - nSymArg)] = *(pBuf + mainPointer - 1);
						mainPointer++;
					}
					break;
				default:
					break;
			}
			break;
		case executing:

			if (pCom->length != 0)
			{
				_db_msg(LPC_UART0, "\r\nething is good - ");
				_db_msg(LPC_UART0,pCom->name);
				_db_msg(LPC_UART0, "\r\nvalue - ");
				_db_msg(LPC_UART0, value);
			}
			else Errors();
			myStat = clearing;
			break;
		default:
			_db_msg(LPC_UART0, "\r\nsmthWrong\r\n");
			break;
	}
}


/*----------------------------------------------------------------------------
 Main Program
 *---------------------------------------------------------------------------*/

int main(void)
{

	debug_frmwrk_init(); //интерфейс отладки, дебаг интерфейс
   _db_msg(LPC_UART0, "\r\nHello I'am SHIM  ver. 0.1\r\n");

	LPC_UART0->LCR &= ~(1<<7);  //  DLAB =0

    LPC_UART0->IER |= (1<<0) | (1<<1) | (1<<2);  //enable IRQ
    NVIC_EnableIRQ(UART0_IRQn);


	LPC_GPIO0->FIODIR |= (1<<0);
	LPC_GPIO0->FIOCLR0 |= (1<<0);

	LPC_GPIO0->FIODIR |= (1<<1);
	LPC_GPIO0->FIOCLR0 |= (1<<1);

	LPC_GPIO1->FIODIR3 |= (0<<5);

	myStat = doNothing;



	while (1)
	{
		Poll();
	}
	return 0;
} // end main ()

/*
 * @}
 */
