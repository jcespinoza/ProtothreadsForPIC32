#include "SerialFLASH.h"

/************************************************************************
* Function: SFLASHInit                                                  
*                                                                       
* Overview: this function setup SPI and IOs connected to SFLASH
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none
*                                                                       
************************************************************************/
void SFLASHInit()
{
/************************************************************************
* For MikroE- MMB RC2 is connected to Serial Flash chip select.
* SPI2 is used to access the Serial Flash 
************************************************************************/

    // Initialize SPI2
    SPI2STAT = 0;
    SPI2CON = 0;
    SPI2BRG = 0;
    SPI2CONbits.MSTEN = 1; 
    SPI2CONbits.CKP = 1;
    SPI2CONbits.CKE = 0;
    SPI2CONbits.SMP = 0;
    SPI2BRG = 1;
    SPI2CONbits.ON = 1;

    // Set IOs directions for SFLASH SPI
    CS_FLASH_LAT_BIT = 1;
    CS_FLASH_TRIS_BIT = 0;

    _TRISG6 = 0;
    _TRISG7 = 1; 
    _TRISG8 = 0;
    
//    SFLASHResetWriteProtection();
}

/************************************************************************
* Function SPIPut(BYTE data)                                                       
*                                                                       
* Overview:  this function sends a byte                     
*                                                                       
* Input: byte to be sent
*                                                                       
* Output: none
*                                                                       
************************************************************************/           
PRIVATE void SPIPut(BYTE data)
{
    // Wait for free buffer
    while(!SPI2STATbits.SPITBE);
    SPI2BUF = data;
    // Wait for data byte
    while(!SPI2STATbits.SPIRBF);

}

/************************************************************************
* Macros SPIGet()                                                       
*                                                                       
* Overview:  this macros gets a byte from SPI                      
*                                                                       
* Input: none
*                                                                       
* Output: none
*                                                                       
************************************************************************/           
#define SPIGet() SPI2BUF

/************************************************************************
* Function: void SFLASHWriteByte(BYTE data, DWORD address)                                           
*                                                                       
* Overview: this function writes a byte to the address specified
*                                                                       
* Input: data to be written and address
*                                                                       
* Output: none                                 
*                                                                       
************************************************************************/
void SFLASHWriteByte(BYTE data, DWORD address)
{
    SFLASHWriteEnable();
    SFLASHCSLow();

    SPIPut(SFLASH_CMD_WRITE);
    SPIGet();

    SPIPut(((DWORD_VAL)address).v[2]);
    SPIGet();

    SPIPut(((DWORD_VAL)address).v[1]);
    SPIGet();

    SPIPut(((DWORD_VAL)address).v[0]);
    SPIGet();

    SPIPut(data);
    SPIGet();

    SFLASHCSHigh();

    // Wait for write end
    while(SFLASHIsWriteBusy());
}

/************************************************************************
* Function: BYTE SFLASHReadID( DWORD address)             
*                                                                       
* Overview: this function reads the  Manufacturer ID code          
*                                                                       
* Input: address                                                     
*                                                                       
* Output: data read
*                                                                       
************************************************************************/
BYTE SFLASHReadID( void){
BYTE temp;
    SFLASHCSLow();

    SPIPut(SFLASH_CMD_RDID);
    SPIGet();

    SPIPut(0);
    temp = SPIGet();

    SFLASHCSHigh();
    return temp;
}

/************************************************************************
* Function: BYTE SFLASHReadByte(DWORD address)             
*                                                                       
* Overview: this function reads a byte from the address specified         
*                                                                       
* Input: address                                                     
*                                                                       
* Output: data read
*                                                                       
************************************************************************/
BYTE SFLASHReadByte(DWORD address){
BYTE temp;
    SFLASHCSLow();

    SPIPut(SFLASH_CMD_READ);
    SPIGet();

    SPIPut(((DWORD_VAL)address).v[2]);
    SPIGet();

    SPIPut(((DWORD_VAL)address).v[1]);
    SPIGet();

    SPIPut(((DWORD_VAL)address).v[0]);
    SPIGet();

    SPIPut(0);
    temp = SPIGet();

    SFLASHCSHigh();
    return temp;
}

/************************************************************************
* Function: void SFLASHWriteWord(WODR data, DWORD address)                                           
*                                                                       
* Overview: this function writes a 16-bit word to the address specified
*                                                                       
* Input: data to be written and address
*                                                                       
* Output: none                                                         
*                                                                       
************************************************************************/
void SFLASHWriteWord(WORD data, DWORD address)
{
    SFLASHWriteByte(((WORD_VAL)data).v[0],address);
    SFLASHWriteByte(((WORD_VAL)data).v[1],address+1);
}

