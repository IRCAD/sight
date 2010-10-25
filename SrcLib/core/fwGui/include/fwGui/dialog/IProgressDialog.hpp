/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_IPROGRESSDIALOG_HPP_
#define _FWGUI_IPROGRESSDIALOG_HPP_

#include <string>
#include <boost/signals/trackable.hpp>
#include <fwCore/base.hpp>
#include <fwData/location/ILocation.hpp>

#include "fwGui/config.hpp"

/**
 * @brief   The namespace fwGui contains the base interface for IHM services.
 * @namespace   fwGui
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic Progress dialog for IHM.
 * @todo    add methods for behavior like autoClose, flying window or in status bar
 * @class   LocationDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API IProgressDialog : public ::fwCore::BaseObject, public ::boost::signals::trackable // to autoDisconnect if handler is destroyed before the notifier
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (IProgressDialog)(::fwCore::BaseObject) )

    typedef std::string FactoryRegistryKeyType;

    /// this *unique* key should  be used *for all* factory for specific LocationDialog(qt,wx,...)
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    FWGUI_API virtual ~IProgressDialog();
    FWGUI_API IProgressDialog();

    ///set the title for the dialog
    FWGUI_API virtual void setTitle(const std::string &title) = 0;

    ///set the message for the dialog
    FWGUI_API virtual void setMessage(const std::string &msg) = 0;

    /// action called by ::fwTools::ProgressAdviser
    FWGUI_API virtual void operator()(float percent,std::string msg) = 0;

};

} //namespace dialog
} // namespace fwGui

#endif /*_FWGUI_LOCATIONDIALOG_HPP_*/


