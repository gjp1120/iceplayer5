#include <gtk/gtk.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>

#include "configure.h"
#include "lrc.h"

/*
    gchar  *hostname = "www.freebsd.org";
    gchar  *filename = "/layout/images/logo-red.png";
    gchar  *savepath = "/home/ice/";

    download(hostname,filename,savepath);
*/
gboolean download(const char *hostname,
				  char *filename,
				  const char *savepath,
				  char *error)
{
    print_debug("连接网络...")
    print_debug(hostname);
    print_debug(filename);
    lrc_text[1][0]=0;
    ssize_t      nsize;
    //ssize_t      nwrite;
    ssize_t      nread;
    //ssize_t      nleft;
    char         sendbuf[4096];
    char         recvbuf[64];
    char         linebuf[1024];
    char        *pos;

    int          r;
    int          s;
    FILE        *fp;
    struct sockaddr_in addr;
    struct hostent  *hp;
    print_debug("socket..");
    lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_NET_SOCKET;
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        print_error("socket出错，无法连接网络。\n");
        strcpy(error,"socket出错，无法连接网络。");
        lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_NET_SOCKET_ERROR;
        return FALSE;
    }
    print_debug("socket!");
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    print_debug("dns..");
    lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_NET_DNS;
    hp = gethostbyname(hostname);
    if(hp==NULL) {
        print_error("无法解析DNS，无法连接网络。\n");
        strcpy(error,"无法解析DNS，无法连接网络。");
        lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_NET_DNS_ERROR;
        return FALSE;
    }
    print_debug("dns!");
    print_debug("inet_pton..");
    memcpy(&addr.sin_addr.s_addr, hp->h_addr_list[0], hp->h_length);
    if (!hp) {
        print_error("inet_pton出错，无法连接网络。\n");
        strcpy(error,"inet_pton出错，无法连接网络。");
        return FALSE;
    }
    print_debug("inet_pton!");

    inet_ntop(AF_INET, &addr.sin_addr, linebuf, sizeof(linebuf));
    //printf("%s\n", linebuf);
    print_debug("connect..");
    lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_NET_CONNECT;
    r = connect(s, (struct sockaddr *)&addr, sizeof(addr));
    if (r < 0) {
        print_error("connect出错，无法连接网络。\n");
        strcpy(error,"connect出错，无法连接网络。");
        lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_NET_CONNECT_ERROR;
        return FALSE;
    }
    print_debug("connect!");

    nsize = snprintf(sendbuf, sizeof(sendbuf),"%s %s %s\r\n""%s %s\r\n""%s %s\r\n""%s %s\r\n""\r\n","GET", filename, "HTTP/1.1","Host:", hostname,"Accept:", "*/*","Connection:", "close");
    if(write(s, sendbuf, nsize));

    pos = NULL;
    fp = NULL;
    while ((nread = read(s, recvbuf, sizeof(recvbuf) - 1)) > 0) {
        if (fp) {
            fwrite(recvbuf, 1, nread, fp);
        } else {
            pos = memchr(recvbuf, '\n', nread);
            while (pos) {
                if ((*(++pos) == '\r') && (*(++pos) == '\n'))
                    break;
                pos = memchr(pos, '\n', nread - (pos - recvbuf));
            }
            if (!pos) {
                recvbuf[nread] = 0;
                //printf("%s", recvbuf);
            } else {
                *pos++ = 0;
                //printf("%s\n", recvbuf);
                char path[2000];
                strcpy(path,savepath);
                fp = fopen(path, "wb");
                if(!fp) {
                    print_error("无法打开歌词文件以写入！%s\n",path);
                }
                fwrite(pos, 1, nread - (pos - recvbuf), fp);
            }
        }
    }
    if(fp)fclose(fp);
    return TRUE;
}
