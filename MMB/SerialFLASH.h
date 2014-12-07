/*****************************************************************************
 *
 * Basic access to SPI Flash SFLASH (25P80) located on MikroE PIC32MX4 MMB 
 *
 *****************************************************************************/
#ifndef _MMB_SERIAL_FLASH
#define _MMB_SERIAL_FLASH

#include "HardwareProfile.h"

/************************************************************************
* SFLASH Commands                                                       
************************************************************************/
#define SFLASH_CMD_RDID     (unsigned)0x9F  // 25P80
#define SFLASH_CMD_READ     (unsigned)0x03
#define SFLASH_CMD_WRITE    (unsigned)0x02
#define SFLASH_CMD_WREN     (unsigned)0x06
#define SFLASH_CMD_RDSR     (unsigned)0x05
#define SFLASH_CMD_ERASE    (unsigned)0xC7   // 25P80
#define SFLASH_CMD_EWSR     (unsigned)0x06   // 25P80
#define SFLASH_CMD_WRSR     (unsigned)0x01
#define SFLASH_CMD_SER      (unsigned)0xD8    //25P80

/************************************************************************
* Macro: SFLASHCSLow()                                                   
*                                                                       
* Preconditions: CS IO must be configured as output
*                                                                       
* Overview: this macro pulls down CS line                    
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none                                                         
*                                                                       
************************************************************************/
#define SFLASHCSLow()      CS_FLASH_LAT_BIT=0;

/************************************************************************
* Macro: SFLASHCSHigh()
*                                                                       
* Preconditions: CS IO must be configured as output
*                                                                       
* Overview: this macro set CS line to high level
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none
*                                                                       
************************************************************************/
#define SFLASHCSHigh()     CS_FLASH_LAT_BIT=1;

/************************************************************************
* Function: SFLASHInit()
*                                                                       
* Overview: this function setups SPI and IOs connected to SFLASH
*                                                                       
* Input: none
*                                                                       
* Output: none  
*                                                                       
************************************************************************/
void SFLASHInit();

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
BYTE SFLASHIsWriteBusy();

/************************************************************************
* Function: void SFLASHWriteByte(BYTE data, DWORD address)                                           
*                                                                       
* Overview: this function writes a byte to the address specified
*                                                                       
* Input: byte to be written and address
*                                                                       
* Output: none
*                                                                       
************************************************************************/
void SFLASHWriteByte(BYTE data, DWORD address);

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
BYTE SFLASHReadID( void);

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
BYTE SFLASHReadByte(DWORD address);

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
void SFLASHWriteWord(WORD data, DWORD address);

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
WORD SFLASHReadWord(DWORD address);

/************************************************************************
* Function: SFLASHWriteEnable()                                       
*
* Overview: this function allows writing into SFLASH. Must be called
*           before every write/erase command
*
* Input: none
*            
* Output: none
*
************************************************************************/
void SFLASHWriteEnable();

/************************************************************************
* Function: BYTE SFLASHWriteArray(DWORD address, BYTE* pData, nCount)
*                                                                       
* Overview: this function writes data array at the address specified
*                                                                       
* Input: flash memory address, pointer to the data array, data number
*                                                                       
* Output: return 1 if the operation was successfull
*                                                                     
************************************************************************/
BYTE SFLASHWriteArray(DWORD address, BYTE* pData, WORD nCount);

/************************************************************************
* Function: void SFLASHReadArray(DWORD address, BYTE* pData, nCount)
*                                                                       
* Overview: this function reads  data into buffer specified
*                                                                       
* Input: flash memory address, pointer to the buffer, data number
*                                                                       
************************************************************************/
void SFLASHReadArray(DWORD address, BYTE* pData, WORD nCount);

/************************************************************************
* Function: void SFLASHChipErase(void)
*                                                                       
* Overview: chip erase
*                                                                       
* Input: none
*                                                                       
************************************************************************/
void SFLASHChipErase(void);

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
void SFLASHResetWriteProtection();

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
void SFLASHSectorErase(DWORD address);

#endif
