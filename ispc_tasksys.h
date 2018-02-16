/*
  Copyright (c) 2011-2018, Intel Corporation, Next Limit
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of Intel Corporation nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.


   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
   IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
   PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
*/

/*
  This file implements simple task systems that provide the three
  entrypoints used by ispc-generated to code to handle 'launch' and 'sync'
  statements in ispc programs.  See the section "Task Parallelism: Language
  Syntax" in the ispc documentation for information about using task
  parallelism in ispc programs, and see the section "Task Parallelism:
  Runtime Requirements" for information about the task-related entrypoints
  that are implemented here.

  There are several task systems in this file, built using:
    - Microsoft's Concurrency Runtime (ISPC_USE_CONCRT)
    - Apple's Grand Central Dispatch (ISPC_USE_GCD)
    - bare pthreads (ISPC_USE_PTHREADS, ISPC_USE_PTHREADS_FULLY_SUBSCRIBED)
    - Cilk Plus (ISPC_USE_CILK)
    - TBB (ISPC_USE_TBB_TASK_GROUP, ISPC_USE_TBB_PARALLEL_FOR)
    - OpenMP (ISPC_USE_OMP)
    - HPX (ISPC_USE_HPX)

  The task system implementation can be selected at compile time, by defining 
  the appropriate preprocessor symbol on the command line (for e.g.: -D ISPC_USE_TBB).
  Not all combinations of platform and task system are meaningful.
  If no task system is requested, a reasonable default task system for the platform
  is selected.  Here are the task systems that can be selected:

#define ISPC_USE_GCD
#define ISPC_USE_CONCRT
#define ISPC_USE_PTHREADS
#define ISPC_USE_PTHREADS_FULLY_SUBSCRIBED
#define ISPC_USE_CILK
#define ISPC_USE_OMP
#define ISPC_USE_TBB_TASK_GROUP
#define ISPC_USE_TBB_PARALLEL_FOR

  The ISPC_USE_PTHREADS_FULLY_SUBSCRIBED model essentially takes over the machine
  by assigning one pthread to each hyper-thread, and then uses spinlocks and atomics
  for task management.  This model is useful for KNC where tasks can take over 
  the machine, but less so when there are other tasks that need running on the machine.

#define ISPC_USE_CREW
#define ISPC_USE_HPX
  The HPX model requires the HPX runtime environment to be set up. This can be
  done manually, e.g. with hpx::init, or by including hpx/hpx_main.hpp which
  uses the main() function as entry point and sets up the runtime system.
  Number of threads can be specified as commandline parameter with
  --hpx:threads, use "all" to spawn one thread per processing unit.

*/

#ifndef ISPC_TASKSYS_H
#define ISPC_TASKSYS_H

#include "ispc_version.h"

//#define ISPC_USE_GCD
//#define ISPC_USE_CONCRT
//#define ISPC_USE_PTHREADS
//#define ISPC_USE_PTHREADS_FULLY_SUBSCRIBED
//#define ISPC_USE_CILK
//#define ISPC_USE_OMP
//#define ISPC_USE_TBB_TASK_GROUP
#define ISPC_USE_TBB_PARALLEL_FOR

#ifdef ISPC_LIBISPC_ENABLED

_ISPC_BEGIN

/** Add external symbols into LLVM to allow resolution at runtime (RuntimeDyld). */
void ispcJitAddExplicitSymbols();

_ISPC_END

#endif // ISPC_LIBISPC_ENABLED
#endif // ISPC_TASKSYS_H
