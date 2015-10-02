/**
 * Get SD card drive size
 *
 * Parameters:
 * 	- uint32_t* total:
 *		Pointer to variable to store total size of card
 * 	- uint32_t* free:
 *		Pointer to variable to store free space on card
 *
 * Returns FRESULT struct members. If data are valid, FR_OK is returned.
 */
FRESULT TM_FATFS_DriveSize(uint32_t* total, uint32_t* free);

/**
 * Get SD card drive size
 *
 * Parameters:
 * 	- uint32_t* total:
 *		Pointer to variable to store total size of card
 * 	- uint32_t* free:
 *		Pointer to variable to store free space on card
 *
 * Returns FRESULT struct members. If data are valid, FR_OK is returned.
 */
FRESULT TM_FATFS_USBDriveSize(uint32_t* total, uint32_t* free);

/**
 * Truncate beginning of file
 *
 * Parameters:
 * 	- FIL* fil: 
 *		Pointer to already opened file
 * 	- uint32_t index:
 *		Number of characters that will be truncated from beginning.
 *		If index is more than file size, everything will be truncated, but file will not be deleted
 *		
 * Example
 *	- You have a file, it's content is: "abcdefghijklmnoprstuvwxyz",
 *	- You want to truncate first 5 bytes,
 *	- Call TM_FATFS_TruncateBeginning(&opened_file, 5);
 *	- You will get new file data: "fghijklmnoprstuvwxyz"
 *
 * Returns FRESULT struct members. If everything ok, FR_OK is returned.
 */
FRESULT TM_FATFS_TruncateBeginning(FIL* fil, uint32_t index);