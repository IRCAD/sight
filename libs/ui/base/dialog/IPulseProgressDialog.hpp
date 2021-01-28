/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/base/config.hpp"
#include "ui/base/GuiBaseObject.hpp"

#include <data/location/ILocation.hpp>

#include <functional>
#include <string>

namespace sight::ui::base
{
namespace dialog
{
/**
 * @brief   Defines the generic Progress dialog for IHM.
 * @todo    add methods for behavior like autoClose, flying window or in status bar
 */
class UI_BASE_CLASS_API IPulseProgressDialog : public ui::base::GuiBaseObject
{

public:

    fwCoreClassMacro(IPulseProgressDialog, ui::base::GuiBaseObject)

    typedef std::string FactoryRegistryKeyType;

    typedef std::function<void ()> Stuff;

    typedef unsigned long MilliSecond;

    /// this *unique* key should  be used *for all* factory for specific LocationDialog(qt,wx,...)
    UI_BASE_API static const FactoryRegistryKeyType REGISTRY_KEY;

    UI_BASE_API virtual ~IPulseProgressDialog();
    UI_BASE_API IPulseProgressDialog();

    ///set the title for the dialog
    UI_BASE_API virtual void setTitle(const std::string& title) = 0;

    ///set the message for the dialog
    UI_BASE_API virtual void setMessage(const std::string& msg) = 0;

    ///set the stuff
    UI_BASE_API virtual void setStuff(Stuff stuff);

    ///set the frequence
    UI_BASE_API virtual void setFrequence(MilliSecond frequence);

    ///show the pulse progress dialog
    UI_BASE_API virtual void show() = 0;

protected:
    Stuff m_stuff;
    MilliSecond m_frequence;
};

} //namespace dialog
} // namespace sight::ui::base
