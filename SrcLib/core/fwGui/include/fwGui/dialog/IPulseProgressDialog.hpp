/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWGUI_DIALOG_IPULSEPROGRESSDIALOG_HPP__
#define __FWGUI_DIALOG_IPULSEPROGRESSDIALOG_HPP__

#include "fwGui/config.hpp"
#include "fwGui/GuiBaseObject.hpp"

#include <fwData/location/ILocation.hpp>

#include <functional>
#include <string>

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic Progress dialog for IHM.
 * @todo    add methods for behavior like autoClose, flying window or in status bar
 */
class FWGUI_CLASS_API IPulseProgressDialog : public ::fwGui::GuiBaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IPulseProgressDialog)(::fwGui::GuiBaseObject) )

    typedef std::string FactoryRegistryKeyType;

    typedef std::function<void ()> Stuff;

    typedef unsigned long MilliSecond;

    /// this *unique* key should  be used *for all* factory for specific LocationDialog(qt,wx,...)
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    FWGUI_API virtual ~IPulseProgressDialog();
    FWGUI_API IPulseProgressDialog();

    ///set the title for the dialog
    FWGUI_API virtual void setTitle(const std::string& title) = 0;

    ///set the message for the dialog
    FWGUI_API virtual void setMessage(const std::string& msg) = 0;

    ///set the stuff
    FWGUI_API virtual void setStuff(Stuff stuff);

    ///set the frequence
    FWGUI_API virtual void setFrequence(MilliSecond frequence);

    ///show the pulse progress dialog
    FWGUI_API virtual void show() = 0;

protected:
    Stuff m_stuff;
    MilliSecond m_frequence;
};

} //namespace dialog
} // namespace fwGui

#endif /*__FWGUI_DIALOG_IPULSEPROGRESSDIALOG_HPP__*/

