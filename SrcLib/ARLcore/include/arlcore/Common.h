/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_COMMMON_H
#define _ARLCORE_COMMMON_H

#include <float.h>
#include <arlcore/Type.h>

    /**
     * @author  IRCAD (Research and Development Team)
     * @date    2007
     * @brief   Directives de compilation
     * Definition de constantes
     */
    #ifndef _WIN32
        #ifndef CLK_TCK
            #define CLK_TCK 1000.0
        #endif
    #endif //_WIN32

    // COMPILATION OPTIONS
    #define ANN // Lib=ANN.lib
    //#undef ANN

//  #define ARL_VERBOSE ARL_LOG_LEVEL_NONE
    #define TMP_FILE "tmp.plt" // "c:/tmp/tmp.txt" // "/tmp/tmp.txt"
    #define TMP_DIR "./" //"c:/tmp/" // "/tmp/"
    #ifdef _WIN32
    //  #define GNUPLOT_EXE "d:/gnuplot/bin/pgnuplot <tmp.txt"
        //#define GNUPLOT_EXE "d:/gnuplot/bin/pgnuplot <tmp.txt"
        #define GNUPLOT_EXE "d:/gnuplot/bin/wgnuplot tmp.plt"
        //#define GNUPLOT_EXE "d:/gnuplot/bin/wgnuplot c:/tmp/tmp.txt"
    #else // _WIN32
        #define GNUPLOT_EXE "/usr/bin/gnuplot /tmp/tmp.plt"
    #endif // _WIN32
    #define GRAPHVIZ_EXE "\"c:/Program Files/ATT/Graphviz/bin/dotty.exe\""
    #define GRAPHVIZ_TMP "tmpGraph.dot"

#ifndef _ARLCORE_CONFIG_HPP_
#ifdef _WIN32
        #ifdef ARLCORE_EXPORTS
        #define ARLCORE_API __declspec(dllexport)
        #else // ARLCORE_EXPORTS
        #define ARLCORE_API __declspec(dllimport)
        #endif // ARLCORE_EXPORTS
        #define ARLCORE_CLASS_API
        #pragma warning(disable: 4290)
    #pragma warning(disable: 4786)
#elif defined(__GNUC__) && (__GNUC__>=4) && defined(__USE_DYLIB_VISIBILITY__)
        #ifdef ARLCORE_EXPORTS
        #define ARLCORE_API __attribute__ ((visibility("default")))
        #define ARLCORE_CLASS_API __attribute__ ((visibility("default")))
        #else
        #define ARLCORE_API __attribute__ ((visibility("hidden")))
        #define ARLCORE_CLASS_API __attribute__ ((visibility("hidden")))
        #endif
#else // _WIN32
        #define ARLCORE_API
        #define ARLCORE_CLASS_API
#endif // _WIN32
#endif // _ARLCORE_CONFIG_HPP_

#endif // _ARLCORE_COMMMON_H
