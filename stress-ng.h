/*
 * Copyright (C) 2013-2014 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * This code is a complete clean re-write of the stress tool by
 * Colin Ian King <colin.king@canonical.com> and attempts to be
 * backwardly compatible with the stress tool by Amos Waterland
 * <apw@rossby.metr.ou.edu> but has more stress tests and more
 * functionality.
 *
 */

#ifndef __STRESS_NG_H__
#define __STRESS_NG_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include <semaphore.h>
#include <sys/time.h>

#define _GNU_SOURCE
/* GNU HURD */
#ifndef PATH_MAX
#define PATH_MAX 		(4096)
#endif

#define STRESS_FD_MAX		(65536)
#define STRESS_PROCS_MAX	(1024)

#ifndef PIPE_BUF
#define PIPE_BUF		(512)
#endif
#define SOCKET_BUF		(8192)

/* Option bit masks */
#define OPT_FLAGS_NO_CLEAN	0x00000001	/* Don't remove hdd files */
#define OPT_FLAGS_DRY_RUN	0x00000002	/* Don't actually run */
#define OPT_FLAGS_METRICS	0x00000004	/* Dump metrics at end */
#define OPT_FLAGS_VM_KEEP	0x00000008	/* Don't keep re-allocating */
#define OPT_FLAGS_RANDOM	0x00000010	/* Randomize */
#define OPT_FLAGS_SET		0x00000020	/* Set if user specifies stress procs */
#define OPT_FLAGS_KEEP_NAME	0x00000040	/* Keep stress names to stress-ng */
#define OPT_FLAGS_UTIME_FSYNC	0x00000080	/* fsync after utime modification */
#define OPT_FLAGS_METRICS_BRIEF	0x00000100	/* dump brief metrics */
#define OPT_FLAGS_VERIFY	0x00000200	/* verify mode */
#define OPT_FLAGS_MMAP_MADVISE	0x00000400	/* enable random madvise settings */
#define OPT_FLAGS_MMAP_MINCORE	0x00000800	/* mincore force pages into mem */
#define OPT_FLAGS_TIMES		0x00001000	/* user/system time summary */

/* Stressor classes */
#define CLASS_CPU		0x00000001	/* CPU only */
#define CLASS_MEMORY		0x00000002	/* Memory thrashers */
#define CLASS_CPU_CACHE		0x00000004	/* CPU cache */
#define CLASS_IO		0x00000008	/* I/O read/writes etc */
#define CLASS_NETWORK		0x00000010	/* Network, sockets, etc */
#define CLASS_SCHEDULER		0x00000020	/* Scheduling */
#define CLASS_VM		0x00000040	/* VM stress, big memory, swapping */
#define CLASS_INTERRUPT		0x00000080	/* interrupt floods */
#define CLASS_OS		0x00000100	/* generic OS tests */

/* debug output bitmasks */
#define PR_ERROR		0x00010000	/* Print errors */
#define PR_INFO			0x00020000	/* Print info */
#define PR_DEBUG		0x00040000	/* Print debug */
#define PR_FAIL			0x00080000	/* Print test failure message */
#define PR_ALL			(PR_ERROR | PR_INFO | PR_DEBUG | PR_FAIL)

#define pr_dbg(fp, fmt, args...)	print(fp, PR_DEBUG, fmt, ## args)
#define pr_inf(fp, fmt, args...)	print(fp, PR_INFO, fmt, ## args)
#define pr_err(fp, fmt, args...)	print(fp, PR_ERROR, fmt, ## args)
#define pr_fail(fp, fmt, args...)	print(fp, PR_FAIL, fmt, ## args)
#define pr_tidy(fp, fmt, args...)	print(fp, opt_sigint ? PR_INFO : PR_DEBUG, fmt, ## args)

#define pr_failed_err(name, what)	pr_failed(PR_ERROR, name, what)
#define pr_failed_dbg(name, what)	pr_failed(PR_DEBUG, name, what)

#define KB			(1024ULL)
#define	MB			(KB * KB)
#define GB			(KB * KB * KB)

#define PAGE_4K_SHIFT		(12)
#define PAGE_4K			(1 << PAGE_4K_SHIFT)

#define MIN_VM_BYTES		(4 * KB)
#define MAX_VM_BYTES		(1 * GB)
#define DEFAULT_VM_BYTES	(256 * MB)

