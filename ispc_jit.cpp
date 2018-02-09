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

/** @file ispc_jit.cpp
@brief ispc jit definitions.
*/

#include "ispc_jit.h"

#ifdef ISPC_LLVM_ORC

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/LambdaResolver.h>

_ISPC_BEGIN

IspcJIT::IspcJIT() :
    TM(llvm::EngineBuilder().selectTarget()),
    DL(TM->createDataLayout()) {
    memMgr = std::make_shared<llvm::SectionMemoryManager>();

    std::function<std::shared_ptr<llvm::RuntimeDyld::MemoryManager>()>
        getMemMgr = [&]() { return memMgr; };

    linkLayer = new LinkLayer(getMemMgr);
    compiler = new Compiler(*TM);    compileLayer = new CompileLayer(*linkLayer, *compiler);
}


IspcJIT::~IspcJIT() {
    delete compileLayer;
    delete compiler;
    delete linkLayer;
}


llvm::TargetMachine &IspcJIT::getTargetMachine() {
    return *TM;
}


IspcJIT::ModuleHandle IspcJIT::addModule(llvm::Module *M) {
    auto resolver = llvm::orc::createLambdaResolver(
        [&](const std::string &name) {
          if (auto sym = compileLayer->findSymbol(name, false))
            return sym;
          return llvm::JITSymbol(nullptr);
        },
        [](const std::string &name) {
          if (auto symAddr =
                llvm::RTDyldMemoryManager::getSymbolAddressInProcess(name))
            return llvm::JITSymbol(symAddr, llvm::JITSymbolFlags::Exported);
          return llvm::JITSymbol(nullptr);
        });

    std::shared_ptr<llvm::Module> sModule(M);

    return llvm::cantFail(compileLayer->addModule(std::move(sModule),
                                                  std::move(resolver)));
}


llvm::JITSymbol IspcJIT::findSymbol(llvm::StringRef name) {
    return compileLayer->findSymbol(name, true);
}


void IspcJIT::removeModule(ModuleHandle H) {
    llvm::cantFail(compileLayer->removeModule(H));
}

_ISPC_END

#endif // ISPC_LLVM_ORC
