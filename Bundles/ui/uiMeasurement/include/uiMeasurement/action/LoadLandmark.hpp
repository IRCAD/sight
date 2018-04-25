/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiMeasurement/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <boost/filesystem/path.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief This action allows to load landmarks from a file
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::uiMeasurement::action::LoadLandmark" >
        <inout key="image" uid="..." />
    </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: Image containig the landmarks field.
 *
 * @deprecated This service is deprecated and will be removed, use SWriter to write the landmarks
 */
class UIMEASUREMENT_CLASS_API LoadLandmark : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (LoadLandmark)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API LoadLandmark() noexcept;

    UIMEASUREMENT_API virtual ~LoadLandmark() noexcept;

protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

    void info(std::ostream& _sstream ) override;

    void load(const ::boost::filesystem::path& path);
};

} // namespace action

} // namespace uiMeasurement
