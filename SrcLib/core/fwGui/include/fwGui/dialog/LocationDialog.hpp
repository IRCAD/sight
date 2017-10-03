/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_DIALOG_LOCATIONDIALOG_HPP__
#define __FWGUI_DIALOG_LOCATIONDIALOG_HPP__

#include "fwGui/config.hpp"
#include "fwGui/dialog/ILocationDialog.hpp"

#include <fwCore/base.hpp>

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic file/folder selector dialog for IHM.
 *
 * Use the Delegate design pattern. The specific implementation selection is ensured by fwGuiRegisterMacro.
 * The specific implementation are in fwGuiWX and fwGuiQT libraries.
 */
class FWGUI_CLASS_API LocationDialog : public ILocationDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LocationDialog)(::fwGui::dialog::ILocationDialog), (()),
                                            new LocationDialog )

    /// Will instantiate the concrete implementation
    FWGUI_API LocationDialog();

    FWGUI_API virtual ~LocationDialog();

    /**
     * @brief Display the dialog
     * @return the ILocation selected or null sptr if user cancel the operation
     */
    FWGUI_API ::fwData::location::ILocation::sptr show() override;

    /// allow to set option to the file dialog mode=READ/WRITE, check=FILE_MUST_EXIST
    FWGUI_API ::fwGui::dialog::ILocationDialog& setOption( ::fwGui::dialog::ILocationDialog::Options option) override;

    /// Set the type of location for the dialog (SINGLE_FILE, FORLDER, MULTI_FILES)
    FWGUI_API void setType( ::fwGui::dialog::ILocationDialog::Types type) override;

    /**
     * @brief specify some filtering when browsing files:
     * @param[in] filterName a string that will be displayed as filter name
     * @param[in] wildcardList a string of extension (glob syntax) separated by spaces
     * example : addFilter("images","*.png *.jpg")
     */
    FWGUI_API void addFilter(const std::string& filterName, const std::string& wildcardList ) override;

    /// Set the title for the dialog
    FWGUI_API void setTitle(const std::string& title) override;

    /// Returns the title for the dialog
    FWGUI_API const std::string& getTitle() override;

    /// Set the initial location for the dialog
    FWGUI_API virtual void setDefaultLocation( ::fwData::location::ILocation::sptr loc) override;

    /// Gets the default location for the dialog (from preferences or specified by user)
    FWGUI_API const ::boost::filesystem::path getDefaultLocation() override;

    /// Save the specified default location for the dialog in preferences (if available)
    FWGUI_API void saveDefaultLocation(::fwData::location::ILocation::sptr loc) override;

    /// Gets the current extension file selection
    FWGUI_API std::string getCurrentSelection() const override;
protected:

    ::fwGui::dialog::ILocationDialog::sptr m_implementation;
};

} //namespace dialog
} // namespace fwGui

#endif /*__FWGUI_DIALOG_LOCATIONDIALOG_HPP__*/

