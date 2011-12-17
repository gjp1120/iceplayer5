/*
    char  *hostname = "www.freebsd.org";
    char  *filename = "/layout/images/logo-red.png";
    char  *savepath = "/home/ice/";

    download(hostname,filename,savepath);
*/
#ifndef __DOWNLOAD_H
#define __DOWNLOAD_H

#ifdef __cplusplus
extern "C" {
#endif

gboolean download(const char *hostname,char *filename,const char *savepath,char *error);

#ifdef __cplusplus
};
#endif


#endif //__DOWNLOAD_H
