#! /bin/bash

# Copyright notice:
echo "
   This file is part of Wool, a library for fine-grained independent 
   task parallelism

   Copyright (C) 2009- Karl-Filip Faxen
      kff@sics.se

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.
" > /dev/null

echo '
#include <pthread.h>

#if defined(__ia64__) && defined(__INTEL_COMPILER)
  #include <ia64intrin.h>
#endif

#define SYNC_MORE 0

#ifndef COUNT_EVENTS
  #define COUNT_EVENTS 0
#endif

#ifndef COUNT_EVENTS_EXP
  #define COUNT_EVENTS_EXP 0
#endif

#ifndef SPAWN_FENCE
  #define SPAWN_FENCE 0  /* correct for x86 */
#endif

#ifndef FINEST_GRAIN
  #define FINEST_GRAIN 2000
#endif

#ifndef LOG_EVENTS
  #define LOG_EVENTS 0
#endif

#ifndef WOOL_DEFER_BOT_DEC
  #define WOOL_DEFER_BOT_DEC 1
#endif

#ifndef WOOL_DEFER_NOT_STOLEN
  #define WOOL_DEFER_NOT_STOLEN 1
#endif

#ifndef WOOL_READ_STOLEN
  #define WOOL_READ_STOLEN 0
#endif

#ifndef WOOL_MEASURE_SPAN
  #define WOOL_MEASURE_SPAN 0
#endif

#if WOOL_MEASURE_SPAN
  #define WOOL_WHEN_MSPAN( x ) x
#else
  #define WOOL_WHEN_MSPAN( x )
#endif

#ifndef THE_SYNC
  #if defined(__ia64__) && ! defined(__INTEL_COMPILER)
    #define THE_SYNC 1
  #else
    #define THE_SYNC 0
  #endif
#endif

#ifndef LINE_SIZE
  #if defined(__ia64__)
    #define LINE_SIZE 128 /* Good for SGI Altix; who else uses Itanic? */
  #else
    #define LINE_SIZE 64  /* A common value for current processors */
  #endif
#endif

#define SMALL_BODY             2
#define MEDIUM_BODY          100
#define LARGE_BODY  FINEST_GRAIN

#define P_SZ (sizeof(void *))
#define I_SZ (sizeof(int))
#define L_SZ (sizeof(long int))

#define PAD(x,b) ( ( (b) - ((x)%(b)) ) & ((b)-1) ) /* b must be power of 2 */ 
#define ROUND(x,b) ( (x) + PAD( (x), (b) ) )
'
echo "
#ifndef TASK_PAYLOAD
  #define TASK_PAYLOAD $1*8
#endif
"
echo '

typedef volatile unsigned long exarg_t;

#if defined(__sparc__)
  #define SFENCE        asm volatile( "membar #StoreStore" )
  #define MFENCE        asm volatile( "membar #StoreLoad|#StoreStore" )
  #define PREFETCH(a)   asm ( "prefetch %0, 2" : : "m"(a) )
#elif defined(__i386__)
  #define SFENCE        asm volatile( "sfence" )
  #define MFENCE        asm volatile( "mfence" )
  #define PREFETCH(a)   /*  */
  #define EXCHANGE(R,M) asm volatile ( "xchg   %1, %0" : "+m" (M), "+r" (R) )
#elif defined(__x86_64__)
  #define SFENCE        asm volatile( "sfence" )
  #define MFENCE        asm volatile( "mfence" )
  /* { volatile int i=1; EXCHANGE( i, i ); } */
  #define PREFETCH(a)   /*  */
  #define EXCHANGE(R,M) asm volatile ( "xchg   %1, %0" : "+m" (M), "+r" (R) )
  #define CAS(R,M,V)  asm volatile ( "lock cmpxchg %2, %1" \
                                     : "+a" (V), "+m"(M) : "r" (R) : "cc" )
#elif defined(__ia64__)
  #define SFENCE       /* */
  #define MFENCE        __sync_synchronize()
  #define PREFETCH(a)   /* */
  #define EXCHANGE(R,M) (__builtin_prefetch( &(M), 1 ), \
       (R) = (typeof(R)) _InterlockedCompareExchange64_acq((exarg_t *) &(M), (exarg_t) R, (exarg_t) M))
#endif

