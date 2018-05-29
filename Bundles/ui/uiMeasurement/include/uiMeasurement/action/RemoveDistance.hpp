/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiMeasurement/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{

namespace action
{

/**
 */

/**
 * @brief   This action removes distances from the image field.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiMeasurement::action::RemoveDistance">
       <inout key="image" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image containing the distance field.
 */
class UIMEASUREMENT_CLASS_API RemoveDistance : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (RemoveDistance)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API RemoveDistance() noexcept;

    UIMEASUREMENT_API virtual ~RemoveDistance() noexcept;

protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

private:
    void notifyNewDistance(const ::fwData::Image::csptr& image, const ::fwData::PointList::sptr& distance) const;

    void notifyDeleteDistance(const ::fwData::Image::csptr& image, const ::fwData::PointList::csptr& distance) const;
};

} // namespace action

} // namespace uiMeasurement
