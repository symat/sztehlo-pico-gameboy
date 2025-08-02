
// ****************************************************************************
//
//                                FAT file system
//
// ****************************************************************************
// PicoLibSDK - Alternative SDK library for Raspberry Pico and RP2040
// Copyright (c) 2023 Miroslav Nemecek, Panda38@seznam.cz, hardyplotter2@gmail.com
// 	https://github.com/Panda381/PicoLibSDK
//	https://www.breatharian.eu/hw/picolibsdk/index_en.html
//	https://github.com/pajenicko/picopad
//	https://picopad.eu/en/
// License:
//	This source code is freely available for any purpose, including commercial.
//	It is possible to take and modify the code or parts of it, without restriction.

// Currently is FAT system connected to SD-card interface (lib_sd.c, lib_sd.h)

#if USE_FAT	// use FAT file system (lib_fat.c, lib_fat.h)

#ifndef _LIB_FAT_H
#define _LIB_FAT_H

#include "lib_sd.h"
#include "../../_sdk/sdk_addressmap.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PATHCHAR	'/'	// default path separator (can be '\\' too)

// path or filename
#define PATH_MAX 254		// max. length of path and command line (do not use 255, u8+1 would overflow)
#define PATH_BUF (PATH_MAX+2)	// path buffer size

// sector size
#define SECT_SIZE_BITS 9	// number of bits per sector
#define SECT_SIZE (1 << SECT_SIZE_BITS) // 512 bytes
#define SECT_MASK (SECT_SIZE-1)	// sector mask

#define BOOTSIG		0xAA55	// boot signature
#define EXTSIG		0x29	// extended signature
#define LEADSIG		0x41615252 // FSI leading signature "RRaA"
#define STRUCSIG	0x61417272 // FSI structure signature "rrAa"

#define MAX_FAT12	0xFF5		// max FAT12 clusters
#define MAX_FAT16	0xFFF8		// max FAT16 clusters
#define MAX_FAT32	0x0FFFFFF5	// max FAT32 clusters

#define DIR_SIZE	0x20	// size of directory entry (=32)

// partition entry structure (16 bytes) ... max. 255 heads, max. 63 sectors per track, max. 1024 cylinders
typedef struct __attribute__ ((packed)) {
	u8	boot;		// (0x00) boot indicator
	u8	shead;		// (0x01) start head (0..254)
	u8	ssect;		// (0x02) bit 0..5: start sector (1..63), bit 6..7: start cylinder high 2 bits
	u8	scyl;		// (0x03) start cylinder low 8 bits (0..1023)
	u8	system;		// (0x04) system ID (0=no system)
	u8	ehead;		// (0x05) end head (0..254)
	u8	esect;		// (0x06) bit 0..5: end sector (1..63), bit 6..7: end cylinder high 2 bits
	u8	ecyl;		// (0x07) end cylinder low 8 bits (0..1023)
	u32	start;		// (0x08) start sector in LBA
	u32	size;		// (0x0C) size in sectors
} sPart;

// master boot sector MBR structure (512 bytes)
typedef struct __attribute__ ((packed)) {
	u8	res[446];	// (0x00) reserved
	sPart	part[4]; 	// (0x1BE) partition table
	u16	bootsig;	// (0x1FE) boot signature BOOTSIG (0xAA55)
} sMBR;

