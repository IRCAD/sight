/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "ctrlSelection/config.hpp"

#include <fwServices/IController.hpp>
#include <fwServices/IService.hpp>

namespace ctrlSelection
{

/**
 * @brief  This service bookmark its object with the name given in configuration.
 * @deprecated This class is no longer supported, it will be removed in sight 20.0
 */
class CTRLSELECTION_CLASS_API BookmarkSrv : public ::fwServices::IController
{

public:

    fwCoreServiceMacro(BookmarkSrv, ::fwServices::IController);

    CTRLSELECTION_API BookmarkSrv() noexcept;

    CTRLSELECTION_API virtual ~BookmarkSrv() noexcept;

protected:

    /// Adds the object in bookmark.
    CTRLSELECTION_API virtual void starting() override;

    /// Removes the object from bookmark
    CTRLSELECTION_API virtual void stopping() override;

    /// Do nothing
    CTRLSELECTION_API virtual void swapping() override;

    /**
     * @code{.xml}
       <service impl="::ctrlSelection::BookmarkSrv" type="::fwServices::IController" autoConnect="no">
           <bookmark fromString="..." name="..." />
       </service>
       @endcode
     * - \b fromString : prefix of the bookmark key
     * - \b name : name of the bookmark
     * The prefix and the name are concatenated as 'prefix_name'.
     */
    CTRLSELECTION_API virtual void configuring() override;

    /// Do nothing
    CTRLSELECTION_API virtual void reconfiguring() override;

    /// Do nothing
    CTRLSELECTION_API virtual void updating() override;

    CTRLSELECTION_API virtual void info( std::ostream& _sstream ) override;

private:
    std::string m_bookmarkName;
};

} // ctrlSelection