#if defined(__ia64__) && defined(__INTEL_COMPILER)
  #define COMPILER_FENCE  __memory_barrier()
  #define STORE_PTR_REL(addr,val) __st8_rel(addr, (__int64) (val))
  #define STORE_INT_REL(addr,val) __st4_rel(addr,(int) (val))
#else
  #define COMPILER_FENCE  asm volatile( "" )
  // x86, amd64 and SPARC v9 can do without a store barrier
  #define STORE_PTR_REL(addr,val) (*(addr) = (val))
  #define STORE_INT_REL(addr,val) (*(addr) = (val))
#endif

typedef int balarm_t;
typedef long long unsigned int hrtime_t; 

WOOL_WHEN_MSPAN( extern hrtime_t __wool_sc; )

#if COUNT_EVENTS
#define PR_ADD(s,i,k) ( ((s)->ctr[i])+= k )
#else
#define PR_ADD(s,i,k) /* Empty */
#endif
#define PR_INC(s,i)  PR_ADD(s,i,1)

#if COUNT_EVENTS_EXP
#define PR_INC_EXP(s,i) (PR_INC(s,i))
#else
#define PR_INC_EXP(s,i) /* Empty */
#endif

typedef enum {
  CTR_spawn=0,
  CTR_inlined,
  CTR_read,
  CTR_waits,
  CTR_sync_lock,
  CTR_steal_tries,
  CTR_steal_locks,
  CTR_steals,
  CTR_leap_tries,
  CTR_leap_locks,
  CTR_leaps,
  CTR_spins,
  CTR_steal_1s,
  CTR_steal_1t,
  CTR_steal_ps,
  CTR_steal_pt,
  CTR_steal_hs,
  CTR_steal_ht,
  CTR_steal_ms,
  CTR_steal_mt,
  CTR_sync_no_dec,
  CTR_steal_no_inc,
  CTR_skip_try,
  CTR_skip,
  CTR_MAX
} CTR_index;

typedef pthread_mutex_t wool_lock_t;
typedef pthread_cond_t  wool_cond_t;

#define wool_lock(l)      pthread_mutex_lock( l )
#define wool_unlock(l)    pthread_mutex_unlock( l )
#define wool_trylock(l)   pthread_mutex_trylock( l )

#define wool_wait(c,l)    pthread_cond_wait( c, l )
#define wool_signal(c)    pthread_cond_signal( c )
#define wool_broadcast(c) pthread_cond_broadcast( c )

#define TASK_COMMON_FIELDS(ty)    \
  WOOL_WHEN_MSPAN( hrtime_t spawn_span; ) \
  void (*f)(struct _Task *, ty);  \
  balarm_t balarm;                \
  unsigned stealable;             \
  struct _Worker *self;

struct _Task;

typedef struct {
  TASK_COMMON_FIELDS( struct _Task * )
} __wool_task_common;

#define COMMON_FIELD_SIZE sizeof( __wool_task_common )

typedef struct _Task {
  TASK_COMMON_FIELDS( struct _Task * )
  char p1[ PAD( COMMON_FIELD_SIZE, P_SZ ) ];
  char d[ TASK_PAYLOAD ];
  char p2[ PAD( ROUND( COMMON_FIELD_SIZE, P_SZ ) + TASK_PAYLOAD, LINE_SIZE ) ];
} Task;

#define WRAPPER_TYPE void (*)( struct _Task *, struct _Task * )

#define T_BUSY ((WRAPPER_TYPE) 0)
#define T_DONE ((WRAPPER_TYPE) 1)
#define T_LAST ((WRAPPER_TYPE) 1)

#define NOT_STOLEN  ( (balarm_t) -3 )
#define STOLEN_BUSY ( (balarm_t) -2 ) // Not used with LF 
#define STOLEN_DONE ( (balarm_t) -1 )
#define B_LAST      STOLEN_DONE

#if LOG_EVENTS
typedef struct _LogEntry {
  hrtime_t time;
  int what;
} LogEntry;
#endif

typedef struct _Worker {
  // First cache line, public stuff seldom written by the owner
  Task     *dq_base, // Always pointing the base of the dequeue
           *dq_top,  // Not used in this version
           *dq_bot;  // The next task to steal
  long int  is_thief; // Pointer size for alignment!
  wool_lock_t *dq_lock; // Mainly used for mutex among thieves, 
                        // but also as backup for victim
  wool_lock_t the_lock; // dq_lock points here
  char pad1[ PAD( 5*P_SZ+sizeof(wool_lock_t), LINE_SIZE ) ];

  // Second cache line, private stuff often written by the owner
  volatile hrtime_t time;
#if LOG_EVENTS
  LogEntry         *logptr;
#else
  void             *logptr;
#endif
  volatile int      clock;
  int               dq_size;
  int               idx;
  int               decrement_deferred;
  unsigned int  ctr[CTR_MAX]; 
  char pad2[ PAD( P_SZ+sizeof(hrtime_t)+4*I_SZ+CTR_MAX*I_SZ, LINE_SIZE ) ];
} Worker;