// boot sector structure - FAT12 and FAT16 (512 bytes)
typedef struct __attribute__ ((packed)) {
	u8	jmp[3];		// (0x00) jmp instruction
	char	oem[8];		// (0x03) OEM name
	u16	sectsize;	// (0x0B) sector size (in bytes)
	u8	clustsize;	// (0x0D) cluster size (in sectors)
	u16	reserved;	// (0x0E) reserved sectors - number of boot sectors	
	u8	fatnum;		// (0x10) number of FAT tables (1 or 2)
	u16	rootent;	// (0x11) number of root entries
	u16	sectnum16;	// (0x13) small total number of sectors (use sectnum32 if sectnum16 = 0)
	u8	media;		// (0x15) media descriptor
	u16	fatsize;	// (0x16) number of sectors per FAT
	u16	tracksize;	// (0x18) BIOS number of sectors per track
	u16	headnum;	// (0x1A) BIOS number of heads
	u32	hiddensect;	// (0x1C) number of hidden sectors (=start offset of partition)
	u32	sectnum32;	// (0x20) big total number of sectors (if sectnum16 = 0)
	u8	drive;		// (0x24) BIOS drive number (0x80 = 1st harddisk)
	u8	lock;		// (0x25) 1=lock flag during write to disk
	u8	extbootsig;	// (0x26) extended boot signature EXTSIG (0x29)
	u32	serial;		// (0x27) disk serial number
	char	label[11];	// (0x2B) volume label
	char	filesys[8];	// (0x36) name of file system (FAT12, FAT16, FAT)
	u8	bootcode[448];	// (0x3E) boot code
	u16	bootsig;	// (0x1FE) boot signature BOOTSIG (0xAA55)
} sBoot16;

// boot sector structure - FAT32 (512 bytes)
typedef struct __attribute__ ((packed)) {
	u8	jmp[3];		// (0x00) jmp instruction
	char	oem[8];		// (0x03) OEM name
	u16	sectsize;	// (0x0B) sector size (in bytes)
	u8	clustsize;	// (0x0D) cluster size (in sectors)
	u16	reserved;	// (0x0E) reserved sectors - number of boot sectors	
	u8	fatnum;		// (0x10) number of FAT tables (1 or 2)
	u16	rootent;	// (0x11) number of root entries
	u16	sectnum16;	// (0x13) small total number of sectors (use sectnum32 if sectnum16 = 0)
	u8	media;		// (0x15) media descriptor
	u16	fatsize16;	// (0x16) small number of sectors per FAT (use fatsize32 if fatsize16 = 0)
	u16	tracksize;	// (0x18) BIOS number of sectors per track
	u16	headnum;	// (0x1A) BIOS number of heads
	u32	hiddensect;	// (0x1C) number of hidden sectors (=start offset of partition)
	u32	sectnum32;	// (0x20) big total number of sectors (if sectnum16 = 0)
	u32	fatsize32;	// (0x24) big number of sectors per FAT (if fatsize16 = 0)
	u16	flags;		// (0x28) extended flags
	u16	ver;		// (0x2A) filesystem version
	u32	rootclust;	// (0x2C) root directory cluster
	u16	fsinfo;		// (0x30) offset of FSINFO sector
	u16	backup;		// (0x32) offset of backup boot sector
	u8	res[12];	// (0x34) ...reserved
	u8	drive;		// (0x40) BIOS drive number (0x80 = 1st harddisk)
	u8	lock;		// (0x41) 1=lock flag during write to disk
	u8	extbootsig;	// (0x42) extended boot signature EXTSIG (0x29)
	u32	serial;		// (0x43) disk serial number
	char	label[11];	// (0x47) volume label
	char	filesys[8];	// (0x52) name of file system (FAT32)
	u8	bootcode[420];	// (0x5A) boot code
	u16	bootsig;	// (0x1FE) boot signature BOOTSIG (0xAA55)
} sBoot32;

// FSinfo (= sector 1 immediately after boot record) (512 bytes)
typedef struct __attribute__ ((packed)) {
	u32	leadsig;	// (0x00) leading signature LEADSIG (0x41615252 = "RRaA")
	u8	res[480];	// (0x04) reserved
	u32	strucsig;	// (0x1E4) structure signature STRUCSIG (0x61417272 = "rrAa")
	u32	freeclust;	// (0x1E8) number of free clusters
	u32	lastclust;	// (0x1EC) last allocated cluster
	u8	res2[14];	// (0x1F0) reserved
	u16	bootsig;	// (0x1FE) boot signature BOOTSIG (0xAA55)
} sFSinfo;

#define DIR_DEL		0xE5	// deleted directory entry mark in name[0]
#define DIR_FREE	0	// free directory entry mark in name[0]