/************************************************************************
* Function: WORD SFLASHReadWord(DWORD address)             
*                                                                       
* Overview: this function reads a 16-bit word from the address specified         
*                                                                       
* Input: address                                                     
*                                                                       
* Output: data read
*                                                                       
************************************************************************/
WORD SFLASHReadWord(DWORD address){
WORD_VAL temp;

    temp.v[0] = SFLASHReadByte(address);
    temp.v[1] = SFLASHReadByte(address+1);

    return temp.Val;
}

/************************************************************************
* Function: SFLASHWriteEnable()                                         
*                                                                       
* Overview: this function allows write/erase SFLASH. Must be called  
* before every write/erase command.                                         
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none                                 
*                                                                       
************************************************************************/
void SFLASHWriteEnable(){
    SFLASHCSLow();
    SPIPut(SFLASH_CMD_WREN);
    SPIGet();
    SFLASHCSHigh();
}

/************************************************************************
* Function: BYTE SFLASHIsWriteBusy(void)  
*                                                                       
* Overview: this function reads status register and chek BUSY bit for write operation
*                                                                       
* Input: none                                                          
*                                                                       
* Output: non zero if busy
*                                                                       
************************************************************************/
BYTE SFLASHIsWriteBusy(){
BYTE temp;

    SFLASHCSLow();
    SPIPut(SFLASH_CMD_RDSR);
    SPIGet();

    SPIPut(0);
    temp = SPIGet();
    SFLASHCSHigh();

    return (temp&0x01);
}

/************************************************************************
* Function: BYTE SFLASHWriteArray(DWORD address, BYTE* pData, nCount)
*                                                                       
* Overview: this function writes a data array at the address specified
*                                                                       
* Input: flash memory address, pointer to the data array, data number
*                                                                       
* Output: return 1 if the operation was successfull
*                                                                     
************************************************************************/
BYTE SFLASHWriteArray(DWORD address, BYTE* pData, WORD nCount)
{
DWORD     addr;
BYTE*     pD;
WORD      counter;


    addr = address;
    pD   = pData;

    // WRITE

    for(counter=0; counter<nCount; counter++)
    {
        SFLASHWriteByte(*pD++, addr++);
    }


    // VERIFY

    for(counter=0; counter<nCount; counter++)
    {              
        if(*pData != SFLASHReadByte(address))
            return 0;
        pData++;
        address++;
    }

    return 1;
}

/************************************************************************
* Function: void SFLASHReadArray(DWORD address, BYTE* pData, nCount)
*                                                                       
* Overview: this function reads data into buffer specified
*                                                                       
* Input: flash memory address, pointer to the data buffer, data number
*                                                                       
************************************************************************/
void SFLASHReadArray(DWORD address, BYTE* pData, WORD nCount)
{

    SFLASHCSLow();

    SPIPut(SFLASH_CMD_READ);
    SPIGet();

    SPIPut(((DWORD_VAL)address).v[2]);
    SPIGet();

    SPIPut(((DWORD_VAL)address).v[1]);
    SPIGet();

    SPIPut(((DWORD_VAL)address).v[0]);
    SPIGet();

    while(nCount--){
        SPIPut(0);
        *pData++ = SPIGet();
    }

    SFLASHCSHigh();
}

/************************************************************************
* Function: void SFLASHChipErase(void)
*                                                                       
* Overview: chip erase
*                                                                       
* Input: none
*                                                                       
************************************************************************/
void SFLASHChipErase(void)
{
    SFLASHWriteEnable();

    SFLASHCSLow();

    SPIPut(SFLASH_CMD_ERASE);
    SPIGet();

    SFLASHCSHigh();

    // Wait for write end
    while(SFLASHIsWriteBusy());
}

/************************************************************************
* Function: void SFLASHResetWriteProtection()
*                                                                       
* Overview: this function reset write protection bits
*                                                                       
* Input: none                                                     
*                                                                       
* Output: none
*                                                                       
************************************************************************/
void SFLASHResetWriteProtection(){

    SFLASHCSLow();

    SPIPut(SFLASH_CMD_EWSR);
    SPIGet();

    SFLASHCSHigh();

    SFLASHCSLow();

    SPIPut(SFLASH_CMD_WRSR);
    SPIGet();

    SPIPut(0);
    SPIGet();

    SFLASHCSHigh();
}
/************************************************************************
* Function: void SFLASHSectorErase(DWORD address)                                           
*                                                                       
* Overview: this function erases a 4Kb sector
*                                                                       
* Input: address within sector to be erased
*                                                                       
* Output: none                                 
*                                                                       
************************************************************************/
void SFLASHSectorErase(DWORD address)
{
    SFLASHWriteEnable();
    SFLASHCSLow();

    SPIPut(SFLASH_CMD_SER);
    SPIGet();

    SPIPut(((DWORD_VAL)address).v[2]);
    SPIGet();

    SPIPut(((DWORD_VAL)address).v[1]);
    SPIGet();

    SPIPut(((DWORD_VAL)address).v[0]);
    SPIGet();

    SFLASHCSHigh();

    // Wait for write end
    while(SFLASHIsWriteBusy());
}
