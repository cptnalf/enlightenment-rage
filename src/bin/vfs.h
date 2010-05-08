/* filename: vfs.h
 *  chiefengineer
 *  Fri May 07 23:11:02 PDT 2010
 */

#ifndef VFS_H
#define VFS_H

typedef struct _Rage_Vfs_File Rage_Vfs_File;

struct _Rage_Vfs_File
{
	char* uri;
	char* label;
	char* icon;
	char* icon_file;
	unsigned char is_directory : 1;
	unsigned char is_menu : 1;
};



#endif
