/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

