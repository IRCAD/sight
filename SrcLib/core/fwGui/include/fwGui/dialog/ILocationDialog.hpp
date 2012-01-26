/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_ILOCATIONDIALOG_HPP_
#define _FWGUI_ILOCATIONDIALOG_HPP_

#include <boost/filesystem.hpp>

#include <string>
#include <fwCore/base.hpp>

#include <fwData/location/ILocation.hpp>
#include <fwData/Composite.hpp>

#include "fwGui/config.hpp"

namespace fwGui
{
namespace dialog
{
/**
 * @brief   Defines the generic file/folder selector dialog for IHM.
 * @class   LocationDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API ILocationDialog : public ::fwCore::BaseObject
{

public:

    fwCoreNonInstanciableClassDefinitionsMacro( (ILocationDialog)(::fwCore::BaseObject) )

    typedef enum {
        NONE            = 0,
        READ            = 1 << 1,
        WRITE           = 1 << 2,
        FILE_MUST_EXIST = 1 << 3,
    } Options;

    typedef enum {
        SINGLE_FILE,
        FOLDER,
        MULTI_FILES
    } Types;

    typedef std::string FactoryRegistryKeyType;

    /// this *unique* key should  be used *for all* factory for specific LocationDialog(qt,wx,...)
    FWGUI_API static const FactoryRegistryKeyType REGISTRY_KEY;

    FWGUI_API static const std::string SOFTWARE_UI;
    FWGUI_API static const std::string DLG_DEFAULT_LOCATION;

    FWGUI_API virtual ~ILocationDialog();

    FWGUI_API ILocationDialog();

    /// set the title for the dialog
    FWGUI_API virtual void setTitle(const std::string& title);

    /// returns the title for the dialog
    FWGUI_API virtual const std::string& getTitle();

    /// set the initial location for the dialog
    FWGUI_API virtual void setDefaultLocation( ::fwData::location::ILocation::sptr loc);

    /// get the default location for the dialog (from preferences or specified by user)
    FWGUI_API virtual const ::boost::filesystem::path getDefaultLocation();

    /// save the specified default location for the dialog in preferences (if available)
    FWGUI_API virtual void saveDefaultLocation(::fwData::location::ILocation::sptr loc);

    /// set the type of location for the dialog (SINGLE_FILE, FORLDER, MULTI_FILES)
    FWGUI_API virtual void setType( Types type ) =0;

    /// allow to set option to the file dialog mode=READ/WRITE , check=FILE_MUST_EXIST
    FWGUI_API virtual ILocationDialog& setOption( Options option) =0;

    /**
     * @brief specify some filtering when browsing files:
     * @param[in] filterName a string that will be displayed as filter name
     * @param[in] wildcardList a string of extension (glob syntax) separated by spaces
     * example : addFilter("images","*.png *.jpg")
     */
    FWGUI_API virtual void addFilter(const std::string &filterName, const std::string &wildcardList )=0;

    /**
     * Display the dialog
     * @return the ILocation selected or null sptr if user cancel the operation
     */
    FWGUI_API virtual ::fwData::location::ILocation::sptr show() = 0;

protected:

    FWGUI_API void readConfig();

    FWGUI_API void writeConfig();

    FWGUI_API ::fwData::Composite::sptr getPreferenceUI();

private:

    std::string m_title;
    ::fwData::location::ILocation::sptr m_defaultLocaction;
};

} //namespace dialog
} // namespace fwGui

#endif /*_FWGUI_ILOCATIONDIALOG_HPP_*/