// file attributes
#define	ATTR_RO		B0	// Read only
#define	ATTR_HID	B1	// Hidden
#define	ATTR_SYS	B2	// System
#define ATTR_VOL	B3	// Volume label
#define ATTR_DIR	B4	// Directory
#define ATTR_ARCH	B5	// Archive

#define ATTR_MODI	B6	// internal: file was modified
#define ATTR_DEF	B7	// internal: used default directory

// disk manager
#define ATTR_SEL	B7	// internal, disk manager: selected file

#define ATTR_NORM	0	// normal attributes
#define ATTR_MASK	0x3F	// mask of valid bits
#define ATTR_FILE_MASK	(ATTR_RO|ATTR_HID|ATTR_SYS|ATTR_ARCH) // mask of valid bits of files
#define ATTR_DIR_MASK	(ATTR_RO|ATTR_HID|ATTR_SYS|ATTR_DIR|ATTR_ARCH) // mask of valid bits of directories

#define ATTR_NONE	0xFF	// flag - file not found
#define ATTR_DEFDIR	(ATTR_DIR|ATTR_DEF) // flag - use default directory

// FAT directory entry (32 bytes)
typedef struct __attribute__ ((packed)) {
	char	name[11];	// (0x00) file name
	u8	attr;		// (0x0B) attributes (ATTR_RO,...)
	u8	ntres;		// (0x0C) lower case flag (should be 0 = no EAs)
	u8	ctime10;	// (0x0D) created time sub-second in 10 ms, value 0..199, or first character of deleted file
	u16	ctime;		// (0x0E) created time in DOS format (b0-4: seconds/2, b5-10: minutes, b11-15: hours)
	u16	cdate;		// (0x10) created date in DOS format (b0-4: day, b5-8: month, b9-15: year - 1980)
	u16	adate;		// (0x12) last accessed date in DOS format (b0-4: day, b5-8: month, b9-15: year - 1980)
	u16	clustH;		// (0x14) higher 16 bits of first cluster (FAT32)
	u16	wtime;		// (0x16) last write time in DOS format (b0-4: seconds/2, b5-10: minutes, b11-15: hours)
	u16	wdate;		// (0x18) last write date in DOS format (b0-4: day, b5-8: month, b9-15: year - 1980)
	u16	clustL;		// (0x1A) lower 16 bits of first cluster
	u32	size;		// (0x1C) file size in bytes
} sDir;

// open file/directory descriptor (39 bytes)
typedef struct {
	char	name[11];	// short file name (0=not open)
	u8	attr;		// attributes (ATTR_RO,..., ATTR_NONE=file not exist)
	sDir*	dir;		// pointer to directory entry in disk buffer
	u32	dirsect;	// sector with directory entry
	u16	wtime;		// last write time
	u16	wdate;		// last write date
	u32	size;		// file size
	u32	sclust;		// start cluster
	u32	off;		// current read/write offset
	u32	clust;		// current read/write cluster
	u32	sect;		// current read/write sector (0=end of directory)
} sFile;

// FILINFO file info (24 bytes)
typedef struct {
	u32	size;		// file size
	u16	wtime;		// last write time
	u16	wdate;		// last write date
	char	name[12+1];	// file name (without separating spaces, with extension and with '.')
	u8	namelen;	// length of file name
	u8	attr;		// file attributes
	u8	res;		// ... align
} sFileInfo;

// file system type
enum {
	FS_NONE = 0,	// invalid file system
	FS_FAT12 = 1,	// FAT12
	FS_FAT16 = 2,	// FAT16
	FS_FAT32 = 3,	// FAT32

	FS_NUM
};

// file system info (FSinfo flags)
#define FSI_DIRTY B0	// FSinfo dirty, need to save
#define FSI_DISABLE B1	// FSinfo disabled

// disk buffer
extern ALIGNED u8 DiskBuf[SECT_SIZE];

// current sector in disk buffer (SECT_NONE = none)
#define SECT_NONE (~0UL) // none sector
extern u32 DiskBufSect;

// dirty flag - sector in disk buffer is modified and should be saved
extern Bool DiskBufDirty;

