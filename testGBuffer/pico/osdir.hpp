/* Copyright (C) 2003, 2004 Mihai Preda.
   See the file LICENSE for license information.
   http://procod.com/picostorage/
*/

#ifndef _OSDIR_HPP_
#define _OSDIR_HPP_


class OsFile;

class OsDir {
public:
    /*
      static const char kDirSep = '/';
      OsDir(const char *path);
      ~OsDir();
    */

    static OsFile openFile(const char *name);
    static OsFile createFile(const char *name);
    static bool remove(const char *name);

    /*
    OsDir openDir(const char *name);
    OsDir createDir(const char *name);
    */
    
private:
    static OsFile openFileHelper(const char *name, int mode);
};


#endif
