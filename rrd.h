#ifndef RRDLIB_H_4FD7D37D56A448C392AF46508C56D3CC
#define RRDLIB_H_4FD7D37D56A448C392AF46508C56D3CC

/*****************************************************************************
 * RRDtool 1.8.0 Copyright by Tobi Oetiker, 1997-2022
 *****************************************************************************
 * rrdlib.h   Public header file for librrd
 *****************************************************************************
 * $Id$
 * $Log$
 * Revision 1.9  2005/02/13 16:13:33  oetiker
 * let rrd_graph return the actual value range it picked ...
 * -- Henrik Stoerner <henrik@hswn.dk>
 *
 * Revision 1.8  2004/05/26 22:11:12  oetiker
 * reduce compiler warnings. Many small fixes. -- Mike Slifcak <slif@bellsouth.net>
 *
 * Revision 1.7  2003/11/12 22:14:26  oetiker
 * allow to pass an open filehandle into rrd_graph as an extra argument
 *
 * Revision 1.6  2003/11/11 19:46:21  oetiker
 * replaced time_value with rrd_time_value as MacOS X introduced a struct of that name in their standard headers
 *
 * Revision 1.5  2003/04/25 18:35:08  jake
 * Alternate update interface, updatev. Returns info about CDPs written to disk as result of update. Output format is similar to rrd_info, a hash of key-values.
 *
 * Revision 1.4  2003/04/01 22:52:23  jake
 * Fix Win32 build. VC++ 6.0 and 7.0 now use the thread-safe code.
 *
 * Revision 1.3  2003/02/13 07:05:27  oetiker
 * Find attached the patch I promised to send to you. Please note that there
 * are three new source files (src/rrd_is_thread_safe.h, src/rrd_thread_safe.c
 * and src/rrd_not_thread_safe.c) and the introduction of librrd_th. This
 * library is identical to librrd, but it contains support code for per-thread
 * global variables currently used for error information only. This is similar
 * to how errno per-thread variables are implemented.  librrd_th must be linked
 * alongside of libpthread
 *
 * There is also a new file "THREADS", holding some documentation.
 *
 * -- Peter Stamfest <peter@stamfest.at>
 *
 * Revision 1.2  2002/05/07 21:58:32  oetiker
 * new command rrdtool xport integrated
 * --  Wolfgang Schrimm <Wolfgang.Schrimm@urz.uni-heidelberg.de>
 *
 * Revision 1.1.1.1  2001/02/25 22:25:05  oetiker
 * checkin
 *
 *****************************************************************************/