// disk FAT info (valid if DiskFS != FS_NONE)
extern u8 DiskFS;		// file system type (FS_FAT12,..)
extern u8 FSinfo;		// FSinfo flag (FSI_DISABLE,...)
extern u8 FatNum;		// number of FATs (1 or 2)
extern u8 ClustSizeSect;	// cluster size in number of sectors (must be power of 2)
extern u16 RootNum;		// number of root directory entries (only FAT12 or FAT16; 0 on FAT32)
extern u32 DiskSizeSect;	// disk size in number of sectors (including system tables)
extern u32 ClustSize;		// cluster size in bytes
extern u32 ClustLast;		// last allocated cluster (SECT_NONE = unknown)
extern u32 ClustFree;		// number of free clusters (SECT_NONE = unknown)
extern u32 ClustNum;		// number of clusters (= number of FAT entries - 2)
extern u32 FatEntry;		// number of FAT entries (= number of clusters + 2)
extern u32 FatSizeSect;		// size of FAT (number of sectors per one FAT table)
extern u32 DiskBase;		// volume base sector (= boot base sector)
extern u32 FatBase;		// FAT base sector
extern u32 DirBase;		// root directory base sector (FAT12, FAT16) or cluster (FAT32)
extern u32 DataBase;		// data base sector
extern u32 CurDirClust;		// current directory start cluster (0 = root)
extern u32 DiskSerial;		// disk serial number (0 = invalid)

// working directory
extern sFile DirTmp;		// working directory

// file info
extern sFileInfo FileInfoTmp;	// file info

// --- application info header
#define APPINFO_MAGIC	0x44415050	// application info header magic (= text "PPAD")
#define APPINFO_MAGIC2	0x64617070	// application info header magic (= text "ppad")

#if RP2040

// RP2040
typedef struct {
	u32	magic;		// app[48]: identification magic mark APPINFO_MAGIC (= text "PPAD")
	u32	len;		// app[49]: application length without vector table and without this header
	u32	crc;		// app[50]: Crc32ADMA checmsum of the application (without vector table and without this header)
	u8	data[0];	// app[51]: start of application data, to calculate CRC
} sAppInfo;

STATIC_ASSERT(sizeof(sAppInfo) == 12, "Incorrect sAppInfo!");

// pointer to application info header (RP2040: vector table contains 16+32=48 u32 vectors)
#define APPINFO  ((const sAppInfo*)(XIP_BASE + BOOTLOADER_SIZE + 48*4))

#else

// RP2350
typedef struct {
	u32	magic;		// app[68] or app[192]: identification magic mark APPINFO_MAGIC (= text "PPAD")
	u32	len;		// app[69] or app[193]: application length without vector table and without this header
	u32	crc;		// app[70] or app[194]: Crc32ADMA checmsum of the application (without vector table and without this header)
	union	{
		u8	data[0];	// start of application data, to calculate CRC
		u32	stack;		// app[71] or app[195]: start stack ...start of program data
	};
	u32	reset;		// app[72] or app[196]: start address
	u32	magic2;		// app[73] or app[197]: identification magic mark APPINFO_MAGIC2 (= text "ppad")
} sAppInfo;

STATIC_ASSERT(sizeof(sAppInfo) == 24, "Incorrect sAppInfo!");

#if RISCV
// pointer to application info header (RP2350 RISC-V: vector table contains 192 u32 words)
#define APPINFO  ((const sAppInfo*)(XIP_BASE + BOOTLOADER_SIZE + 192*4))
#else
// pointer to application info header (RP2350 ARM: vector table contains 16+52=68 u32 vectors)
#define APPINFO  ((const sAppInfo*)(XIP_BASE + BOOTLOADER_SIZE + 68*4))
#endif

#endif

// pointer to start of application (pointer to start of vector table)
#define APPSTART ((const u32*)(XIP_BASE + BOOTLOADER_SIZE))

