/*
Copyright (c) 2018, Next Limit
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


/** @file ispc_jit.h
    @brief Main ispc jit header file.
*/

#ifndef ISPC_JIT_H
#define ISPC_JIT_H

#include "ispc.h"
#ifdef ISPC_LLVM_ORC

#include <memory>

#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

namespace llvm {

class SectionMemoryManager;

namespace orc {

template <typename BaseLayerT, typename CompileFtor> class IRCompileLayer;
class RTDyldObjectLinkingLayer;
class SimpleCompiler;

} // end namespace orc
} // end namespace llvm

_ISPC_BEGIN

/** @brief ISPC JIT interface.
*/
class IspcJIT {
private:
    typedef llvm::orc::RTDyldObjectLinkingLayer LinkLayer;
    typedef llvm::orc::SimpleCompiler Compiler;
    typedef llvm::orc::IRCompileLayer<LinkLayer, Compiler> CompileLayer;

public:
    typedef CompileLayer::ModuleHandleT ModuleHandle;

  /** The IspcJIT constructor. */
    IspcJIT();

    /** The IspcJIT destructor. */
    ~IspcJIT();

    /** Returns the target machine. */
    llvm::TargetMachine &getTargetMachine();

    /** Add module. */
    ModuleHandle addModule(llvm::Module *module);

    /** Remove module. */
    void removeModule(ModuleHandle H);

    /** Find JITSymbol. */
    llvm::JITSymbol findSymbol(llvm::StringRef name);

private:
    /** The target machine. */
    std::unique_ptr<llvm::TargetMachine> TM;
    /** The data layout. */
    const llvm::DataLayout DL;

    /** Compiler. */
    Compiler *compiler;    /** Compile layer. */
    CompileLayer *compileLayer;

    /** Link layer. */
    LinkLayer *linkLayer;

    /** Memory manager. */
    std::shared_ptr<llvm::SectionMemoryManager> memMgr;
};

_ISPC_END

#endif // ISPC_LLVM_ORC
#endif // ISPC_JIT_H