#if LOG_EVENTS
  void logEvent( Worker*, int );
#else
  #define logEvent( w, i ) /* Nothing */
#endif

#define get_self( t ) ( t->self )

#ifdef __cplusplus
extern "C" {
#endif

void  wool_sync( volatile Task *, balarm_t );
balarm_t sync_get_balarm( Task * );
int CALL_main( Task *, int, char ** );

#ifdef __cplusplus
}
#endif

#define SYNC( f )       (__dq_top--, SYNC_##f( __dq_top ) )
#define SPAWN( f, ... ) ( SPAWN_##f( __dq_top ,##__VA_ARGS__ ), __dq_top++ )
#define CALL( f, ... )  ( CALL_##f( __dq_top , ##__VA_ARGS__ ) )
#define FOR( f, ... )   ( CALL( TREE_##f , ##__VA_ARGS__ ) )

'
#
# Second part, once for each arity
#

for(( r = 0; r <= $1; r++ )) do

# Naming various argument lists

MACRO_ARGS=""
for(( i = 1; i <= $r; i++ )) do
  MACRO_ARGS="$MACRO_ARGS, ATYPE_$i, ARG_$i"
done

FIELD_DECL=""
for(( i = 1; i <= $r; i++ )) do
  FIELD_DECL="${FIELD_DECL}      ATYPE_$i ARG_$i;
"
done

TASK_ARGS=""
for(( i = 1; i <= $r; i++ )) do
  TASK_ARGS="$TASK_ARGS, ATYPE_$i ARG_$i"
done

TASK_INIT=`for(( i = 1; i <= $r; i++ )) do
  echo "  p->d.a.ARG_$i = ARG_$i;"
done;`

TASK_GET_FROM_t=`for(( i = 1; i <= $r; i++ )) do
  echo -n ", t->d.a.ARG_$i"
done`

CALL_ARGS=`for(( i = 1; i <= $r; i++ )) do
  echo -n ", ARG_$i"
done`