// --- boot loader data
// This structure is located at the beginning of the scratch-Y segment of
// RAM memory, at address 0x20041000 (under the CPU core 1 stack). It contains
// information about the last selected file and the path, as it is set by the
// boot loader. It is used to return the cursor to its original position after
// program exit.

typedef struct {
	u8	lastnamelen;	// 0x00: length of name of selected file
	u8	lastnamedir;	// 0x01: ATTR_DIR attribute of the selected file
	u8	lastname[8];	// 0x02: name of last selected file (without terminating 0)
	u8	res, res2;	// 0x0A: ... reserved (align), contains 0
	u32	curdirclust;	// 0x0C: cluster of current directory
	u32	filetop;	// 0x10: index of first file in file window
	u32	crc;		// 0x14: CRC32ADMA checksum of first 20 bytes of this structure
} sLoaderData;

STATIC_ASSERT(sizeof(sLoaderData) == 24, "Incorrect sLoaderData!");

// boot loader resident segment
extern u8 __attribute__((section(".bootloaderdata"))) LoaderData[BOOTLOADER_DATA];
#define LOADERDATA ((sLoaderData*)LoaderData) // pointer to loader data

// --- application home path
// This structure is located in next 256-byte page in Flash after application end.

#define APPPATH_MAGIC	'P'	// magic byte
#define APPPATH_PATHMAX	(256 - 8 - 1) // max. length of the path, without terminating 0 (= 247)

typedef struct {
	u32	crc;		// 0x00: CRC32ADMA checksum of following data and path with terminating 0
	u8	magic;		// 0x04: magic byte APPPATH_MAGIC
	u8	pathlen;	// 0x05: length of the path (without terminating 0)
	u8	res, res2;	// 0x06: ... reserved (align), contains 0xff
	char	path[APPPATH_PATHMAX+1]; // 0x08: path with terminating 0 ... 0xff data fill rest of buffer
} sAppPath;

STATIC_ASSERT(sizeof(sAppPath) == 256, "Incorrect sAppPath!");

// --- public functions

// check if cluster is valid
Bool Disk_ClustValid(u32 clust);

// compose DOS date and DOS time
INLINE u16 DosDate(u8 d, u8 m, u16 y) { return d + (m << 5) + ((y - 1980) << 9); }
INLINE u16 DosTime(u8 h, u8 m, u8 s) { return (s/2) + (m << 5) + (h << 11); }

// day, month and year from DOS date
INLINE u8 DosDay(u16 d) { return d & 0x1f; }
INLINE u8 DosMon(u16 d) { return (d >> 5) & 0xf; }
INLINE u16 DosYear(u16 d) { return (d >> 9) + 1980; }

// second, minute and hour from DOS time
INLINE u8 DosSec(u16 t) { return (t & 0x1f) * 2; }
INLINE u8 DosMin(u16 t) { return (t >> 5) & 0x3f; }
INLINE u8 DosHour(u16 t) { return t >> 11; }

// default FAT date and time 1/1/2000 12:00
#define FATDATE() (1 + (1 << 5) + ((2000 - 1980) << 9))
#define FATTIME() (0 + (0 << 5) + (12 << 11))

// unmount disk
void DiskUnmount();

// mount disk (returns False on error)
Bool DiskMount();

// check if disk is mounted
INLINE Bool DiskMounted() { return DiskFS != FS_NONE; }

// mount disk if not mounted
Bool DiskAutoMount();

// flush disk write buffers (should be called repeatedly after some time)
Bool DiskFlush();

// convert character to uppercase
char FileUpperCase(char ch);

// convert character to lowercase
char FileLowerCase(char ch);

// get file system name (in ROM)
const char* DiskFATName();

// get disk FAT type (0=invalid, 1=FAT12, 2=FAT16, 3=FAT32)
INLINE u8 DiskFATType() { return DiskFS; }

// get number of FATs (1 or 2)
INLINE u8 DiskFATNum() { return FatNum; }

// get number of sectors per cluster
INLINE u8 DiskClustSect() { return ClustSizeSect; }

// get number of root directory entries (only FAT12 or FAT16; 0 on FAT32)
INLINE u16 DiskRootNum() { return RootNum; }

