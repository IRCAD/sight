/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCore/base.hpp"
#include <iostream>

int main()
{

        // In test.options, The debug level and specific logger is activated

        //--------------------------------------------------------------------------
        // SPY LOGGER MANAGER TEST
        //--------------------------------------------------------------------------
        SLM_TRACE("Affiche ce message de trace");
        SLM_DEBUG("Affiche ce message de debug");
        SLM_INFO("Affiche ce message d'information");
        SLM_WARN("Affiche ce message de warning");
        SLM_ERROR("Affiche ce message d'erreur");
        SLM_FATAL("Affiche ce message d'erreur fatale");

        //--------------------------------------------------------------------------
        // SPY LOGGER MANAGER TEST (OSTREAM)
        //--------------------------------------------------------------------------
        OSLM_TRACE("Affiche : " << "ce message de trace");
        OSLM_DEBUG("Affiche : " << "ce message de debug");
        OSLM_INFO("Affiche : " << "ce message d'information");
        OSLM_WARN("Affiche : " << "ce message de warning");
        OSLM_ERROR("Affiche : " << "ce message d'erreur");
        OSLM_FATAL("Affiche : " << "ce message d'erreur fatale");



        //--------------------------------------------------------------------------
        // SPY LOGGER TEST
        //--------------------------------------------------------------------------

        // Logger name : SpyLogger
        // Print message : DEBUG, INFO, WARN, ERROR, FATAL
        // message are writed on std::cout and in log.txt
        spyLog::SpyLogger logA;
        logA.addConsoleAppender();
        logA.addFileAppender("log.txt");

        SL_TRACE(logA, "Trace message");
        SL_DEBUG(logA, "Debug message");
        SL_INFO(logA, "Info message");
        SL_WARN(logA, "Warning message");
        SL_ERROR(logA, "Error message");
        SL_FATAL(logA, "Fatal error message");

        // Logger name : Other SpyLogger(WARN MODE)
        // Print message : WARN, ERROR, FATAL
        // message are writed on std::cout
        spyLog::SpyLogger logB ("OtherSpyLogger(WARN MODE)");
        logB.addConsoleAppender();
        logB.setLevel(spyLog::SpyLogger::SL_WARN);
        //logB.addSyslogAppender("monitor.ircad.fr","LOCAL7"); // host & syslog facility

        SL_TRACE(logB, "Trace message");                // This message is not printed
        SL_DEBUG(logB, "Debug message");                // This message is not printed
        SL_INFO(logB, "Info message");                  // This message is not printed
        SL_WARN(logB, "Warning message");
        SL_ERROR(logB, "Error message");
        SL_FATAL(logB, "Fatal error message");

}
