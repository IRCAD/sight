/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_PLUGIN_HPP__
#define __UIMEDDATAQT_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

namespace uiMedDataQt
{

/**
 * 
 * @date    2009-2013.
 */
struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() throw();

    // Overrides
    void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    void stop() throw();

};

} // namespace uiMedDataQt

#endif //__UIMEDDATAQT_PLUGIN_HPP__
