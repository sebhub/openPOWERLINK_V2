/**
********************************************************************************
\file   hrestimer-rtems.c

\brief  High-resolution timer module for RTEMS

This module is the target specific implementation of the high-resolution
timer module for RTEMS.

\ingroup module_hrestimer
*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2017, embedded brains GmbH
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holders nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <common/oplkinc.h>
#include <kernel/hrestimer.h>

#include <rtems.h>
#include <rtems/chain.h>

#include <assert.h>
#include <pthread.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//          P R I V A T E   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define HRESTIMER_MAX_ENTRIES 2

//------------------------------------------------------------------------------
// local types
typedef struct
{
    rtems_chain_node node;
    rtems_id         id;
    tTimerEventArg   eventArg;
    tTimerkCallback  pfnCallback;
    rtems_interval   intervalInTicks;
} tHresTimerEntry;

typedef struct
{
    pthread_mutex_t     lock;
    rtems_chain_control freeEntries;
} tHresTimerInstance;

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static tHresTimerInstance hresTimerInstance_l = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .freeEntries = RTEMS_CHAIN_INITIALIZER_EMPTY(hresTimerInstance_l.freeEntries)
};

static tHresTimerEntry hresTimerEntries_l[TIMERU_MAX_ENTRIES];

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
void hresTimerLock(void);
void hresTimerUnlock(void);
void hresTimerHandler(rtems_id, void*);
//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief    Initialize high-resolution timer module

The function initializes the high-resolution timer module

\return Returns a tOplkError error code.

\ingroup module_hrestimer
*/
//------------------------------------------------------------------------------
tOplkError hrestimer_init(void)
{
    size_t i;

    for (i = 0; i < HRESTIMER_MAX_ENTRIES; ++i) {
        rtems_status_code sc;

        sc = rtems_timer_create(rtems_build_name('O', 'P', 'L', 'K'), &hresTimerEntries_l[i].id);
        if (sc != RTEMS_SUCCESSFUL)
            return kErrorNoResource;

        rtems_chain_append_unprotected(&hresTimerInstance_l.freeEntries, &hresTimerEntries_l[i].node);
        hresTimerEntries_l[i].eventArg.timerHdl.handle = (tTimerHdl)&hresTimerEntries_l[i];
    }

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief    Shut down high-resolution timer module

The function shuts down the high-resolution timer module.

\return Returns a tOplkError error code.

\ingroup module_hrestimer
*/
//------------------------------------------------------------------------------
tOplkError hrestimer_exit(void)
{

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief    Modify a high-resolution timer

The function modifies the timeout of the timer with the specified handle.
If the handle to which the pointer points to is zero, the timer must be created
first. If it is not possible to stop the old timer, this function always assures
that the old timer does not trigger the callback function with the same handle
as the new timer. That means the callback function must check the passed handle
with the one returned by this function. If these are unequal, the call can be
discarded.

\param[in,out]  pTimerHdl_p         Pointer to timer handle.
\param[in]      time_p              Relative timeout in [ns].
\param[in]      pfnCallback_p       Callback function, which is called when timer expires.
                                    (The function is called mutually exclusive with
                                    the Edrv callback functions (Rx and Tx)).
\param[in]      argument_p          User-specific argument.
\param[in]      fContinue_p         If TRUE, the callback function will be called continuously.
                                    Otherwise, it is a one-shot timer.

\return Returns a tOplkError error code.

\ingroup module_hrestimer
*/
//------------------------------------------------------------------------------
tOplkError hrestimer_modifyTimer(tTimerHdl* pTimerHdl_p,
                                 ULONGLONG time_p,
                                 tTimerkCallback pfnCallback_p,
                                 ULONG argument_p,
                                 BOOL fContinue_p)
{
    tHresTimerEntry*  pHresTimerEntry;
    ULONG             nsPerTick;
    ULONG             timeInTicks;
    rtems_status_code sc;

    if (pTimerHdl_p == NULL)
        return kErrorTimerInvalidHandle;

    pHresTimerEntry = (tHresTimerEntry *)*pTimerHdl_p;
    if (pHresTimerEntry == NULL) {
        hresTimerLock();
        pHresTimerEntry = (tHresTimerEntry *)rtems_chain_get_unprotected(&hresTimerInstance_l.freeEntries);
        hresTimerUnlock();

        *pTimerHdl_p = (tTimerHdl)pHresTimerEntry;
        if (pHresTimerEntry == NULL)
            return kErrorTimerNoTimerCreated;
    } else {
        sc = rtems_timer_cancel(pHresTimerEntry->id);
        assert(sc == RTEMS_SUCCESSFUL);
    }

    nsPerTick = rtems_configuration_get_nanoseconds_per_tick();
    timeInTicks = (time_p + nsPerTick - 1) / nsPerTick;

    pHresTimerEntry->eventArg.argument.value = argument_p;
    pHresTimerEntry->pfnCallback = pfnCallback_p;

    if (fContinue_p) {
        pHresTimerEntry->intervalInTicks = timeInTicks;
    } else {
        pHresTimerEntry->intervalInTicks = 0;
    }

    sc = rtems_timer_server_fire_after(pHresTimerEntry->id, timeInTicks, hresTimerHandler, pHresTimerEntry);
    assert(sc == RTEMS_SUCCESSFUL);

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief    Delete a high-resolution timer

The function deletes a created high-resolution timer. The timer is specified
by its timer handle. After deleting, the handle is reset to zero.

\param[in,out]  pTimerHdl_p         Pointer to timer handle.

\return Returns a tOplkError error code.

\ingroup module_hrestimer
*/
//------------------------------------------------------------------------------
tOplkError hrestimer_deleteTimer(tTimerHdl* pTimerHdl_p)
{
    tHresTimerEntry*  pHresTimerEntry;
    rtems_status_code sc;

    if (pTimerHdl_p == NULL)
        return kErrorTimerInvalidHandle;

    pHresTimerEntry = (tHresTimerEntry *)*pTimerHdl_p;
    if (pHresTimerEntry == NULL)
        return kErrorOk;

    sc = rtems_timer_cancel(pHresTimerEntry->id);
    assert(sc == RTEMS_SUCCESSFUL);

    hresTimerLock();
    rtems_chain_append_unprotected(&hresTimerInstance_l.freeEntries, &pHresTimerEntry->node);
    hresTimerUnlock();

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Control external synchronization interrupt

This function enables/disables the external synchronization interrupt. If the
external synchronization interrupt is not supported, the call is ignored.

\param[in]      fEnable_p           Flag determines if sync should be enabled or disabled.

\ingroup module_hrestimer
*/
//------------------------------------------------------------------------------
void hrestimer_controlExtSyncIrq(BOOL fEnable_p)
{
    UNUSED_PARAMETER(fEnable_p);
}

//------------------------------------------------------------------------------
/**
\brief  Set external synchronization interrupt time

This function sets the time when the external synchronization interrupt shall
be triggered to synchronize the host processor. If the external synchronization
interrupt is not supported, the call is ignored.

\param[in]      time_p              Time when the sync shall be triggered

\ingroup module_hrestimer
*/
//------------------------------------------------------------------------------
void hrestimer_setExtSyncIrqTime(tTimestamp time_p)
{
    UNUSED_PARAMETER(time_p);
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{
void hresTimerLock(void)
{
    pthread_mutex_lock(&hresTimerInstance_l.lock);
}

void hresTimerUnlock(void)
{
    pthread_mutex_unlock(&hresTimerInstance_l.lock);
}

void hresTimerHandler(rtems_id id, void *arg)
{
    tHresTimerEntry*  pHresTimerEntry;
    rtems_status_code sc;

    pHresTimerEntry = arg;

    if (pHresTimerEntry->intervalInTicks != 0) {
        sc = rtems_timer_reset(id);
        assert(sc == RTEMS_SUCCESSFUL);
    }

    pHresTimerEntry->pfnCallback(&pHresTimerEntry->eventArg);
}
/// \}