#define MIN_MMAP_BYTES		(4 * KB)
#define MAX_MMAP_BYTES		(1 * GB)
#define DEFAULT_MMAP_BYTES	(256 * MB)

#define MIN_VM_STRIDE		(1)
#define MAX_VM_STRIDE		(1 * MB)
#define DEFAULT_VM_STRIDE	(4 * KB)

#define MIN_HDD_BYTES		(1 * MB)
#define MAX_HDD_BYTES		(256 * GB)
#define DEFAULT_HDD_BYTES	(1 * GB)

#define MIN_HDD_WRITE_SIZE	(1)
#define MAX_HDD_WRITE_SIZE	(4 * MB)
#define DEFAULT_HDD_WRITE_SIZE	(64 * 1024)

#define MIN_VM_HANG		(0)
#define MAX_VM_HANG		(3600)
#define DEFAULT_VM_HANG		(~0ULL)

#define DEFAULT_TIMEOUT		(60 * 60 * 24)
#define DEFAULT_BACKOFF		(0)
#define DEFAULT_DENTRIES	(2048)
#define DEFAULT_LINKS		(8192)
#define DEFAULT_DIRS		(8192)

#define DEFAULT_OPS_MIN		(100ULL)
#define DEFAULT_OPS_MAX		(100000000ULL)

#define DEFAULT_SENDFILE_SIZE	(4 * MB)

#define SWITCH_STOP		'X'
#define PIPE_STOP		"PIPE_STOP"

#define MEM_CACHE_SIZE		(65536 * 32)
#define UNDEFINED		(-1)

#define PAGE_MAPPED		(0x01)
#define PAGE_MAPPED_FAIL	(0x02)

#define FFT_SIZE		(4096)

#define SIEVE_GETBIT(a, i)	(a[i / 32] & (1 << (i & 31)))
#define SIEVE_CLRBIT(a, i)	(a[i / 32] &= ~(1 << (i & 31)))
#define SIEVE_SIZE 		(10000000)

#define MWC_SEED_Z		(362436069)
#define MWC_SEED_W		(521288629)

#define MWC_SEED()		mwc_seed(MWC_SEED_W, MWC_SEED_Z)

#define DEFAULT_FORKS		(1)
#define DEFAULT_FORKS_MIN	(1)
#define DEFAULT_FORKS_MAX	(16000)

#define DEFAULT_SEQUENTIAL	(0)	/* Disabled */
#define DEFAULT_SEQUENTIAL_MIN	(0)
#define DEFAULT_SEQUENTIAL_MAX	(1000000)

#define MIN_SEEK_SIZE		(1 * MB)
#define MAX_SEEK_SIZE 		(256 * GB)
#define DEFAULT_SEEK_SIZE	(16 * MB)

#define SIZEOF_ARRAY(a)		(sizeof(a) / sizeof(a[0]))

#define ABORT_FAILURES		(5)

#if defined(__x86_64__) || defined(__x86_64) || defined(__i386__) || defined(__i386)
#define STRESS_X86	1
#endif

/* stress process prototype */
typedef int (*func)(uint64_t *const counter, const uint32_t instance,
		    const uint64_t max_ops, const char *name);

/* Help information for options */
typedef struct {
	const char *opt_s;		/* short option */
	const char *opt_l;		/* long option */
	const char *description;	/* description */
} help_t;

#ifdef __GNUC__
#define ALIGN64	__attribute__ ((aligned(64)))
#else
#define ALIGN64
#endif

typedef struct {
	uint8_t	 mem_cache[MEM_CACHE_SIZE] ALIGN64;
	uint32_t futex[STRESS_PROCS_MAX] ALIGN64;
	uint64_t futex_timeout[STRESS_PROCS_MAX] ALIGN64;
	uint64_t counters[0] ALIGN64;
} shared_t;

/* Stress test classes */
typedef struct {
	uint32_t class;		/* Class type bit mask */
	const char *name;	/* Name of class */
} class_t;

