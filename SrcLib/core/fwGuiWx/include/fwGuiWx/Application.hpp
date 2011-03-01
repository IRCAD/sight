/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIWX_APPLICATION_HPP_
#define _FWGUIWX_APPLICATION_HPP_

#include <fwCore/base.hpp>

#include <fwGui/Application.hpp>

#include "fwGuiWx/config.hpp"

namespace fwGuiWx
{

/**
 * @brief   Gives access to the wx application part
 * @class   Application.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIWX_CLASS_API Application : public ::fwGui::Application
{

public:

    //fwCoreClassDefinitionsWithFactoryMacro( (Application)(::fwGui::IApplication), (()), new Application );

    /// Tells the application to exit with a returncode
    FWGUIWX_API virtual void exit( int returncode = 0);

};

} // namespace fwGuiWx

#endif /*_FWGUIWX_APPLICATION_HPP_*/


