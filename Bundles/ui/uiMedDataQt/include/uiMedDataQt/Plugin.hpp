/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
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
    ~Plugin() noexcept;

    // Overrides
    void start();

    // Overrides
    void stop() noexcept;

};

} // namespace uiMedDataQt

#endif //__UIMEDDATAQT_PLUGIN_HPP__
