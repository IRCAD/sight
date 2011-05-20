/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UITF_PLUGIN_HPP_
#define _UITF_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "uiTF/export.hpp"

namespace uiTF
{
/**
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 */
class UITF_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /**
     * @brief   destructor
     */
 UITF_API ~Plugin() throw();

    // Overrides
    UITF_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    UITF_API void stop() throw();

};

} // namespace uiTF

#endif //_P_
