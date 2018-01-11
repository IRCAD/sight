/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPIMAGEFILTER_PLUGIN_HPP__
#define __OPIMAGEFILTER_PLUGIN_HPP__


#include <fwRuntime/Plugin.hpp>


namespace opImageFilter
{


/**
 * @brief   Implements a plugin for the opImageFilter bundle.
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


} // namespace opImageFilter


#endif //__OPIMAGEFILTER_PLUGIN_HPP__
