/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <sys/stat.h>

#include <ttypes.h>
#include <tvutil.h>

int getcurdir(int , char *buffer)
{
  getcwd(buffer, PATH_MAX);
  return 0;
}


/*
 * Expands a path into its directory and file components.
 */

void expandPath(const char *path, char *dir, char *file)
{
  char *tag = strrchr(path, '/');

  if (tag != NULL)
  {
    if (file)
      strncpy(file, tag + 1,PATH_MAX);
    if (dir)
    {
      strncpy(dir, path, tag - path + 1);
      dir[tag - path + 1] = '\0';
    }
  }
  else
  {
    /* there is only the file name */
    if (file)
      strncpy(file, path,PATH_MAX);
    if (dir)
      dir[0] = '\0';
  }
}


void fexpand( char *rpath )
{
  char path[PATH_MAX];
  _fixpath(rpath,path);
  strncpy(rpath,path,PATH_MAX);
}

Boolean driveValid( char drive )
{
  if (drive) {};
  return False;
}

Boolean isDir( const char *str )
{
  struct stat s;
  
  if (stat(str, &s) == 0 && S_ISDIR(s.st_mode)) return True;
  return False;
}

Boolean pathValid( const char *path )
{
  char dir[PATH_MAX];
  char name[PATH_MAX];
  
  expandPath(path, dir, name);
  if (*dir && dir[strlen(dir)-1] == '/') strncat(dir, ".",PATH_MAX-1-strlen(dir));
  else strncat(dir, "/.",PATH_MAX-1-strlen(dir));
  return isDir(dir);
}

Boolean validFileName( const char *fileName )
{
  static const char * const illegalChars = "<>|/\"";
  const char *slash = strrchr(fileName,'/');
  if (!slash)
  {
    if (strpbrk(fileName,illegalChars) != 0)
      return False;
    return True;
  }
  if (strpbrk(slash+1,illegalChars) != 0)
    return False;
  return pathValid(fileName);
}

void getCurDir( char *dir )
{
  getcwd(dir, PATH_MAX);
  if (dir[strlen(dir)-1] != '/')
    strncat(dir,"/",PATH_MAX-1-strlen(dir));
}

Boolean isWild( const char *f )
{
    return Boolean( strpbrk( f, "?*" ) != 0 );
}


/* 
   Here now the very usefull function _fixpath() from DJGPP's
   libc 'fixpath.c'
   I have modified it to be used on unix systems (like linux).
*/

/* Copyright (C) 1996 DJ Delorie, see COPYING.DJ for details */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */

__inline__ static int
is_slash(int c)
{
  return c == '/';
}

__inline__ static int
is_term(int c)
{
  return c == '/' || c == '\0';
}

/* Takes as input an arbitrary path.  Fixes up the path by:
   1. Removing consecutive slashes
   2. Removing trailing slashes
   3. Making the path absolute if it wasn't already
   4. Removing "." in the path
   5. Removing ".." entries in the path (and the directory above them)
 */
void
_fixpath(const char *in, char *out)
{
  const char	*ip = in;
  char		*op = out;

  /* Convert relative path to absolute */
  if (!is_slash(*ip))
  {
    getcurdir(0,op);
    op += strlen(op);
  }

  /* Step through the input path */
  while (*ip)
  {
    /* Skip input slashes */
    if (is_slash(*ip))
    {
      ip++;
      continue;
    }

    /* Skip "." and output nothing */
    if (*ip == '.' && is_term(*(ip + 1)))
    {
      ip++;
      continue;
    }

    /* Skip ".." and remove previous output directory */
    if (*ip == '.' && *(ip + 1) == '.' && is_term(*(ip + 2)))
    {
      ip += 2;
      /* Don't back up over root '/' */
      if (op > out )
      /* This requires "/" to follow drive spec */
	while (!is_slash(*--op));
      continue;
    }

    /* Copy path component from in to out */
    *op++ = '/';
    while (!is_term(*ip)) *op++ = *ip++;
  }

  /* If root directory, insert trailing slash */
  if (op == out) *op++ = '/';

  /* Null terminate the output */
  *op = '\0';

}

/*
  End of modified code from DJGPP's libc 'fixpath.c'
*/

int __file_exists(const char *fname)
{
  return (access(fname,R_OK) == 0);
}