/* Stress tests */
typedef enum {
#if defined(__linux__)
	STRESS_AFFINITY = 0,
#endif
	STRESS_BSEARCH,
	STRESS_BIGHEAP,
	STRESS_CACHE,
#if _POSIX_C_SOURCE >= 199309L
	STRESS_CLOCK,
#endif
	STRESS_CPU,
	STRESS_DENTRY,
	STRESS_DIR,
#if defined(__linux__)
	STRESS_EVENTFD,
#endif
#if _XOPEN_SOURCE >= 600 || _POSIX_C_SOURCE >= 200112L
	STRESS_FALLOCATE,
#endif
	STRESS_FAULT,
	STRESS_FLOCK,
	STRESS_FORK,
	STRESS_FSTAT,
#if defined(__linux__)
	STRESS_FUTEX,
#endif
	STRESS_GET,
	STRESS_HDD,
#if defined(__linux__)
	STRESS_INOTIFY,
#endif
	STRESS_IOSYNC,
	STRESS_KILL,
	STRESS_LINK,
	STRESS_LSEARCH,
	STRESS_MEMCPY,
	STRESS_MMAP,
#if !defined(__gnu_hurd__)
	STRESS_MSG,
#endif
	STRESS_NICE,
	STRESS_NULL,
	STRESS_OPEN,
	STRESS_PIPE,
	STRESS_POLL,
#if defined(__linux__)
	STRESS_PROCFS,
#endif
	STRESS_QSORT,
#if defined(STRESS_X86)
	STRESS_RDRAND,
#endif
	STRESS_RENAME,
	STRESS_SEEK,
#if defined(__linux__)
	STRESS_SENDFILE,
#endif
	STRESS_SEMAPHORE,
	STRESS_SIGFPE,
#if _POSIX_C_SOURCE >= 199309L && !defined(__gnu_hurd__)
	STRESS_SIGQUEUE,
#endif
	STRESS_SIGSEGV,
	STRESS_SOCKET,
	STRESS_SWITCH,
	STRESS_SYMLINK,
	STRESS_SYSINFO,
#if defined(__linux__)
	STRESS_TIMER,
#endif
	STRESS_TSEARCH,
#if defined(__linux__) || defined(__gnu_hurd__)
	STRESS_URANDOM,
#endif
	STRESS_UTIME,
#if  _BSD_SOURCE || \
    (_XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED) && \
    !(_POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)
	STRESS_VFORK,
#endif
	STRESS_VM,
#if !defined(__gnu_hurd__)
	STRESS_WAIT,
#endif
#if defined (_POSIX_PRIORITY_SCHEDULING)
	STRESS_YIELD,
#endif
	STRESS_ZERO,
	/* STRESS_MAX must be last one */
	STRESS_MAX
} stress_id;

