/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWGUI_ILocationDialog_HPP_
#define _FWGUI_ILocationDialog_HPP_

#include <string>
#include <fwCore/base.hpp>

#include "fwGui/config.hpp"

#include <fwData/location/ILocation.hpp>

namespace fwGui
{

/**
 * @brief   Defines the generic file/folder dialog for IHM.
 * @class   LocationDialog.
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 *
 */
class FWGUI_CLASS_API ILocationDialog
{

public:

    typedef enum {
        READ,
        WRITE,
        FILE_MUST_EXIST
    } Options;

    typedef std::string FactoryRegistryKeyType;
    FWGUI_API static const FactoryRegistryKeyType factoryRegistryKey;

    FWGUI_API virtual void setTitle(const std::string &title) = 0;
    FWGUI_API virtual void setDefaultLocation( ::fwData::location::ILocation::csptr ) =0;
    FWGUI_API virtual ILocationDialog& setOption( Options option) =0;

    // exemple ( addFilter("images","*.png *.jpg");
    FWGUI_API virtual void addFilter(const std::string &filterName, const std::string &wildcardList )=0;

    FWGUI_API virtual ::fwData::location::ILocation::sptr show() = 0;

    FWGUI_API virtual ~ILocationDialog();
    FWGUI_API ILocationDialog();






};

} // namespace fwGui

#endif /*_FWGUI_LocationDialog_HPP_*/