// get total disk size in number of sectors (including system tables)
INLINE u32 DiskTotalSect() { return DiskSizeSect; }

// get size of cluster in bytes
INLINE u32 DiskClustSize() { return ClustSize; }

// get last allocated cluster (-1=uknown)
INLINE u32 DiskLastClust() { return ClustLast; }

// get number of free clusters (returns -1 on error)
u32 DiskFreeClust();

// get size of free space in KB (returns -1 on error)
u32 DiskFreeKB();

// get total number of clusters (only data, without system tables)
INLINE u32 DiskTotalClust() { return ClustNum; }

// get total size in KB (only data, without system tables)
u32 DiskTotalKB();

// get number of used clusters (returns -1 on error)
u32 DiskUsedClust();

// get size of used space in KB (returns -1 on error)
u32 DiskUsedKB();

// number of FAT entries
INLINE u32 DiskFatEntry() { return FatEntry; }

// size of FAT (number of sectors per one FAT table)
INLINE u32 DiskFatSize() { return FatSizeSect; }

// volume base sector
INLINE u32 DiskVolBase() { return DiskBase; }

// FAT base sector
INLINE u32 DiskFatBase() { return FatBase; }

// root directory base sector (FAT12, FAT16) or cluster (FAT32)
INLINE u32 DiskDirBase() { return DirBase; }

// data base sector
INLINE u32 DiskDataBase() { return DataBase; }

// current directory start cluster (0=root)
INLINE u32 DiskDirClust() { return CurDirClust; }

// initialize file structure sFile (set as not-open)
void FileInit(sFile* file);

// check if file is open
Bool FileIsOpen(sFile* file);

// create new file (returns False on error, and name[0] = 0 on error)
Bool FileCreate(sFile* file, const char* path);

// open existing file (returns False on error, and name[0] = 0 on error)
Bool FileOpen(sFile* file, const char* path);

// read file (returns number of bytes read, or less on error)
u32 FileRead(sFile* file, void* buf, u32 num);

// write file (returns number of bytes write, or less on error)
u32 FileWrite(sFile* file, const void* buf, u32 num);

// print character to file
void FilePrintChar(sFile* file, char ch);

// print unformatted text to file (returns number of characters)
u32 FilePrintText(sFile* file, const char* txt);

#if USE_STREAM	// use Data stream (lib_stream.c, lib_stream.h)
// formatted print string to file, with argument list (returns number of characters)
u32 FilePrintArg(sFile* file, const char* fmt, va_list args);

// formatted print string to file, with variadic arguments (returns number of characters)
NOINLINE u32 FilePrint(sFile* file, const char* fmt, ...);
#endif // USE_STREAM

// flush file writes and flush disk buffers (returns False on error)
Bool FileFlush(sFile* file);

// close file and flush disk buffers (returns False on error)
Bool FileClose(sFile* file);

// change current directory (returns False on error)
Bool SetDir(const char* path);

// get current directory (returns length of path, or 0 on error)
u16 GetDir(char* buf, u16 len);

// seek file read/write pointer (returns False on error)
Bool FileSeek(sFile* file, u32 off);

// seek file read/write pointer to end of the file (returns False on error)
INLINE Bool FileSeekEnd(sFile* file) { return FileSeek(file, file->size); }

// get current seek position of open file
INLINE u32 FilePos(sFile* file) { return file->off; }

// get size of open file
INLINE u32 FileSize(sFile* file) { return file->size; }

// get attributes of open file (ATTR_RO,...)
INLINE u8 FileAttr(sFile* file) { return file->attr; }

// get last write time of open file (in DOS format)
INLINE u16 FileWTime(sFile* file) { return file->wtime; }

// get last write date of open file (in DOS format)
INLINE u16 FileWDate(sFile* file) { return file->wdate; }

// open search files (returns False on error, and name[0] = 0 on error)
//  - path = path to directory (without search pattern)
//  - searching can be reopen (without close) to rewind search from begin
Bool FindOpen(sFile* find, const char* path);

