/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUIQT_LOCATIONDIALOG_HPP_
#define _FWGUIQT_LOCATIONDIALOG_HPP_

#include <fwCore/base.hpp>

#include <fwGui/dialog/ILocationDialog.hpp>

#include "fwGuiQt/config.hpp"


class QString;


/**
 * @brief       The namespace fwGuiQt contains classes which provide the implementation of the Gui using Qt library.
 * @namespace   fwGuiQt
 * @author      IRCAD (Research and Development Team).
 * @date        2009-2010.
 *
 */
namespace fwGuiQt
{
namespace dialog
{
/**
 * @brief   Defines the generic file/folder dialog for IHM.
 * @class   LocationDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUIQT_CLASS_API LocationDialog : public ::fwGui::dialog::ILocationDialog
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (LocationDialog)(::fwGui::dialog::ILocationDialog), (()), new LocationDialog );

    LocationDialog();

    ::fwData::location::ILocation::sptr show();

    void setType( ::fwGui::dialog::ILocationDialog::Types type );

    ::fwGui::dialog::ILocationDialog& setOption( ::fwGui::dialog::ILocationDialog::Options option);

    // Example ( addFilter("images","*.png *.jpg");
    void addFilter(const std::string &filterName, const std::string &wildcardList );

protected:

    ::fwGui::dialog::ILocationDialog::Options m_style;
    ::fwGui::dialog::ILocationDialog::Types m_type;
    std::vector< std::pair< std::string, std::string > > m_filters;

    /// helper to transform m_filters into qt encoding ("BMP and GIF files (*.bmp *.gif);;PNG files (*.png)"
    QString fileFilters();

};
} // namespace dialog
} // namespace fwGuiQt

#endif /*_FWGUIQT_LOCATIONDIALOG_HPP_*/


