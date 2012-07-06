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

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic file/folder selector dialog for IHM.
 *
 * Use the Delegate design pattern. The specific implementation selection is ensured by ::fwClassFactoryRegistry.
 * The specific implementation are in fwWX and fwQT libraries.
 *
 * @class   LocationDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API LocationDialog : public ILocationDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LocationDialog)(::fwGui::dialog::ILocationDialog), (()), new LocationDialog );

    /// Will instantiate the concrete implementation
    FWGUI_API LocationDialog();

    FWGUI_API virtual ~LocationDialog();

    /**
     * @brief Display the dialog
     * @return the ILocation selected or null sptr if user cancel the operation
     */
    FWGUI_API ::fwData::location::ILocation::sptr show();

    /// allow to set option to the file dialog mode=READ/WRITE, check=FILE_MUST_EXIST
    FWGUI_API ::fwGui::dialog::ILocationDialog& setOption( ::fwGui::dialog::ILocationDialog::Options option);

    /// Set the type of location for the dialog (SINGLE_FILE, FORLDER, MULTI_FILES)
    FWGUI_API void setType( ::fwGui::dialog::ILocationDialog::Types type);

    /**
     * @brief specify some filtering when browsing files:
     * @param[in] filterName a string that will be displayed as filter name
     * @param[in] wildcardList a string of extension (glob syntax) separated by spaces
     * example : addFilter("images","*.png *.jpg")
     */
    FWGUI_API void addFilter(const std::string &filterName, const std::string &wildcardList );

    /// Set the title for the dialog
    FWGUI_API void setTitle(const std::string& title);

    /// Returns the title for the dialog
    FWGUI_API const std::string& getTitle();

    /// Set the initial location for the dialog
    FWGUI_API virtual void setDefaultLocation( ::fwData::location::ILocation::sptr loc);

    /// Gets the default location for the dialog (from preferences or specified by user)
    FWGUI_API const ::boost::filesystem::path getDefaultLocation();

    /// Save the specified default location for the dialog in preferences (if available)
    FWGUI_API void saveDefaultLocation(::fwData::location::ILocation::sptr loc);

protected :

    ::fwGui::dialog::ILocationDialog::sptr m_implementation;
};

} //namespace dialog
} // namespace fwGui

#endif /*_FWGUI_LOCATIONDIALOG_HPP_*/


