#include "delay.h"
#include "flash.h"
#include "edit.h"
#include"gui.h"
#include"wm.h"
#include <string.h>
#include "filerw.h"

#define FLASH_SIZE ((8*1024*1024) / 1024)
#define  BytesPerGroup (ModeNameLenMax + 2 * sizeof(unsigned) * maxstep + 1)
#define  MaxNumOfGroups  ((FLASH_SIZE - 1) / BytesPerGroup)

//#define AddrStart (sizeof(unsigned int))
#define AddrGroupIndex (sizeof(unsigned int))
#define AddrData (MaxNumOfGroups + AddrGroupIndex)

//char fnamebuf[30];
//char fileReadBuf[fileReadBufMax];//����ÿ��touch_child����֮��Ͱ����ݶ���filereadbuf����ok֮�������д��flash��ǰ���������������
unsigned groupIndex[MaxNumOfGroups];
unsigned groupIndexCounter = 0;

char curModeNameBuf[ModeNameLenMax];
unsigned curDuration[maxstep];
unsigned curSpeed[maxstep];
//����mode
char modeName1[ModeNameLenMax];
char modeName2[ModeNameLenMax];
char modeName3[ModeNameLenMax];
unsigned speed1[maxstep];
unsigned speed2[maxstep];
unsigned speed3[maxstep];
unsigned duration1[maxstep];
unsigned duration2[maxstep];
unsigned duration3[maxstep];

unsigned getInt(unsigned int addr)
{
		unsigned res;
		SPI_Flash_Read((char*)&res, addr, sizeof(unsigned int));
		return res;
}

void setInt(unsigned int addr, unsigned dat)
{
		SPI_Flash_Write((char*)&dat, addr, sizeof(unsigned int));
}

unsigned int getCount(void)
{
		return getInt(0);	
}

void setCount(unsigned int cnt)
{
		setInt(0, cnt);
}

unsigned char readData(char *modename, unsigned int *speedArr, unsigned int *durationArr, unsigned index)
{
		unsigned addr;
		if(index >= getCount()){
				return 0;
		}	
		addr = getInt(AddrGroupIndex + index);//get the absolute address
		SPI_Flash_Read((char*)modename, addr, ModeNameLenMax);
		SPI_Flash_Read((char*)speedArr, addr+ModeNameLenMax, maxstep * (sizeof(unsigned)));
		SPI_Flash_Read((char*)durationArr, addr+ModeNameLenMax+maxstep * (sizeof(unsigned)), maxstep * (sizeof(unsigned)));
		return 1;
}

unsigned char writeData(char *modename, unsigned int *speedArr, unsigned int *durationArr)
{		
		unsigned cnt = getCount();
		unsigned addr = AddrData + cnt * BytesPerGroup;
		if(cnt >= MaxNumOfGroups){
				return 0;
		}
		SPI_Flash_Write((char*)modename, addr, ModeNameLenMax);
		SPI_Flash_Write((char*)speedArr, addr+ModeNameLenMax, maxstep * (sizeof(unsigned)));
		SPI_Flash_Write((char*)durationArr, addr+ModeNameLenMax+maxstep * (sizeof(unsigned)), maxstep * (sizeof(unsigned)));
		
		groupIndex[groupIndexCounter++] = addr;

		return 1;
}
void editData(char *modename, unsigned int *speedArr, unsigned int *durationArr, unsigned index)
{		
		unsigned addr = groupIndex[index];
		SPI_Flash_Write((char*)modename, addr, ModeNameLenMax);
		SPI_Flash_Write((char*)speedArr, addr+ModeNameLenMax, maxstep * (sizeof(unsigned)));
		SPI_Flash_Write((char*)durationArr, addr+ModeNameLenMax+maxstep * (sizeof(unsigned)), maxstep * (sizeof(unsigned)));
}

void deleteData(unsigned index)
{
		unsigned int i;
		for(i = index; i < groupIndexCounter - 1; ++i){
				groupIndex[i] = groupIndex[i+1];
		}
		--groupIndexCounter;
}

void swapGroupIndexBuf(unsigned index1, unsigned index2)
{
		groupIndex[index1] ^= groupIndex[index2];
		groupIndex[index2] ^= groupIndex[index1];
		groupIndex[index1] ^= groupIndex[index2];
}

void refreshGroupIndex(void)
{
		SPI_Flash_Write((char*)groupIndex, AddrGroupIndex, groupIndexCounter);
		setCount(groupIndexCounter);
}
/*void swapData(unsigned index1, unsigned index2)
{
		unsigned tmp1, tmp2;
		tmp1 = getInt(AddrGroupIndex + index1);
		tmp2 = getInt(AddrGroupIndex + index2);
		setInt(AddrGroupIndex + index1, tmp2);
		setInt(AddrGroupIndex + index2, tmp1);
}*/
/*void swapData(unsigned index1, unsigned index2)
{
		unsigned char buf[fileReadBufMax];
		readData((unsigned*)buf, (unsigned*)(buf+fileReadBufMax/2), index1);
		readData((unsigned*)fileReadBuf, (unsigned*)(fileReadBuf+fileReadBufMax/2), index2);
	
		SPI_Flash_Write((char*)fileReadBuf, StartByte + index1 * fileReadBufMax, fileReadBufMax / 2);
		SPI_Flash_Write((char*)(fileReadBuf+fileReadBufMax / 2), StartByte + index1 * fileReadBufMax + fileReadBufMax / 2, fileReadBufMax / 2);
	
		SPI_Flash_Write((char*)buf, StartByte + index2 * fileReadBufMax, fileReadBufMax / 2);
		SPI_Flash_Write((char*)(buf+fileReadBufMax / 2), StartByte + index2 * fileReadBufMax + fileReadBufMax / 2, fileReadBufMax / 2);	
}*/

unsigned initData(void)
{
		unsigned cnt = getCount(), i = 0;
		groupIndexCounter = cnt;
		if(cnt == 0){
				return 0;
		}
		readData(modeName1, speed1, duration1, 0);
		if(1 == cnt){
				return 1;
		}
		readData(modeName2, speed2, duration2, 1);
		if(2 == cnt){
				return 2;
		}	
		readData(modeName3, speed3, duration3, 1);
		return 3;
}
/*
void getFileList(char **fnamelist)
{
		
}
void retrieveMode(char *fname)
{
		
}

unsigned retrueveName(unsigned char index)
{
		char teststr[] = "teststring";
		unsigned int len = strlen(teststr);
		SPI_Flash_Write(teststr,FLASH_SIZE-len*2,len);
		delay_ms(100);
		SPI_Flash_Read(fileReadBuf,FLASH_SIZE-len*2,len);
		fileReadBuf[len] = 0;
		return 0;
}

void test(EDIT_Handle hwin)
{
		unsigned int i = 0;
//		i = retrueveName(1);
	//	EDIT_SetText(hwin, fileReadBuf);
		SPI_Flash_Write((char*)&i, 0, (sizeof(unsigned)));
}
*/