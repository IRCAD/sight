/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWWX_LOCATIONDIALOG_HPP_
#define _FWWX_LOCATIONDIALOG_HPP_

#include <map>
#include <fwCore/base.hpp>
#include <wx/filedlg.h>

#include <fwGui/ILocationDialog.hpp>

#include "fwWX/config.hpp"


namespace fwWX
{

/**
 * @brief   Defines the generic file/folder dialog for IHM.
 * @class   LocationDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWWX_CLASS_API LocationDialog : public ::fwGui::ILocationDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LocationDialog)(::fwGui::ILocationDialog), (()), new LocationDialog );

    LocationDialog();

    void setTitle(const std::string &title);

    ::fwData::location::ILocation::sptr show();

    // FIXME ok with Folder but not singleFile
    void setDefaultLocation( ::fwData::location::ILocation::csptr );

    ::fwGui::ILocationDialog& setOption( ::fwGui::ILocationDialog::Options option);

    // exemple ( addFilter("images","*.png *.jpg");
    void addFilter(const std::string &filterName, const std::string &wildcardList );


    //wxFileDialog m_fileDialog; // we can not change the style(OPEN/SAVE) of a wxFileDialog after its instanciation
                               // so we have to keep all information

protected:
    std::string m_title;
    ::boost::filesystem::path m_path;
    unsigned long m_style;
    std::multimap< std::string, std::string > m_filters;

    /// helper to transform m_filters into wx encoding ("BMP and GIF files (*.bmp;*.gif)|*.bmp;*.gif|PNG files (*.png)|*.png"
    wxString fileFilters();

};

} // namespace fwWX

#endif /*_FWWX_LOCATIONDIALOG_HPP_*/


