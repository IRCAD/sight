/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiMeasurement/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief This action adds distances on an image field.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiMeasurement::action::AddDistance">
       <inout key="image" uid="..." />
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image containing the distance field.
 */
class UIMEASUREMENT_CLASS_API AddDistance : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (AddDistance)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API AddDistance() noexcept;

    UIMEASUREMENT_API virtual ~AddDistance() noexcept;

protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

};

} // namespace action

} // namespace uiMeasurement