/* Command line long options */
typedef enum {
	/* Short options */
	OPT_QUERY = '?',
	OPT_ALL = 'a',
	OPT_BACKOFF = 'b',
	OPT_BIGHEAP = 'B',
	OPT_CPU = 'c',
	OPT_CACHE = 'C',
	OPT_HDD = 'd',
	OPT_DENTRY = 'D',
	OPT_FORK = 'f',
#if _XOPEN_SOURCE >= 600 || _POSIX_C_SOURCE >= 200112L
	OPT_FALLOCATE = 'F',
#endif
	OPT_IOSYNC = 'i',
	OPT_HELP = 'h',
	OPT_KEEP_NAME = 'k',
	OPT_CPU_LOAD = 'l',
	OPT_VM = 'm',
	OPT_METRICS = 'M',
	OPT_DRY_RUN = 'n',
	OPT_RENAME = 'R',
	OPT_OPEN = 'o',
	OPT_PIPE = 'p',
	OPT_POLL = 'P',
	OPT_QUIET = 'q',
	OPT_RANDOM = 'r',
	OPT_SWITCH = 's',
	OPT_SOCKET = 'S',
	OPT_TIMEOUT = 't',
#if defined (__linux__)
	OPT_TIMER = 'T',
#endif
#if defined (__linux__) || defined(__gnu_hurd__)
	OPT_URANDOM = 'u',
#endif
	OPT_VERBOSE = 'v',
	OPT_VERSION = 'V',
	OPT_YIELD = 'y',

	/* Long options only */


	OPT_AFFINITY = 0x80,
	OPT_AFFINITY_OPS,

	OPT_BSEARCH,
	OPT_BSEARCH_OPS,
	OPT_BSEARCH_SIZE,

	OPT_BIGHEAP_OPS,
	OPT_BIGHEAP_GROWTH,

	OPT_CLASS,
	OPT_CACHE_OPS,

#if _POSIX_C_SOURCE >= 199309L
	OPT_CLOCK,
	OPT_CLOCK_OPS,
#endif

	OPT_CPU_OPS,
	OPT_CPU_METHOD,

	OPT_DENTRY_OPS,
	OPT_DENTRIES,

	OPT_DIR,
	OPT_DIR_OPS,

	OPT_HDD_BYTES,
	OPT_HDD_NOCLEAN,
	OPT_HDD_WRITE_SIZE,
	OPT_HDD_OPS,

#if defined(__linux__)
	OPT_EVENTFD,
	OPT_EVENTFD_OPS,
#endif

#if _XOPEN_SOURCE >= 600 || _POSIX_C_SOURCE >= 200112L
	OPT_FALLOCATE_OPS,
#endif
	OPT_FAULT,
	OPT_FAULT_OPS,

	OPT_FLOCK,
	OPT_FLOCK_OPS,

	OPT_FORK_OPS,
	OPT_FORK_MAX,

	OPT_FSTAT,
	OPT_FSTAT_OPS,
	OPT_FSTAT_DIR,

	OPT_FUTEX,
	OPT_FUTEX_OPS,

	OPT_GET,
	OPT_GET_OPS,

#if defined (__linux__)
	OPT_INOTIFY,
	OPT_INOTIFY_OPS,
#endif

#if defined (__linux__)
	OPT_IONICE_CLASS,
	OPT_IONICE_LEVEL,
#endif

	OPT_IOSYNC_OPS,

	OPT_KILL,
	OPT_KILL_OPS,

	OPT_LINK,
	OPT_LINK_OPS,

	OPT_LSEARCH,
	OPT_LSEARCH_OPS,
	OPT_LSEARCH_SIZE,

	OPT_METRICS_BRIEF,

	OPT_MEMCPY,
	OPT_MEMCPY_OPS,

	OPT_MMAP,
	OPT_MMAP_OPS,
	OPT_MMAP_BYTES,

	OPT_MSG,
	OPT_MSG_OPS,

	OPT_NICE,
	OPT_NICE_OPS,

	OPT_NO_MADVISE,

	OPT_NULL,
	OPT_NULL_OPS,

	OPT_OPEN_OPS,

	OPT_PAGE_IN,

	OPT_PIPE_OPS,

	OPT_POLL_OPS,

	OPT_PROCFS,
	OPT_PROCFS_OPS,

	OPT_QSORT,
	OPT_QSORT_OPS,
	OPT_QSORT_INTEGERS,

	OPT_RDRAND,
	OPT_RDRAND_OPS,

	OPT_RENAME_OPS,

	OPT_SCHED,
	OPT_SCHED_PRIO,

	OPT_SEEK,
	OPT_SEEK_OPS,
	OPT_SEEK_SIZE,
	
	OPT_SENDFILE,
	OPT_SENDFILE_OPS,
	OPT_SENDFILE_SIZE,

	OPT_SEMAPHORE,
	OPT_SEMAPHORE_OPS,

	OPT_SEQUENTIAL,

	OPT_SIGFPE,
	OPT_SIGFPE_OPS,

	OPT_SIGSEGV,
	OPT_SIGSEGV_OPS,

#if _POSIX_C_SOURCE >= 199309L
	OPT_SIGQUEUE,
	OPT_SIGQUEUE_OPS,
#endif

	OPT_SOCKET_OPS,
	OPT_SOCKET_PORT,

	OPT_SWITCH_OPS,

	OPT_SYMLINK,
	OPT_SYMLINK_OPS,

	OPT_SYSINFO,
	OPT_SYSINFO_OPS,

#if defined (__linux__)
	OPT_TIMER_OPS,
	OPT_TIMER_FREQ,
#endif

	OPT_TSEARCH,
	OPT_TSEARCH_OPS,
	OPT_TSEARCH_SIZE,

#if defined (__linux__)
	OPT_TIMES,
#endif

#if defined (__linux__) || defined(__gnu_hurd__)
	OPT_URANDOM_OPS,
#endif
	OPT_UTIME,
	OPT_UTIME_OPS,
	OPT_UTIME_FSYNC,

	OPT_VERIFY,

#if  _BSD_SOURCE || \
     (_XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED) && \
     !(_POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)
	OPT_VFORK,
	OPT_VFORK_OPS,
	OPT_VFORK_MAX,
#endif

	OPT_VM_BYTES,
	OPT_VM_STRIDE,
	OPT_VM_HANG,
	OPT_VM_KEEP,
#ifdef MAP_POPULATE
	OPT_VM_MMAP_POPULATE,
#endif
#ifdef MAP_LOCKED
	OPT_VM_MMAP_LOCKED,
#endif
	OPT_VM_OPS,

#if !defined(__gnu_hurd__)
	OPT_WAIT,
	OPT_WAIT_OPS,
#endif

#if defined (_POSIX_PRIORITY_SCHEDULING)
	OPT_YIELD_OPS,
#endif

	OPT_ZERO,
	OPT_ZERO_OPS,
} stress_op;

