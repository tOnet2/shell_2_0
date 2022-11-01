#include <stdlib.h>
#include "headers/mk_full_hist_file_path.h"

char *create_hist_dir_path (const char *dir)
{
	char *home_dir = getenv("HOME");
	int32_t dir_size, home_size;
	dir_size = home_size ^= home_size;
	while (dir[dir_size++]);
	while (home_dir[home_size++]);
	dir_size--;
	home_size--;
	char *home_plus_dir = (char*)malloc(dir_size + home_size + 3);
	char *ret_home_plus_dir = home_plus_dir;
	for (; *home_dir;)
		*home_plus_dir++ = *home_dir++;
	*home_plus_dir++ = '/';
	for (; *dir;)
		*home_plus_dir++ = *dir++;
	*home_plus_dir++ = '/';
	*home_plus_dir = 0;
	return ret_home_plus_dir;
}

char *create_full_hist_path (const char *fdir, const char *fname)
{
	int32_t fdir_size, fname_size;
	fdir_size = fname_size ^= fname_size;
	while(fdir[fdir_size++]);
	while(fname[fname_size++]);
	fdir_size--;
	fname_size--;
	char *ret_full_hist_path, *full_hist_path = (char*)malloc(fdir_size + fname_size + 1);
	ret_full_hist_path = full_hist_path;
	for (; *fdir;)
		*full_hist_path++ = *fdir++;
	for (; *fname;)
		*full_hist_path++ = *fname++;
	*full_hist_path = 0;
	return ret_full_hist_path;
}
