/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_APPLICATION_HPP_
#define _FWGUI_APPLICATION_HPP_

#include <fwCore/base.hpp>
#include <fwTools/Factory.hpp>

#include "fwGui/IApplication.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{

/**
 * @brief   Gives access to the underlayed application part
 * @class   Application.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API Application : public ::fwGui::IApplication
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Application)(::fwGui::IApplication), (()), Application::factory);

    FWGUI_API static Application::sptr getDefault();


protected:


    /// Application factory, returning the registered intance of Application. Manage a Singleton.
    FWGUI_API static Application::sptr factory();

    FWGUI_API Application();
    FWGUI_API virtual ~Application();


};

} // namespace fwGui

#endif /*_FWGUI_APPLICATION_HPP_*/


