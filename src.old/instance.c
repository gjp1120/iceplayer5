#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <dirent.h>
#include <glib.h>
#include <iconv.h>
#include <gdk/gdk.h>
#include <gdk/gdkwindow.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <glib/gtypes.h>
#include <gtk/gtkaccelgroup.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "core.h"
#include "single.h"
#include "list.h"
#include "instance.h"

typedef void (*BaconMessageReceivedFunc) (const gchar *message,gpointer user_data);
typedef struct BaconMessageConnection BaconMessageConnection;

struct BaconMessageConnection {
    gboolean is_server;
    gchar *path;
    int fd;
    GIOChannel *chan;
    int conn_id;
    GSList *accepted_connections;
    void (*func) (const gchar *message, gpointer user_data);
    gpointer data;
};

void bacon_message_connection_free (BaconMessageConnection *conn) ;
BaconMessageConnection * bacon_message_connection_new (const gchar *prefix);
void bacon_message_connection_set_callback (BaconMessageConnection *conn, BaconMessageReceivedFunc func, gpointer user_data);
void bacon_message_connection_send (BaconMessageConnection *conn, const gchar *message);
gboolean bacon_message_connection_get_is_server (BaconMessageConnection *conn);


gboolean add_and_jump_to_song_and_play(gchar *argv) {
    g_print("Debug:传来数据是音乐文件:%s\n",argv);
    GSList *files=(GSList *)malloc(sizeof(GSList));
    files->data=argv;
    files->next = NULL;
    add_file(files, focus_lists);

    iceplayer_songs *songs=find_lists_from_name(focus_lists)->songs;
    gint num=0;
    while(songs&&songs->uri[0]) {
        songs=songs->next;
        num++;
    }
    strcpy(focus_song,argv);
    sprintf(playing_song,"%d",num-1);
    strcpy(playing_list,focus_lists);
    play_uri();
    show_songs(NULL);
    show_lists(NULL);
    list_menu_jump_to_playing_song(NULL);
    return FALSE;
}
void BaconMessageReceived(gchar *message) {
    static gchar argv[STRINGS_LENGTH];
    strcpy(argv,message);
    g_print("Debug:Rec Data:%s\n",argv);
    if(strcmp(argv,"-raise")==0) { //客户端上传有数据的
        g_print("Debug:数据为\"-raise\"，显示主窗口\n");
        show();
        return ;
    }
    if(was_music_file(argv)) {
        add_and_jump_to_song_and_play(argv);
    }
}
void instance_init(char *argv) {
    BaconMessageConnection *message_conn;
    gchar *connection_name = "iceplayer";
    BaconMessageConnection *bacon_message_connection_new (const gchar *prefix);
    message_conn = bacon_message_connection_new (connection_name);
    if(argv==NULL) {
        argv="-raise";
    }
    if (!bacon_message_connection_get_is_server (message_conn)) {
        g_print("Debug:Send Data:%s\n",argv);
        bacon_message_connection_send (message_conn, argv);
        printf("\nATTENTION!\niceplayer 检测到有其他iceplayer进程在运行，正常情况下我会尝试将另一个iceplayer窗口显示出来，失败？首先请确认没有其他iceplayer进程在运行，可以运行命令 pkill iceplayer ，再将/tmp/下的所有 iceplayer 开头的文件删除,再开启一次,祝你使用愉快。\n");
        exit(0);
    }
    if(was_music_file(argv)) {
        static gchar message[STRINGS_LENGTH];
        strcpy(message,argv);
        g_timeout_add(1000,G_SOURCE_FUNC(add_and_jump_to_song_and_play),message);
    }
    bacon_message_connection_set_callback (message_conn,(BaconMessageReceivedFunc) BaconMessageReceived,NULL);//view为自定义显示窗口函数
}
static gboolean test_is_socket (const gchar *path) {
    struct stat s;
    if (stat (path, &s) == -1)
        return FALSE;
    if (S_ISSOCK (s.st_mode))
        return TRUE;
    return FALSE;
}
static gboolean is_owned_by_user_and_socket (const gchar *path) {
    struct stat s;
    if (stat (path, &s) == -1)
        return FALSE;
    if (s.st_uid != geteuid ())
        return FALSE;
    if ((s.st_mode & S_IFSOCK) != S_IFSOCK)
        return FALSE;
    return TRUE;
}
void bacon_message_connection_free (BaconMessageConnection *conn) {
    GSList *child_conn;

    g_return_if_fail (conn != NULL);
    g_return_if_fail (conn->is_server != FALSE ||conn->accepted_connections == NULL);

    child_conn = conn->accepted_connections;
    while (child_conn != NULL) {
        bacon_message_connection_free (child_conn->data);
        child_conn = g_slist_next (child_conn);
    }
    g_slist_free (conn->accepted_connections);

    if (conn->conn_id) {
        g_source_remove (conn->conn_id);
        conn->conn_id = 0;
    }
    if (conn->chan) {
        g_io_channel_shutdown (conn->chan, FALSE, NULL);
        g_io_channel_unref (conn->chan);
    }

    if (conn->is_server != FALSE) {
        unlink (conn->path);
    }
    if (conn->fd != -1) {
        close (conn->fd);
    }

    g_free (conn->path);
    g_free (conn);
}
static gboolean server_cb (GIOChannel *source, GIOCondition condition, gpointer data);
static gboolean setup_connection (BaconMessageConnection *conn) {
    g_return_val_if_fail (conn->chan == NULL, FALSE);

    conn->chan = g_io_channel_unix_new (conn->fd);
    if (!conn->chan) {
        return FALSE;
    }
    g_io_channel_set_line_term (conn->chan, "\n", 1);
    conn->conn_id = g_io_add_watch (conn->chan, G_IO_IN, server_cb, conn);

    return TRUE;
}
static void accept_new_connection (BaconMessageConnection *server_conn) {
    BaconMessageConnection *conn;
    int alen;

    g_return_if_fail (server_conn->is_server);

    conn = g_new0 (BaconMessageConnection, 1);
    conn->is_server = FALSE;
    conn->func = server_conn->func;
    conn->data = server_conn->data;
    conn->fd = accept (server_conn->fd, NULL, (guint *)&alen);

    server_conn->accepted_connections =g_slist_prepend (server_conn->accepted_connections, conn);

    setup_connection (conn);
}
static gboolean server_cb (GIOChannel *source, GIOCondition condition, gpointer data) {
    BaconMessageConnection *conn = (BaconMessageConnection *)data;
    gchar *message, *subs, buf;
    int cd, rc, offset;
    gboolean finished;

    offset = 0;
    if (conn->is_server && conn->fd == g_io_channel_unix_get_fd (source)) {
        accept_new_connection (conn);
        return TRUE;
    }
    message = g_malloc (1);
    cd = conn->fd;
    rc = read (cd, &buf, 1);
    while (rc > 0 && buf != '\n') {
        message = g_realloc (message, rc + offset + 1);
        message[offset] = buf;
        offset = offset + rc;
        rc = read (cd, &buf, 1);
    }
    if (rc <= 0) {
        g_io_channel_shutdown (conn->chan, FALSE, NULL);
        g_io_channel_unref (conn->chan);
        conn->chan = NULL;
        close (conn->fd);
        conn->fd = -1;
        g_free (message);
        conn->conn_id = 0;

        return FALSE;
    }
    message[offset] = '\0';

    subs = message;
    finished = FALSE;

    while (finished == FALSE && *subs != '\0') {
        if (conn->func != NULL)
            (*conn->func) (subs, conn->data);

        subs += strlen (subs) + 1;
        if (subs - message >= offset)
            finished = TRUE;
    }

    g_free (message);

    return TRUE;
}
static gchar * find_file_with_pattern (const gchar *dir, const gchar *pattern) {
    GDir *filedir;
    gchar *found_filename;
    const gchar *filename;
    GPatternSpec *pat;

    filedir = g_dir_open (dir, 0, NULL);
    if (filedir == NULL)
        return NULL;

    pat = g_pattern_spec_new (pattern);
    if (pat == NULL) {
        g_dir_close (filedir);
        return NULL;
    }

    found_filename = NULL;

    while ((filename = g_dir_read_name (filedir))) {
        if (g_pattern_match_string (pat, filename)) {
            gchar *tmp = g_build_filename (dir, filename, NULL);
            if (is_owned_by_user_and_socket (tmp))
                found_filename = g_strdup (filename);
            g_free (tmp);
        }

        if (found_filename != NULL)
            break;
    }

    g_pattern_spec_free (pat);
    g_dir_close (filedir);

    return found_filename;
}
static gchar * socket_filename (const gchar *prefix) {
    gchar *pattern, *newfile, *path, *filename;
    const gchar *tmpdir;

    pattern = g_strdup_printf ("%s.%s.*", prefix, g_get_user_name ());
    tmpdir = g_get_tmp_dir ();
    filename = find_file_with_pattern (tmpdir, pattern);
    if (filename == NULL) {
        newfile = g_strdup_printf ("%s.%s.%u", prefix, g_get_user_name (), g_random_int ());
        path = g_build_filename (tmpdir, newfile, NULL);
        g_free (newfile);
    } else {
        path = g_build_filename (tmpdir, filename, NULL);
        g_free (filename);
    }

    g_free (pattern);
    return path;
}
static gboolean try_server (BaconMessageConnection *conn) {
    struct sockaddr_un uaddr;

    uaddr.sun_family = AF_UNIX;
    strncpy (uaddr.sun_path, conn->path,
             MIN (strlen(conn->path)+1, 1000));
    conn->fd = socket (PF_UNIX, SOCK_STREAM, 0);
    if (bind (conn->fd, (struct sockaddr *) &uaddr, sizeof (uaddr)) == -1) {
        conn->fd = -1;
        return FALSE;
    }
    listen (conn->fd, 5);

    if (!setup_connection (conn))
        return FALSE;
    return TRUE;
}
static gboolean try_client (BaconMessageConnection *conn) {
    struct sockaddr_un uaddr;

    uaddr.sun_family = AF_UNIX;
    strncpy (uaddr.sun_path, conn->path,
             MIN(strlen(conn->path)+1, 1000));
    conn->fd = socket (PF_UNIX, SOCK_STREAM, 0);
    if (connect (conn->fd, (struct sockaddr *) &uaddr,sizeof (uaddr)) == -1) {
        conn->fd = -1;
        return FALSE;
    }

    return setup_connection (conn);
}
BaconMessageConnection *
bacon_message_connection_new (const gchar *prefix) {
    BaconMessageConnection *conn;

    g_return_val_if_fail (prefix != NULL, NULL);

    conn = g_new0 (BaconMessageConnection, 1);
    conn->path = socket_filename (prefix);

    if (test_is_socket (conn->path) == FALSE) {
        if (!try_server (conn)) {
            bacon_message_connection_free (conn);
            return NULL;
        }

        conn->is_server = TRUE;
        return conn;
    }

    if (try_client (conn) == FALSE) {
        unlink (conn->path);
        try_server (conn);
        if (conn->fd == -1) {
            bacon_message_connection_free (conn);
            return NULL;
        }

        conn->is_server = TRUE;
        return conn;
    }

    conn->is_server = FALSE;
    return conn;
}
void bacon_message_connection_set_callback (BaconMessageConnection *conn, BaconMessageReceivedFunc func, gpointer user_data) {
    g_return_if_fail (conn != NULL);

    conn->func = func;
    conn->data = user_data;
}
void bacon_message_connection_send (BaconMessageConnection *conn, const gchar *message) {
    g_return_if_fail (conn != NULL);
    g_return_if_fail (message != NULL);

    g_io_channel_write_chars (conn->chan, message, strlen (message), NULL, NULL);
    g_io_channel_write_chars (conn->chan, "\n", 1, NULL, NULL);
    g_io_channel_flush (conn->chan, NULL);
}
gboolean bacon_message_connection_get_is_server (BaconMessageConnection *conn) {
    g_return_val_if_fail (conn != NULL, FALSE);

    return conn->is_server;
}


