/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <core/base.hpp>

#include "installSIGSEVBacktrace.hpp"

#ifndef WIN32

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <execinfo.h>

#include <cstring>
#include <cxxabi.h>
#include <iostream>
#include <sstream>
#include <string>

/* get REG_EIP/REG_RIP from ucontext.h */
#include <ucontext.h>

#if defined(REG_EIP)
constexpr int debugReg = REG_EIP;
#elif defined(REG_RIP)
constexpr int debugReg = REG_RIP;
#else
#error Neither REG_EIP nor REG_RIP is defined.
#endif

#else // WIN32

#include <list>
#include <string>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <Windows.h>
#include <DbgHelp.h>

#endif

namespace sight::module::debug
{

#ifndef WIN32
//------------------------------------------------------------------------------

std::string demangle(const std::string& mangled)
{
    return core::Demangler(mangled).getClassname();
}

//------------------------------------------------------------------------------

std::string decode(char* message)
{
    std::string msg(message);
    std::string res(message);

    std::string::size_type popen = msg.find('(');
    if(popen != std::string::npos)
    {
        std::string::size_type plus = msg.find('+');
        res = std::string(message, popen + 1) + " ";
        std::string mangled(message, popen + 1, plus - popen - 1);
        res += demangle(mangled) + " ";
        res += std::string(message + plus, message + strlen(message));
    }

    return res;
}

//------------------------------------------------------------------------------

void btSighandler(
    int sig,
    siginfo_t* info,
    void* secret
)
{
    std::array<void*, 16> trace {};
    int i          = 0;
    int trace_size = 0;
    const auto* uc = reinterpret_cast<const ucontext_t*>(secret);

    std::stringstream ss;
    ss << "Got signal " << sig;

    /* Do something useful with siginfo_t */
    if(sig == SIGSEGV)
    {
        ss << " faulty address is " << info->si_addr;
        ss << " from " << uc->uc_mcontext.gregs[debugReg];
    }

    ss << std::endl;

    trace_size = backtrace(trace.data(), 16);
    /* overwrite sigaction with caller's address */
    trace[1] = reinterpret_cast<void*>(uc->uc_mcontext.gregs[debugReg]);

    char** messages = backtrace_symbols(trace.data(), trace_size);
    /* skip first stack frame (points here) */
    ss << "    [bt] Execution path:" << std::endl;
    for(i = 1 ; i < trace_size ; ++i)
    {
        ss << "    [bt] " << decode(messages[i]) << std::endl;
    }

    if(sig == SIGSEGV)
    {
        SIGHT_FATAL("SIGSEV signal " + ss.str());
        exit(0);
    }
    else
    {
        SIGHT_ERROR("SIGUSR1 signal " + ss.str());
    }
}

//------------------------------------------------------------------------------

void installSIGSEVBacktrace()
{
    struct sigaction sa {};

    sa.sa_sigaction = btSighandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_SIGINFO;

    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGUSR1, &sa, nullptr);
}
#else // if  win32
const std::size_t nbChar = 100;
#if _MSC_VER > 1499 // Visual C++ 2008 only
//------------------------------------------------------------------------------

BOOL CALLBACK EnumerateLoadedModules(LPCSTR ModuleName, DWORD64 ModuleBase, ULONG, PVOID UserContext)
#else
//------------------------------------------------------------------------------

BOOL CALLBACK EnumerateLoadedModules(LPSTR ModuleName, DWORD64 ModuleBase, ULONG, PVOID UserContext)
#endif
{
    std::list<std::string>* pLoadedModules = reinterpret_cast<std::list<std::string>*>(UserContext);
    pLoadedModules->push_back(std::string((char*) ModuleName) + "\t" + boost::lexical_cast<std::string>(ModuleBase));
    return true;
}

/**
 * Dumps the backtrace on a stream
 */
void printDump(
    std::list<std::string>& loadedModules,
    std::list<std::string>& callStack,
    std::list<std::string>& fileStack
)
{
    std::stringstream stream;

    // Dumps the loaded modules on the stream
    stream << "-----------------------------------------" << std::endl;
    stream << "\nLoaded Modules\n";
    for(std::list<std::string>::const_iterator it = loadedModules.begin() ; it != loadedModules.end() ; ++it)
    {
        stream << "> " << *it << std::endl;
    }

    stream << "-----------------------------------------" << std::endl;
    // Dumps the call stack on the stream
    stream << "\nCallStack\n";
    for(std::list<std::string>::const_iterator it = callStack.begin(), it2 = fileStack.begin() ;
        it != callStack.end() && it2 != fileStack.end() ; ++it, ++it2)
    {
        stream << "> " << *it << std::endl;
        stream << "   " << *it2 << std::endl;
    }

    stream << "-----------------------------------------" << std::endl;

    SIGHT_ERROR(stream.str());
}

/**
 * Loads the elements of the call stack in a list
 * @param exceptionInfos are useful information on the exception
 */