echo
echo "// Task definition for arity $r"
echo
if ((r)); then
for isvoid in 0 1; do
(\
if (( isvoid==0 )); then
  echo "#define TASK_$r(RTYPE, NAME$MACRO_ARGS )";
  RTYPE="RTYPE"
  RES_FIELD="$RTYPE res;
"
  SAVE_RVAL="t->d.res ="
  RETURN_RES="( (TD_##NAME *) q )->d.res"
  ASSIGN_RES="res = "
  RES_VAR="res"
else
  echo "#define VOID_TASK_$r(NAME$MACRO_ARGS )";
  RTYPE="void"
  RES_FIELD=""
  SAVE_RVAL=""
  RETURN_RES=""
  ASSIGN_RES=""
  RES_VAR=""
fi


echo "
typedef struct _TD_##NAME {
  TASK_COMMON_FIELDS( struct _TD_##NAME * )
  union {
    struct {
$FIELD_DECL\
    } a;
    $RES_FIELD
  } d;
} TD_##NAME;

static void WRAP_##NAME(Task *, TD_##NAME *);
$RTYPE CALL_##NAME(Task *__dq_top$TASK_ARGS);

inline void SPAWN_##NAME(Task *__dq_top$TASK_ARGS)
{
  TD_##NAME *p = (TD_##NAME *) __dq_top;

  WOOL_WHEN_MSPAN( p->spawn_span = __wool_update_time(); )

$TASK_INIT
  if( ( SPAWN_FENCE || LOG_EVENTS ) && p->stealable ) {
    /*logEvent( get_self( p ), 5 );*/
    SFENCE;
  }
  if( WOOL_DEFER_NOT_STOLEN ) {
    p->balarm = NOT_STOLEN;
  }
  if( WOOL_DEFER_BOT_DEC ) {
    Worker *self = get_self( p );
    if( self->decrement_deferred && self->dq_bot > __dq_top ) {
      self->decrement_deferred = 0;
      self->dq_bot = __dq_top;
    } else {
      /* PR_INC( self, CTR_sync_no_dec ); */
    }
  } 
  COMPILER_FENCE;
  STORE_PTR_REL( &(p->f), &WRAP_##NAME );
}

static void WRAP_##NAME(Task *__dq_top, TD_##NAME *t)
{
  $SAVE_RVAL CALL_##NAME( __dq_top$TASK_GET_FROM_t );
}

inline $RTYPE SYNC_##NAME(Task *__dq_top)
{
  Task *q = __dq_top;
  void (*f)(Task *, Task *) = T_BUSY; /* For exchg sync */
  balarm_t a = NOT_STOLEN;            /* For THE sync */
  WOOL_WHEN_MSPAN( hrtime_t e_span; )

  if( ! q->stealable ) {
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */
    $RES_FIELD
    PR_INC( get_self( q ), CTR_inlined );

    WOOL_WHEN_MSPAN( e_span =  __wool_update_time(); )
    WOOL_WHEN_MSPAN( __wool_set_span( t->spawn_span ); )

    $ASSIGN_RES CALL_##NAME( __dq_top$TASK_GET_FROM_t );

    WOOL_WHEN_MSPAN( hrtime_t c_span = __wool_update_time(); )
    WOOL_WHEN_MSPAN( hrtime_t one_span = e_span - t->spawn_span; )
    WOOL_WHEN_MSPAN( hrtime_t two_span = c_span - e_span; )
    WOOL_WHEN_MSPAN( if( __wool_sc > one_span || __wool_sc > two_span ) )
    WOOL_WHEN_MSPAN(   __wool_set_span( c_span + one_span ); )
    WOOL_WHEN_MSPAN( else if( c_span < e_span ) __wool_set_span( e_span+__wool_sc ); )

    return $RES_VAR;
  }
  /*logEvent( get_self( q ), 6 );*/
  if( WOOL_READ_STOLEN ) {
    if( ( THE_SYNC && q->balarm != NOT_STOLEN ) || ( !THE_SYNC && q->f <= T_LAST ) ) {
      wool_sync( __dq_top, q->balarm );
      return $RETURN_RES;
    }
  }
  if( THE_SYNC ) {
    q->f = T_BUSY;
    MFENCE;
    a = q->balarm;
  } else {
    EXCHANGE( f, q->f );
  }

  if( ( THE_SYNC && 
        ( a == NOT_STOLEN || ( a = sync_get_balarm( q ) ) == NOT_STOLEN ) )
      ||
      ( ! THE_SYNC && f > T_LAST ) ) {
    TD_##NAME *t = (TD_##NAME *) q; /* Used in TASK_GET_FROM_t */
    /* Not stolen, nobody else might be using it */
    PR_INC( get_self( q ), CTR_inlined );
    return CALL_##NAME( __dq_top$TASK_GET_FROM_t );
  } else {
    wool_sync( __dq_top, a );
    return $RETURN_RES;
  }
}

$RTYPE CALL_##NAME(Task *__dq_top$TASK_ARGS)" \
) | awk '{printf "%-70s\\\n", $0 }'

echo " "
echo " "

done
fi

if ((r < $1-1)); then
(\
echo "\
#define LOOP_BODY_$r(NAME, COST, IXTY, IXNAME$MACRO_ARGS)

static unsigned long const __min_iters__##NAME 
   = COST > FINEST_GRAIN ? 1 : FINEST_GRAIN / ( COST ? COST : 20 );

inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME$TASK_ARGS);

VOID_TASK_$((r+2))(TREE_##NAME, IXTY, __from, IXTY, __to$MACRO_ARGS)
{
  if( __to - __from <= __min_iters__##NAME ) {
    IXTY __i;
    for( __i = __from; __i < __to; __i++ ) {
      LOOP_##NAME( __dq_top, __i$CALL_ARGS );
    }
  } else {
    IXTY __mid = (__from + __to) / 2;
    SPAWN( TREE_##NAME, __mid, __to$CALL_ARGS );
    CALL( TREE_##NAME, __from, __mid$CALL_ARGS );
    SYNC( TREE_##NAME );
  }
}

inline void LOOP_##NAME(Task *__dq_top, IXTY IXNAME$TASK_ARGS)"\
) | awk '{printf "%-70s\\\n", $0 }'

fi
done
