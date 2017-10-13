/**
********************************************************************************
\file   oplk/targetdefs/linux.h

\brief  Target definitions for POSIX

This file contains target definitions for POSIX systems
*******************************************************************************/

/*------------------------------------------------------------------------------
Copyright (c) 2017, embedded brains GmbH
Copyright (c) 2016, Kalycito Infotech Private Limited
Copyright (c) 2016, Bernecker+Rainer Industrie-Elektronik Ges.m.b.H. (B&R)
Copyright (c) 2013, SYSTEC electronic GmbH
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
#ifndef _INC_oplk_targetdefs_posix_H_
#define _INC_oplk_targetdefs_posix_H_

//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>

#include <oplk/basictypes.h>

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

#define OPLKDLLEXPORT

#define INLINE

#define OPLK_FILE_HANDLE            int

#define UNUSED_PARAMETER(par)       (void)par

#define PRINTF(...)                 printf(__VA_ARGS__)

// Target IO functions
// - Write
#define OPLK_IO_WR8(addr, val)      (*(volatile UINT8*)(addr)) = (val)
#define OPLK_IO_WR16(addr, val)     (*(volatile UINT16*)(addr)) = (val)
#define OPLK_IO_WR32(addr, val)     (*(volatile UINT32*)(addr)) = (val)
// - Read
#define OPLK_IO_RD8(addr)           (*(volatile UINT8*)(addr))
#define OPLK_IO_RD16(addr)          (*(volatile UINT16*)(addr))
#define OPLK_IO_RD32(addr)          (*(volatile UINT32*)(addr))

// Target data cache functions
#define OPLK_DCACHE_FLUSH(addr, len)        ((void)0)
#define OPLK_DCACHE_INVALIDATE(addr, len)   ((void)0)

// Target memory barrier function
#define OPLK_MEMBAR()               ((void)0)

// Target lock
#define OPLK_LOCK_T                 UINT8
#define OPLK_ATOMIC_T               UINT8

#define OPLK_ATOMIC_EXCHANGE(address, newval, oldval) \
    oldval = __sync_lock_test_and_set(address, newval);

#define OPLK_MUTEX_T                sem_t*

#define O_BINARY                        0
#define MAX_PATH_LEN    PATH_MAX

#endif /* _INC_oplk_targetdefs_posix_H_ */