void LoadCallStack(
    EXCEPTION_POINTERS* exceptionInfos,
    HANDLE& hProcess,
    std::list<std::string>& callStack,
    std::list<std::string>& fileStack
)
{
    STACKFRAME64 tempStackFrame;
    CONTEXT context = *(exceptionInfos->ContextRecord);
    memset(&tempStackFrame, 0, sizeof(STACKFRAME64));
    DWORD machineType;

#ifdef _M_IX86
    machineType                     = IMAGE_FILE_MACHINE_I386;
    tempStackFrame.AddrPC.Offset    = context.Eip;
    tempStackFrame.AddrPC.Mode      = AddrModeFlat;
    tempStackFrame.AddrStack.Offset = context.Esp;
    tempStackFrame.AddrStack.Mode   = AddrModeFlat;
    tempStackFrame.AddrFrame.Offset = context.Ebp;
    tempStackFrame.AddrFrame.Mode   = AddrModeFlat;
#elif _M_X64
    machineType                     = IMAGE_FILE_MACHINE_AMD64;
    tempStackFrame.AddrPC.Offset    = context.Rip;
    tempStackFrame.AddrPC.Mode      = AddrModeFlat;
    tempStackFrame.AddrFrame.Offset = context.Rsp;
    tempStackFrame.AddrFrame.Mode   = AddrModeFlat;
    tempStackFrame.AddrStack.Offset = context.Rsp;
    tempStackFrame.AddrStack.Mode   = AddrModeFlat;
#elif _M_IA64
    machineType                      = IMAGE_FILE_MACHINE_IA64;
    tempStackFrame.AddrPC.Offset     = context.StIIP;
    tempStackFrame.AddrPC.Mode       = AddrModeFlat;
    tempStackFrame.AddrFrame.Offset  = context.IntSp;
    tempStackFrame.AddrFrame.Mode    = AddrModeFlat;
    tempStackFrame.AddrBStore.Offset = context.RsBSP;
    tempStackFrame.AddrBStore.Mode   = AddrModeFlat;
    tempStackFrame.AddrStack.Offset  = context.IntSp;
    tempStackFrame.AddrStack.Mode    = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

    ULONG64 buffer[(sizeof(SYMBOL_INFO) + nbChar * sizeof(TCHAR) + sizeof(ULONG64) + 1) / sizeof(ULONG64)];
    PSYMBOL_INFO pSymbol = reinterpret_cast<PSYMBOL_INFO>(buffer);
    PSTR undecoratedName = (PSTR) malloc(sizeof(TCHAR) * nbChar);

    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen   = nbChar;
    DWORD lineDisplacement;
    IMAGEHLP_LINE64 lineInfo = {sizeof(IMAGEHLP_LINE64)};

    while(StackWalk64(
              machineType,
              hProcess,
              GetCurrentThread(),
              &tempStackFrame,
              &context,
              nullptr,
              SymFunctionTableAccess64,
              SymGetModuleBase64,
              nullptr
    ))
    {
        // Sanity stack check
        if(tempStackFrame.AddrPC.Offset == 0)
        {
            break;
        }

        DWORD64 symDisplacement = 0;
        // Try to get the symbol name
        if(SymFromAddr(hProcess, tempStackFrame.AddrPC.Offset, &symDisplacement, pSymbol))
        {
            UnDecorateSymbolName(pSymbol->Name, undecoratedName, MAX_SYM_NAME, UNDNAME_COMPLETE);
            callStack.push_back(
                std::string((char*) undecoratedName) + "+"
                + boost::lexical_cast<std::string>(symDisplacement)
            );

            if(SymGetLineFromAddr64(hProcess, tempStackFrame.AddrPC.Offset, &lineDisplacement, &lineInfo))
            {
                fileStack.push_back(
                    std::string(lineInfo.FileName) + "\tl:"
                    + boost::lexical_cast<std::string>(lineInfo.LineNumber)
                );
            }
            else
            {
                fileStack.push_back("No info");
            }
        }
        else
        {
        }
    }

    free(undecoratedName);
}

/// The exception filter that will display the stack
static LONG WINAPI UnhandledExpFilter(PEXCEPTION_POINTERS pExceptionInfo)
{
    /// Handle to the process
    HANDLE hProcess;
    /// List of the loaded modules
    std::list<std::string> loadedModules;
    /// List of the elements on the call stack
    std::list<std::string> callStack;
    /// List of the elements on the file stack
    std::list<std::string> fileStack;

    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
    hProcess = GetCurrentProcess();

    if(SymInitialize(hProcess, nullptr, TRUE))
    {
        LoadCallStack(pExceptionInfo, hProcess, callStack, fileStack);
        ::EnumerateLoadedModules64(hProcess, EnumerateLoadedModules, &loadedModules);
        ::SymCleanup(hProcess);
    }

    printDump(loadedModules, callStack, fileStack);

    return EXCEPTION_CONTINUE_SEARCH;
}

//------------------------------------------------------------------------------

void installSIGSEVBacktrace()
{
    SetUnhandledExceptionFilter(UnhandledExpFilter);
}
#endif // ifndef WIN32

} // namespace sight::module::debug
