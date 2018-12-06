/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWGUI_APPLICATION_HPP__
#define __FWGUI_APPLICATION_HPP__

#include <fwCore/base.hpp>

#include "fwGui/IApplication.hpp"
#include "fwGui/config.hpp"

namespace fwGui
{

/**
 * @brief   Gives access to the underlayed application part
 * @class   Application
 *
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API Application : public ::fwGui::IApplication
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (Application)(::fwGui::IApplication), (()), Application::factory);

    FWGUI_API static Application::sptr getDefault();

protected:

    /// Application factory, returning the registered instance of Application. Manage a Singleton.
    FWGUI_API static Application::sptr factory();

    Application()
    {
    }
    virtual ~Application()
    {
    }


};

} // namespace fwGui

#endif /*__FWGUI_APPLICATION_HPP__*/