/* stress test metadata */
typedef struct {
	const func stress_func;		/* stress test function */
	const stress_id id;		/* stress test ID */
	const short int short_getopt;	/* getopt short option */
	const stress_op op;		/* ops option */
	const char *name;		/* name of stress test */
	const uint32_t class;		/* class of stress test */
} stress_t;

/*
 *  the CPU stress test has different classes of cpu stressor
 */
typedef void (*stress_cpu_func)(void);

typedef struct {
	const char		*name;	/* human readable form of stressor */
	const stress_cpu_func	func;	/* the stressor function */
} stress_cpu_stressor_info_t;

typedef struct {
	pid_t	pid;		/* process id */
	double	start;		/* time process started */
	double	finish;		/* time process got reaped */
} proc_info_t;

typedef struct {
	const char	ch;	/* Scaling suffix */
	const uint64_t	scale;	/* Amount to scale by */
} scale_t;

/* stress process prototype */
typedef int (*func)(uint64_t *const counter, const uint32_t
	instance, const uint64_t max_ops, const char *name);


/* Various option settings and flags */
extern const char *app_name;				/* Name of application */
extern sem_t	sem;					/* stress_semaphore sem */
extern bool     sem_ok;					/* stress_semaphore init ok */
extern shared_t *shared;				/* shared memory */
extern uint64_t	opt_dentries;				/* dentries per loop */
extern uint64_t opt_ops[STRESS_MAX];			/* max number of bogo ops */
extern uint64_t	opt_vm_hang; 				/* VM delay */
extern uint64_t	opt_hdd_bytes; 				/* HDD size in byts */
extern uint64_t opt_hdd_write_size;
extern uint64_t opt_sendfile_size;			/* sendfile size */
extern uint64_t opt_seek_size;				/* seek file size */
extern uint64_t	opt_timeout;				/* timeout in seconds */
extern uint64_t	mwc_z, mwc_w;				/* random number vals */
extern uint64_t opt_qsort_size; 			/* Default qsort size */
extern uint64_t opt_bsearch_size; 			/* Default bsearch size */
extern uint64_t opt_tsearch_size; 			/* Default tsearch size */
extern uint64_t opt_lsearch_size; 			/* Default lsearch size */
extern uint64_t opt_bigheap_growth;			/* Amount big heap grows */
extern uint64_t opt_fork_max;				/* Number of fork stress processes */
extern uint64_t opt_vfork_max;				/* Number of vfork stress processes */
extern uint64_t opt_sequential;				/* Number of sequention iterations */
extern int64_t	opt_backoff ;				/* child delay */
extern int32_t	started_procs[STRESS_MAX];		/* number of processes per stressor */
extern int32_t	opt_flags;				/* option flags */
extern int32_t  opt_cpu_load;				/* CPU max load */
extern stress_cpu_stressor_info_t *opt_cpu_stressor;	/* Default stress CPU method */
extern size_t	opt_vm_bytes;				/* VM bytes */
extern size_t	opt_vm_stride;				/* VM stride */
extern int	opt_vm_flags;				/* VM mmap flags */
extern size_t	opt_mmap_bytes;				/* MMAP size */
extern pid_t	socket_server, socket_client;		/* pids of socket client/servers */
#if defined (__linux__)
extern uint64_t	opt_timer_freq;				/* timer frequency (Hz) */
extern int	opt_sched;				/* sched policy */
extern int	opt_sched_priority;			/* sched priority */
extern int 	opt_ionice_class;			/* ionice class */
extern int	opt_ionice_level;			/* ionice level */
#endif
extern int	opt_socket_port;			/* Default socket port */
extern long int	opt_nprocessors_online;			/* Number of processors online */
extern char	*opt_fstat_dir;				/* Default fstat directory */
extern volatile bool opt_do_run;			/* false to exit stressor */
extern volatile bool opt_sigint;			/* true if stopped by SIGINT */
extern proc_info_t *procs[STRESS_MAX];			/* per process info */
extern stress_cpu_stressor_info_t cpu_methods[];	/* cpu stressor methods */

