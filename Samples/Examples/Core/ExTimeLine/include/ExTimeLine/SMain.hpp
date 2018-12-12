/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "ExTimeLine/config.hpp"

#include <fwServices/IService.hpp>

namespace ExTimeLine
{

/**
 * @brief   This service is just a hack to block the main thread. Services on the other threads can run.
 *          Without that, the application exits immediately since we have no GUI.
 */
class EXTIMELINE_CLASS_API SMain : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro( (SMain)(::fwServices::IService) );

    EXTIMELINE_API SMain() noexcept;
    EXTIMELINE_API virtual ~SMain() noexcept;

protected:

    /// Does nothing.
    virtual void starting() override;

    /// Does nothing.
    virtual void stopping() override;

    /// Does nothing.
    virtual void swapping() override;

    /// Contains one input instruction to block the main thread.
    virtual void updating() override;

    /// Does nothing.
    virtual void configuring() override;

private:

};

}  // namespace ExTimeLine
