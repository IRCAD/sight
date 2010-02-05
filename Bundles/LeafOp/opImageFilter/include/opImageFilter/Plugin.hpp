/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _OPIMAGEFILTER_PLUGIN_HPP_
#define _OPIMAGEFILTER_PLUGIN_HPP_


#include <fwRuntime/Plugin.hpp>


namespace opImageFilter
{


/**
 * @brief       Implements a plugin for the basicVRCtrl bundle.
 */
struct Plugin : public ::fwRuntime::Plugin
{

        /**
         * @brief       destructor
         */
        ~Plugin() throw();

        // Overrides
        void start() throw(::fwRuntime::RuntimeException);

        // Overrides
        void stop() throw();

};


} // namespace opImageFilter


#endif //_OPIMAGEFILTER_PLUGIN_HPP_
