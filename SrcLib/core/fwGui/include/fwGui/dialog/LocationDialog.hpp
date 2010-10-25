/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_LOCATIONDIALOG_HPP_
#define _FWGUI_LOCATIONDIALOG_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"
#include "fwGui/dialog/ILocationDialog.hpp"

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
 * @brief   Defines the generic file/folder dialog for IHM.
 * Use the Delegate design pattern. The specific implementation selection is ensured by ::fwClassFactoryRegistry
 * The specific implementation are in fwWX and fwQT libraries
 * @class   LocationDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API LocationDialog : public ILocationDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LocationDialog)(::fwGui::dialog::ILocationDialog), (()), new LocationDialog );

    /// will instanciate the concrete implementation
    FWGUI_API LocationDialog();

    /// override
    FWGUI_API void setTitle(const std::string &title);

    /// override
    FWGUI_API void setDefaultLocation( ::fwData::location::ILocation::csptr );

    /// override
    FWGUI_API ::fwData::location::ILocation::sptr show();

    /// override
    FWGUI_API ::fwGui::dialog::ILocationDialog& setOption( ::fwGui::dialog::ILocationDialog::Options option);

    /// override
    FWGUI_API void setType( ::fwGui::dialog::ILocationDialog::Types type);


    /// override
    FWGUI_API void addFilter(const std::string &filterName, const std::string &wildcardList );

protected :

    ::fwGui::dialog::ILocationDialog::sptr m_implementation;

};

} //namespace dialog
} // namespace fwGui

#endif /*_FWGUI_LOCATIONDIALOG_HPP_*/


