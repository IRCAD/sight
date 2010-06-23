/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_LocationDialog_HPP_
#define _FWGUI_LocationDialog_HPP_

#include <fwCore/base.hpp>

#include "fwGui/config.hpp"
#include "fwGui/ILocationDialog.hpp"
namespace fwGui
{

/**
 * @brief   Defines the generic file/folder dialog for IHM.
 * Use the Delegate design pattern
 * @class   LocationDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API LocationDialog : public ILocationDialog
{

public:




    FWGUI_API LocationDialog();

    FWGUI_API void setTitle(const std::string &title);

    FWGUI_API ::fwData::location::ILocation::sptr show();

    FWGUI_API void setDefaultLocation( ::fwData::location::ILocation::csptr );
    FWGUI_API ILocationDialog& setOption( ILocationDialog::Options option);

    // exemple ( addFilter("images","*.png *.jpg");
    FWGUI_API void addFilter(const std::string &filterName, const std::string &wildcardList );

protected :

    ::boost::shared_ptr< ILocationDialog > m_implementation;





};

} // namespace fwGui

#endif /*_FWGUI_LocationDialog_HPP_*/