// find next file (returns False on error or end of directory)
//  attr = attributes to search more to normal attributes (use ATTR_DIR_MASK to search all except volumes and long filenames)
//  pat = ASCIIZ pattern (?=one character matching, *=rest matching)
Bool FindNext(sFile* find, sFileInfo* fileinfo, u8 attr, const char* pat);

// close file searching (returns False on error) - not needed to use
Bool FindClose(sFile* find);

// check if file/directory exists (returns False on error or if not exist)
Bool FileExist(const char* path);

// get file/directory info (returns False on error)
Bool FileInfo(const char* path, sFileInfo* fileinfo);

// set file size (truncate or enlarge; returns False on error)
Bool SetFileSize(sFile* file, u32 size);

// delete file/directory (directory must be empty; returns False on error)
Bool FileDelete(const char* path);

// create directory (returns False on error)
Bool DirCreate(const char* path);

// rename/move file/directory (returns False on error)
Bool FileMove(const char* oldpath, const char* newpath);

// get file size (returns SECT_NONE on error)
u32 GetFileSize(const char* path);

// get file/directory attributes (returns ATTR_NONE on error)
u8 GetFileAttr(const char* path);

// set file/directory attributes (cannot change DIR or VOL attributes - bits ignored; returns False on error)
Bool SetFileAttr(const char* path, u8 attr);

// get file/directory write time (returns (u16)-1 on error)
u16 GetFileTime(const char* path);

// get file/directory write date (returns (u16)-1 on error)
u16 GetFileDate(const char* path);

// set file/directory write time (returns False on error)
Bool SetFileTime(const char* path, u16 time);

// set file/directory write date (returns False on error)
Bool SetFileDate(const char* path, u16 date);

// get disk serial number (0 = invalid)
INLINE u32 GetDiskSerial() { return DiskSerial; }

// get disk volume label (returns label in FileInfoTmp; returns False on error or if label does not exists - sets empty label)
Bool GetDiskLabel();

// set disk volume label (empty = delete volume label; returns False on error)
Bool SetDiskLabel(const char* label);

// get media size in number of sectors (returns 0 on error)
INLINE u32 GetMediaSize() { return SDMediaSize(); }

// get media size in KB (returns 0 on error)
INLINE u32 GetMediaSizeKB() { return GetMediaSize() >> 1; }

#define FORMAT_MAGIC 0xBAB00CA
// SD card format (returns False on error)
//  fs = filesystem FS_FAT12,... (FS_NONE = auto)
//  clust = cluster size in number of sectors 0,1,2,4,8,16,32,64,128 (must be power of 2, or 0 = auto)
//  mbr = use MBR
//  magic = FORMAT_MAGIC
Bool DiskFormat(u8 fs, u8 clust, Bool mbr, u32 magic);

// SD card format with recommended settings
INLINE Bool DiskFormatDef(u32 magic) { return DiskFormat(FS_NONE, 0, True, magic); }

// check file extension
Bool FileCheckExt(sFileInfo* fi, const char* ext);

// check application in Flash memory (application starts at address APPSTART = XIP_BASE + BOOTLOADER_SIZE)
//   applen ... pointer to get application length, without header (NULL = not needed)
//   proglen ... pointer to get total program length, with boot loader and with header (NULL = not needed)
//   appcrc ... pointer to get application CRC (NULL = not needed)
Bool CheckApp(u32* applen, u32* proglen, u32* appcrc);

// check boot loader data (boot loader data are at address LOADERDATA)
Bool CheckLoaderData();

// check application home path (returns pointer to sAppPath, or NULL on error)
const sAppPath* CheckAppPath();

// get application home path
//   path ... buffer of size APPPATH_PATHMAX+1 (= 248) to get path with terminating 0
//   def ... default path (used on error)
// Sets home path as current directory. Returns length of the path.
int GetHomePath(char* path, const char* def);
                                             
#ifdef __cplusplus
}
#endif

#endif // _LIB_FAT_H

#endif // USE_FAT	// use FAT file system (fat.c, fat.h)
