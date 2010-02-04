/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#if defined(__MACOSX__) && !defined(FWRUNTIME_API)

// Dynamic shared library (dylib) initialization routine
//   required to initialize static C++ objects bacause of lazy dynamic linking
//   http://developer.apple.com/techpubs/macosx/Essentials/
//          SystemOverview/Frameworks/Dynamic_Shared_Libraries.html

extern "C" {
    void __initialize_Cplusplus(void);
    void DylibInit(void);
};

void DylibInit()
{
    // The function __initialize_Cplusplus() must be called from the shared
    // library initialization routine to cause the static C++ objects in
    // the library to be initialized (reference number 2441683).

    // This only seems to be necessary if the library initialization routine
    // needs to use the static C++ objects
    __initialize_Cplusplus();

}

#endif
