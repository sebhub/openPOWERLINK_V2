/**
********************************************************************************
\file   timer-rtems.c

\brief  Implementation of user timer module using the RTEMS timer interface

This file contains an implementation of the user timer module which uses the
RTEMS timer interface implementing the functionality.

\ingroup module_timeru
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
#include <user/timeru.h>
#include <user/eventu.h>

#include <rtems.h>
#include <rtems/chain.h>

#include <assert.h>
#include <pthread.h>

//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
typedef struct
{
    rtems_chain_node node;
    rtems_id         id;
    tTimerArg        timerArg;
} tTimerEntry;

typedef struct
{
    pthread_mutex_t     lock;
    rtems_chain_control freeEntries;
} tTimeruInstance;

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static tTimeruInstance timeruInstance_l = {
    .lock = PTHREAD_MUTEX_INITIALIZER,
    .freeEntries = RTEMS_CHAIN_INITIALIZER_EMPTY(timeruInstance_l.freeEntries)
};

static tTimerEntry timeruEntries_l[TIMERU_MAX_ENTRIES];

//------------------------------------------------------------------------------
// local function prototypes
//------------------------------------------------------------------------------
void timeruLock(void);
void timeruUnlock(void);
void timeruHandler(rtems_id, void*);
tOplkError timeruFire(tTimerEntry*, ULONG, const tTimerArg*);

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief  Initialize user timers

The function initializes the user timer module.

\return The function returns a tOplkError error code.

\ingroup module_timeru
*/
//------------------------------------------------------------------------------
tOplkError timeru_init(void)
{
    size_t i;

    for (i = 0; i < TIMERU_MAX_ENTRIES; ++i) {
        rtems_status_code sc;

        sc = rtems_timer_create(rtems_build_name('O', 'P', 'L', 'K'), &timeruEntries_l[i].id);
        if (sc != RTEMS_SUCCESSFUL)
            return kErrorNoResource;

        rtems_chain_append_unprotected(&timeruInstance_l.freeEntries, &timeruEntries_l[i].node);
    }

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Shutdown user timer

The function shuts down the user timer instance.

\return The function returns a tOplkError error code.

\ingroup module_timeru
*/
//------------------------------------------------------------------------------
tOplkError timeru_exit(void)
{

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  User timer process function

This function must be called repeatedly from within the application. It checks
whether a timer has expired.

\return The function returns a tOplkError error code.

\ingroup module_timeru
*/
//------------------------------------------------------------------------------
tOplkError timeru_process(void)
{
    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Create and set a timer

This function creates a timer, sets up the timeout and saves the
corresponding timer handle.

\param[out]     pTimerHdl_p         Pointer to store the timer handle.
\param[in]      timeInMs_p          Timeout in milliseconds.
\param[in]      pArgument_p         Pointer to user definable argument for timer.

\return The function returns a tOplkError error code.

\ingroup module_timeru
*/
//------------------------------------------------------------------------------
tOplkError timeru_setTimer(tTimerHdl* pTimerHdl_p,
                           ULONG timeInMs_p,
                           const tTimerArg* pArgument_p)
{
    tTimerEntry *pNewEntry;

    if (pTimerHdl_p == NULL)
        return kErrorTimerInvalidHandle;

    timeruLock();
    pNewEntry = (tTimerEntry *)rtems_chain_get_unprotected(&timeruInstance_l.freeEntries);
    timeruUnlock();

    *pTimerHdl_p = (tTimerHdl)pNewEntry;
    if (pNewEntry == NULL)
        return kErrorTimerNoTimerCreated;

    return timeruFire(pNewEntry, timeInMs_p,  pArgument_p);
}

//------------------------------------------------------------------------------
/**
\brief  Modifies an existing timer

This function modifies an existing timer. If the timer was not yet created
it creates the timer and stores the new timer handle at \p pTimerHdl_p.

\param[in,out]  pTimerHdl_p         Pointer to store the timer handle.
\param[in]      timeInMs_p          Timeout in milliseconds.
\param[in]      pArgument_p         Pointer to user definable argument for timer.

\return The function returns a tOplkError error code.

\ingroup module_timeru
*/
//------------------------------------------------------------------------------
tOplkError timeru_modifyTimer(tTimerHdl* pTimerHdl_p,
                              ULONG timeInMs_p,
                              const tTimerArg* pArgument_p)
{
    tTimerEntry*      pTimerEntry;
    rtems_status_code sc;

    if (pTimerHdl_p == NULL)
        return kErrorTimerInvalidHandle;

    pTimerEntry = (tTimerEntry *)*pTimerHdl_p;
    if (pTimerEntry == NULL) {
        return timeru_setTimer(pTimerHdl_p, timeInMs_p, pArgument_p);
    }

    sc = rtems_timer_cancel(pTimerEntry->id);
    assert(sc == RTEMS_SUCCESSFUL);

    return timeruFire(pTimerEntry, timeInMs_p,  pArgument_p);
}

//------------------------------------------------------------------------------
/**
\brief  Delete a timer

This function deletes an existing timer.

\param[in,out]  pTimerHdl_p         Pointer to timer handle of timer to delete.

\return The function returns a tOplkError error code.
\retval kErrorTimerInvalidHandle    An invalid timer handle was specified.
\retval kErrorOk                    The timer is deleted.

\ingroup module_timeru
*/
//------------------------------------------------------------------------------
tOplkError timeru_deleteTimer(tTimerHdl* pTimerHdl_p)
{
    tTimerEntry*      pTimerEntry;
    rtems_status_code sc;

    if (pTimerHdl_p == NULL)
        return kErrorTimerInvalidHandle;

    pTimerEntry = (tTimerEntry *)*pTimerHdl_p;
    if (pTimerEntry == NULL)
        return kErrorOk;

    sc = rtems_timer_cancel(pTimerEntry->id);
    assert(sc == RTEMS_SUCCESSFUL);

    timeruLock();
    rtems_chain_append_unprotected(&timeruInstance_l.freeEntries, &pTimerEntry->node);
    timeruUnlock();

    return kErrorOk;
}

//============================================================================//
//            P R I V A T E   F U N C T I O N S                               //
//============================================================================//
/// \name Private Functions
/// \{
void timeruLock(void)
{
    pthread_mutex_lock(&timeruInstance_l.lock);
}

void timeruUnlock(void)
{
    pthread_mutex_unlock(&timeruInstance_l.lock);
}

void timeruHandler(rtems_id id, void *arg)
{
    tTimerEntry*   pTimerEntry;
    tEvent         event;
    tTimerEventArg timerEventArg;

    pTimerEntry = arg;
    timerEventArg.timerHdl.handle = (tTimerHdl)pTimerEntry;
    OPLK_MEMCPY(&timerEventArg.argument, &pTimerEntry->timerArg.argument, sizeof(timerEventArg.argument));

    event.eventSink = pTimerEntry->timerArg.eventSink;
    event.eventType = kEventTypeTimer;
    OPLK_MEMSET(&event.netTime, 0x00, sizeof(tNetTime));
    event.eventArg.pEventArg = &timerEventArg;
    event.eventArgSize = sizeof(timerEventArg);

    eventu_postEvent(&event);
}

tOplkError timeruFire(tTimerEntry* pTimerEntry_p, ULONG timeInMs_p, const tTimerArg* pArgument_p)
{
    ULONG             msPerTick;
    ULONG             timeoutInTicks;
    rtems_status_code sc;

    OPLK_MEMCPY(&pTimerEntry_p->timerArg, pArgument_p, sizeof(tTimerArg));
    msPerTick = rtems_configuration_get_milliseconds_per_tick();
    timeoutInTicks = (timeInMs_p + msPerTick - 1) / msPerTick;
    sc = rtems_timer_server_fire_after(pTimerEntry_p->id, timeoutInTicks, timeruHandler, pTimerEntry_p);
    assert(sc == RTEMS_SUCCESSFUL);

    return kErrorOk;
}
/// \}
