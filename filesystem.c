/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.
*/

// Name: Puru Pathak
//For compilation: gcc -Wall filesystem.c `pkg-config fuse --cflags --libs` -o filesystem
//For execution: sudo ./filesystem /tmp/myproc -o nonempty



#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <ctype.h>

static const char *hello_str = "Hello World!\n";
static const char *myprocPath = "/myproc";
#define BUFF_SIZE 100000

char* ProcDetails_read(const char *filename,char *source);
void get_Direct(const char *name, void *buf, fuse_fill_dir_t filler);
int id_check(const char *pchar_id);

static int filesystem_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	char source[BUFF_SIZE + 1];

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (strcmp(path, myprocPath) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(hello_str);

	// Modified by Puru Pathak	
	} else if (id_check(path)) {                  //Check if path is a process id
		stbuf->st_mode = S_IFREG | 0444;                //setting the mode
		stbuf->st_nlink = 1;
		stbuf->st_size = strlen(ProcDetails_read(path,source));      //get the size
	} 
	else
		res = -ENOENT;

	return res;
}

static int filesystem_open(const char *path, struct fuse_file_info *fi)
{

	//Modified by Puru Pathak
	if (!id_check(path + 1)) return -ENOENT;            //Check if the path name is a process id

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int filesystem_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		off_t offset, struct fuse_file_info *fi)
{

	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

//Modified by Puru Pathak
	char *proc_path = "/proc/";            //Used to list the process
	get_Direct(proc_path, buf, filler);     //Put into the buffer

	return 0;
}

static int filesystem_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;

	if(id_check(path + 1)) 
	{
		char source[BUFF_SIZE + 1];
		const char *p = ProcDetails_read(path,source);

		len = strlen(p);

		if (offset < len) {
			if (offset + size > len)
				size = len - offset;
			memcpy(buf, p + offset, size);
		} 
		else {
			size = 0;
		}

		return size;
	}
		return 0;
}

static struct fuse_operations filesystem_oper = {
	.getattr	= filesystem_getattr,
	.readdir	= filesystem_readdir,
	.open		= filesystem_open,
	.read		= filesystem_read,
};

// Written by Puru Pathak
void get_Direct(const char *name,void *buf, fuse_fill_dir_t filler)  //Getting all the process directories
{
	char path[1024];
	DIR *dir;
	struct dirent *source_f;

	if (!(dir = opendir(name))) // Open the directory
		return;
	if (!(source_f = readdir(dir))) //Read the file
		return;

	do {
		if (source_f->d_type == DT_DIR) 
		{
			int len = snprintf(path, sizeof(path)-1, "%s/%s", name, source_f->d_name);
			path[len] = 0;
			if (strcmp(source_f->d_name, ".") == 0 || strcmp(source_f->d_name, "..") == 0) //Checking current or previous directory
				continue;
			if (id_check(source_f->d_name))  
			{
				filler(buf, source_f->d_name, NULL, 0);   //Adding id to buffer
			}}
	  } while ((source_f = readdir(dir)));
	closedir(dir);
}

// Written by Puru Pathak
int id_check(const char *pchar_id)   //Check if the path contains pid by using a character pointer
{
	while(*pchar_id) 
	{
	if (!isdigit(*pchar_id) && *pchar_id != '/')    //Checking if the id is numeric
		{
			return 0;}
	else
		{ 
			pchar_id++;}
	}
	return 1;
}

//Written by Puru Pathak
char* ProcDetails_read(const char *filename,char *source) 
{
	char f_path[100] = "";
	const char *partition = "/";
	const char *path = "/proc";
	size_t len;
	const char *destination_f = "status";
	
	FILE *fp = NULL;
	strcat(f_path, path);                               //Making the file name using string concatenation
	strcat(f_path, filename);
	strcat(f_path, partition);
	strcat(f_path, destination_f);

	fp = fopen(f_path, "r");                                        //Opening the file
	if (fp != NULL) 
	{
	  len = fread(source, sizeof(char), BUFF_SIZE, fp); //Read data from the stream
		if (len != 0) 
		{
    	   len++;
		   source[len] = '\0'; 
		}
		else 
		{
			fputs("Error in file reading: Recheck", stderr);             //Throw error
		}
		fclose(fp);
	}
	return source;
}

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &filesystem_oper, NULL);
}

