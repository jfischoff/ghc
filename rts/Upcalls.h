/* -----------------------------------------------------------------------------
 *
 * (c) The GHC Team, 2000-2009
 *
 * Upcall support -- Upcalls are typically pending IO actions (UPCALL_CLOSURE)
 * that are ALWAYS evaluated by capability's upcall_thread. Upcalls are created
 * when threads block on black holes, finalizers, etc. An upcall can itself
 * become blocked, in which case, its continuation is captured and stored as a
 * new upcall (UPCALL_STACK).
 *
 * ---------------------------------------------------------------------------*/

#ifndef UPCALLS_H
#define UPCALLS_H

#include "BeginPrivate.h"

typedef WSDeque UpcallQueue;
typedef StgClosure* Upcall;

// Initialization
UpcallQueue *allocUpcallQueue (void);

// Add a new upcall
void addUpcall                     (Capability* cap, Upcall uc);

Upcall getResumeThreadUpcall       (Capability* cap, StgTSO* tso);
Upcall getSwitchToNextThreadUpcall (Capability* cap, StgTSO* tso);
Upcall getFinalizerUpcall          (Capability* cap, StgTSO* tso);

// Get an upcall from the capability's upcall queue. This could be a IO ()
// action or a stack. Hence, immediately after picking up the upcall, check the
// upcall kind using isSuspendedUpcall ().
INLINE_HEADER StgClosure* popUpcallQueue (UpcallQueue* q);

INLINE_HEADER long upcallQueueSize (UpcallQueue *q);

//Upcall queue has at least 1 element || (upcall queue size == 0 &&
//cap->upcall_thread is the saved thread)
rtsBool pendingUpcalls (Capability* cap);

//If there are pending upcalls prepare the upcall thread with the first upcall.
//Save the given current thread in cap->upcall_thread.
StgTSO* prepareUpcallThread (Capability* cap, StgTSO* current_thread);

//If the given current thread is the upcall thread, swap it with the original
//current thread (saved under cap->upcall_thread), and return it.
StgTSO* restoreCurrentThreadIfNecessary (Capability* cap, StgTSO* current_thread);

INLINE_HEADER rtsBool isUpcallThread (StgTSO* tso);


void traverseUpcallQueue (evac_fn eval, void* user, Capability *cap);

/* -----------------------------------------------------------------------------
 * PRIVATE below here
 * -------------------------------------------------------------------------- */

INLINE_HEADER long upcallQueueSize (UpcallQueue* q)
{
  return dequeElements(q);
}

INLINE_HEADER StgClosure* popUpcallQueue (UpcallQueue* q)
{
  return popWSDeque (q);
}


INLINE_HEADER rtsBool isUpcallThread (StgTSO* tso)
{
  return tso->is_upcall_thread;
}

#include "EndPrivate.h"

#endif /* SPARKS_H */