#ifdef  __cplusplus
extern    "C" {
#endif

#include <sys/types.h>  /* for off_t */

#ifndef _WIN32
#include <unistd.h>     /* for off_t */
#endif

#ifdef _MSC_VER
#include <BaseTsd.h>
    typedef SSIZE_T ssize_t;
#ifndef PERLPATCHLEVEL
    typedef int mode_t;
#endif
#if _MSC_VER < 1800
#define strtoll _strtoi64
#endif
#endif

#include <time.h>
#include <stdio.h>      /* for FILE */
#include <string.h>

/* Formerly rrd_nan_inf.h */
#ifndef DNAN
# define DNAN rrd_set_to_DNAN()
#endif

/* declare opaque data structure, so we can use its pointers for type safety */

    struct rrd_t;

#ifndef DINF
# define DINF rrd_set_to_DINF()
#endif
    double    rrd_set_to_DNAN(
    void);
    double    rrd_set_to_DINF(
    void);
/* end of rrd_nan_inf.h */

/* Transplanted from rrd_format.h */
    typedef double rrd_value_t; /* the data storage type is
                                 * double */
/* END rrd_format.h */

/* information about an rrd file */
    typedef struct rrd_file_t {
        size_t    header_len;   /* length of the header of this rrd file */
        size_t    file_len; /* total size of the rrd file */
        size_t    pos;  /* current pos in file */
        void     *pvt;
        struct rrd_t *rrd;  /* the corresponding RRD structure, if any */
#ifdef HAVE_LIBRADOS
        struct rrd_rados_t *rados;
#endif
    } rrd_file_t;

/* information used for the conventional file access methods */
    typedef struct rrd_simple_file_t {
        int       fd;   /* file descriptor of this rrd file */
#ifdef HAVE_MMAP
        char     *file_start;   /* start address of an open rrd file */
        int       mm_prot;
        int       mm_flags;
#endif
    } rrd_simple_file_t;

/* rrd info interface */
    typedef struct rrd_blob_t {
        unsigned long size; /* size of the blob */
        unsigned char *ptr; /* pointer */
    } rrd_blob_t;

    typedef enum rrd_info_type { RD_I_VAL = 0,
        RD_I_CNT,
        RD_I_STR,
        RD_I_INT,
        RD_I_BLO
    } rrd_info_type_t;

    typedef union rrd_infoval {
        unsigned long u_cnt;
        rrd_value_t u_val;
        char     *u_str;
        int       u_int;
        rrd_blob_t u_blo;
    } rrd_infoval_t;

    typedef struct rrd_info_t {
        char     *key;
        rrd_info_type_t type;
        rrd_infoval_t value;
        struct rrd_info_t *next;
    } rrd_info_t;

    typedef size_t (
    *rrd_output_callback_t) (
    const void *,
    size_t,
    void *);

/* main function blocks */
    int       rrd_create(
    int,
    char **);
    rrd_info_t *rrd_info(
    int,
    char **);
    rrd_info_t *rrd_info_push(
    rrd_info_t *,
    char *,
    rrd_info_type_t,
    rrd_infoval_t);
    void      rrd_info_print(
    rrd_info_t *data);
    void      rrd_info_free(
    rrd_info_t *);
    char     *rrd_list(
    int,
    const char **);
    char     *rrd_list_r(
    int,
    const char *dirname);
    int       rrd_update(
    int,
    char **);
    rrd_info_t *rrd_update_v(
    int,
    const char **);
    int       rrd_graph(
    int,
    char **,
    char ***,
    int *,
    int *,
    FILE *,
    double *,
    double *);
    rrd_info_t *rrd_graph_v(
    int,
    char **);

    int       rrd_fetch(
    int,
    char **,
    time_t *,
    time_t *,
    unsigned long *,
    unsigned long *,
    char ***,
    rrd_value_t **);
    int       rrd_restore(
    int,
    char **);
    int       rrd_dump(
    int,
    const char **);
    int       rrd_tune(
    int,
    char **);
    time_t    rrd_last(
    int,
    const char **);
    int       rrd_lastupdate(
    int argc,
    const char **argv);
    time_t    rrd_first(
    int,
    const char **);
    int       rrd_resize(
    int,
    const char **);
    char     *rrd_strversion(
    void);
    double    rrd_version(
    void);
    int       rrd_xport(
    int,
    char **,
    int *,
    time_t *,
    time_t *,
    unsigned long *,
    unsigned long *,
    char ***,
    rrd_value_t **);
    int       rrd_flushcached(
    int argc,
    const char **argv);

    void      rrd_freemem(
    void *mem);

/* thread-safe (hopefully) */
    int       rrd_create_r(
    const char *filename,
    unsigned long pdp_step,
    time_t last_up,
    /* int no_overwrite, */
    int argc,
    const char **argv);
    int       rrd_create_r2(
    const char *filename,
    unsigned long pdp_step,
    time_t last_up,
    int no_overwrite,
    const char **sources,
    const char *_template,
    int argc,
    const char **argv);
    rrd_info_t *rrd_info_r(
    const char *);
/* NOTE: rrd_update_r and rrd_update_v_r are only thread-safe if no at-style
   time specifications get used!!! */

    int       rrd_update_r(
    const char *filename,
    const char *_template,
    int argc,
    const char **argv);
    int       rrd_update_v_r(
    const char *filename,
    const char *_template,
    int argc,
    const char **argv,
    rrd_info_t *pcdp_summary);

/* extra flags */
#define RRD_SKIP_PAST_UPDATES 0x01

/* Locking mode which can be set in 'extra_flags':
 *
 * DEFAULT ... read $RRD_LOCKING environment or fall back to TRY
 * NONE  ... no locking; caller is responsible to ensure that the file is not
 *           used else
 * BLOCK ... wait until lock is available
 * TRY   ... try to lock but fail when file is used elsewhere (default)
 */
#define RRD_FLAGS_LOCKING_MODE_NONE    (1 << 7)
#define RRD_FLAGS_LOCKING_MODE_DEFAULT (0 << 7)
#define RRD_FLAGS_LOCKING_MODE_BLOCK   (2 << 7)
#define RRD_FLAGS_LOCKING_MODE_TRY     (3 << 7)
#define RRD_FLAGS_LOCKING_MODE_MASK    (3 << 7)

    int       rrd_updatex_r(
    const char *filename,
    const char *_template,
    int extra_flags,
    int argc,
    const char **argv);
    int       rrd_updatex_v_r(
    const char *filename,
    const char *_template,
    int extra_flags,
    int argc,
    const char **argv,
    rrd_info_t *pcdp_summary);
    int       rrd_fetch_r(
    const char *filename,
    const char *cf,
    time_t *start,
    time_t *end,
    unsigned long *step,
    unsigned long *ds_cnt,
    char ***ds_namv,
    rrd_value_t **data);
    int       rrd_tune_r(
    const char *filename,
    int argc,
    const char **argv);
    int       rrd_dump_opt_r(
    const char *filename,
    const char *outname,
    int opt_noheader);
    int       rrd_dump_r(
    const char *filename,
    char *outname);
    time_t    rrd_last_r(
    const char *filename);
    int       rrd_lastupdate_r(
    const char *filename,
    time_t *ret_last_update,
    unsigned long *ret_ds_count,
    char ***ret_ds_names,
    char ***ret_last_ds);
    time_t    rrd_first_r(
    const char *filename,
    const int rraindex);

    int       rrd_dump_cb_r(
    const char *filename,
    int opt_header,
    rrd_output_callback_t cb,
    void *user);

/* Transplanted from rrd_parsetime.h */
    typedef enum {
        ABSOLUTE_TIME,
        RELATIVE_TO_START_TIME,
        RELATIVE_TO_END_TIME,
        RELATIVE_TO_EPOCH
    } rrd_timetype_t;

#define TIME_OK NULL

    typedef struct rrd_time_value {
        rrd_timetype_t type;
        long      offset;
        struct tm tm;
    } rrd_time_value_t;

    char     *rrd_parsetime(
    const char *spec,
    rrd_time_value_t *ptv);
/* END rrd_parsetime.h */

    typedef struct rrd_context {
        char      lib_errstr[256];
        char      rrd_error[4096];
    } rrd_context_t;

/* returns the current per-thread rrd_context */
    rrd_context_t *rrd_get_context(
    void);

#ifdef _WIN32
/* this was added by the win32 porters Christof.Wegmann@exitgames.com */
    rrd_context_t *rrd_force_new_context(
    void);
#endif

    int       rrd_proc_start_end(
    rrd_time_value_t *,
    rrd_time_value_t *,
    time_t *,
    time_t *);

/* HELPER FUNCTIONS */
    void      rrd_set_error(
    char *,
    ...);
    void      rrd_clear_error(
    void);
    int       rrd_test_error(
    void);
    char     *rrd_get_error(
    void);

    /* rrd_strerror is thread safe, but still it uses a global buffer
       (but one per thread), thus subsequent calls within a single
       thread overwrite the same buffer */
    const char *rrd_strerror(
    int err);

/** MULTITHREADED HELPER FUNCTIONS */
    rrd_context_t *rrd_new_context(
    void);
    void      rrd_free_context(
    rrd_context_t *buf);

/* void   rrd_set_error_r  (rrd_context_t *, char *, ...); */
/* void   rrd_clear_error_r(rrd_context_t *); */
/* int    rrd_test_error_r (rrd_context_t *); */
/* char  *rrd_get_error_r  (rrd_context_t *); */

/** UTILITY FUNCTIONS */

    long      rrd_random(
    void);

    int       rrd_add_ptr_chunk(
    void ***dest,
    size_t *dest_size,
    void *src,
    size_t *alloc,
    size_t chunk);
    int       rrd_add_ptr(
    void ***dest,
    size_t *dest_size,
    void *src);
    int       rrd_add_strdup(
    char ***dest,
    size_t *dest_size,
    char *src);
    int       rrd_add_strdup_chunk(
    char ***dest,
    size_t *dest_size,
    char *src,
    size_t *alloc,
    size_t chunk);
    void      rrd_free_ptrs(
    void ***src,
    size_t *cnt);

    int       rrd_mkdir_p(
    const char *pathname,
    mode_t mode);

    const char *rrd_scaled_duration(
    const char *token,
    unsigned long divisor,
    unsigned long *valuep);
    void      rrd_thread_init(
    void);

/*
 * The following functions are _internal_ functions needed to read the raw RRD
 * files. Since they are _internal_ they may change with the file format and
 * will be replaced with a more general interface in RRDtool 1.8.0 Don't use
 * these functions unless you have good reasons to do so. If you do use these
 * functions you will have to adapt your code for RRDtool 1.8.0
 *
 * To enable the deprecated functions define `RRD_EXPORT_DEPRECATED' before
 * including <rrd_test.h>. You have been warned! If you come back to the
 * RRDtool mailing list and whine about your broken application, you will get
 * hit with something smelly!
 */
#if defined(RRD_TOOL_H_3853987DDF7E4709A5B5849E5A6204F4) || defined(RRD_EXPORT_DEPRECATED)

# if defined(RRD_TOOL_H_3853987DDF7E4709A5B5849E5A6204F4)
#  include "rrd_format.h"
# else
#  include <rrd_format.h>
# endif

#if defined(__GNUC__) && defined (RRD_EXPORT_DEPRECATED)
# define RRD_DEPRECATED __attribute__((deprecated))
#else
# define RRD_DEPRECATED          /**/
#endif
    void      rrd_free(
    rrd_t *rrd)
              RRD_DEPRECATED;
    void      rrd_init(
    rrd_t *rrd)
              RRD_DEPRECATED;

    rrd_file_t *rrd_open(
    const char *const file_name,
    rrd_t *rrd,
    unsigned rdwr)
              RRD_DEPRECATED;

    void      rrd_dontneed(
    rrd_file_t *rrd_file,
    rrd_t *rrd)
              RRD_DEPRECATED;
    int       rrd_close(
    rrd_file_t *rrd_file)
              RRD_DEPRECATED;
    ssize_t   rrd_read(
    rrd_file_t *rrd_file,
    void *buf,
    size_t count)
              RRD_DEPRECATED;
    ssize_t   rrd_write(
    rrd_file_t *rrd_file,
    const void *buf,
    size_t count)
              RRD_DEPRECATED;
    void      rrd_flush(
    rrd_file_t *rrd_file)
              RRD_DEPRECATED;
    off_t     rrd_seek(
    rrd_file_t *rrd_file,
    off_t off,
    int whence)
              RRD_DEPRECATED;
    off_t     rrd_tell(
    rrd_file_t *rrd_file)
              RRD_DEPRECATED;
    int       rrd_lock(
    rrd_file_t *file)
              RRD_DEPRECATED;
    void      rrd_notify_row(
    rrd_file_t *rrd_file,
    int rra_idx,
    unsigned long rra_row,
    time_t rra_time)
              RRD_DEPRECATED;
    unsigned long rrd_select_initial_row(
    rrd_file_t *rrd_file,
    int rra_idx,
    rra_def_t *rra)
              RRD_DEPRECATED;
#endif                  /* defined(_RRD_TOOL_H) || defined(RRD_EXPORT_DEPRECATED) */

#ifdef  __cplusplus
}
#endif
#endif                  /* RRDLIB_H */