/*
 *  externs to force gcc to stash computed values and hence
 *  to stop the optimiser optimising code away to zero. The
 *  *_put funcs are essentially no-op functions.
 */
extern void double_put(const double a);
extern void uint64_put(const uint64_t a);
extern uint64_t uint64_zero(void);
extern int stress_temp_filename(char *path, const size_t len, const char *name, const pid_t pid, const uint32_t instance, const uint64_t magic);
extern int stress_temp_dir(char *path, const size_t len, const char *name, const pid_t pid, const uint32_t instance);
extern int stress_temp_dir_mk(const char *name, const pid_t pid, const uint32_t instance);
extern int stress_temp_dir_rm(const char *name, const pid_t pid, const uint32_t instance);

extern double timeval_to_double(const struct timeval *tv);
extern double time_now(void);
extern uint64_t mwc(void);
extern void mwc_seed(const uint64_t w, const uint64_t z);
extern void mwc_reseed(void);
extern stress_cpu_stressor_info_t *stress_cpu_find_by_name(const char *name);
extern void pr_failed(const int flag, const char *name, const char *what);
extern void set_oom_adjustment(const char *name, bool killable);
extern void set_sched(const int sched, const int sched_priority);
extern int get_opt_sched(const char *const str);
extern int get_opt_ionice_class(const char *const str);
extern void set_iopriority(const int class, const int level);
extern void set_oom_adjustment(const char *name, bool killable);
extern void set_coredump(const char *name);
extern void set_proc_name(const char *name);
extern int madvise_random(void *addr, size_t length);

extern void check_value(const char *const msg, const int val);
extern void check_range(const char *const opt, const uint64_t val,
	const uint64_t lo, const uint64_t hi);
extern int get_int(const char *const str);
extern uint64_t get_uint64(const char *const str);
extern uint64_t get_uint64_scale(const char *const str, const scale_t scales[],
	const char *const msg);
extern uint64_t get_uint64_byte(const char *const str);
extern uint64_t get_uint64_time(const char *const str);
extern void lock_mem_current(void);
extern int mincore_touch_pages(void *buf, size_t buf_len);

#define STRESS(name)								\
	extern int name(uint64_t *const counter, const uint32_t instance,	\
        const uint64_t max_ops, const char *name)				

STRESS(stress_affinity);
STRESS(stress_bigheap);
STRESS(stress_bsearch);
STRESS(stress_cache);
STRESS(stress_clock);
STRESS(stress_cpu);
STRESS(stress_dir);
STRESS(stress_dentry);
STRESS(stress_eventfd);
STRESS(stress_hdd);
STRESS(stress_fallocate);
STRESS(stress_fault);
STRESS(stress_flock);
STRESS(stress_fork);
STRESS(stress_fstat);
STRESS(stress_futex);
STRESS(stress_get);
STRESS(stress_inotify);
STRESS(stress_iosync);
STRESS(stress_kill);
STRESS(stress_link);
STRESS(stress_lsearch);
STRESS(stress_memcpy);
STRESS(stress_mmap);
STRESS(stress_msg);
STRESS(stress_nice);
STRESS(stress_noop);
STRESS(stress_null);
STRESS(stress_open);
STRESS(stress_pipe);
STRESS(stress_poll);
STRESS(stress_procfs);
STRESS(stress_qsort);
STRESS(stress_rdrand);
STRESS(stress_rename);
STRESS(stress_seek);
STRESS(stress_semaphore);
STRESS(stress_sendfile);
STRESS(stress_sigfpe);
STRESS(stress_sigsegv);
STRESS(stress_sigq);
STRESS(stress_socket);
STRESS(stress_switch);
STRESS(stress_symlink);
STRESS(stress_sysinfo);
STRESS(stress_timer);
STRESS(stress_tsearch);
STRESS(stress_urandom);
STRESS(stress_utime);
STRESS(stress_vfork);
STRESS(stress_vm);
STRESS(stress_wait);
STRESS(stress_yield);
STRESS(stress_zero);

extern int print(FILE *fp, const int flag,
	const char *const fmt, ...) __attribute__((format(printf, 3, 4)));

#endif
