/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_ADDLANDMARK_HPP__
#define __UIMEASUREMENT_ACTION_ADDLANDMARK_HPP__

#include "uiMeasurement/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action adds landmaks on image.
 *
 * @section XML XML configuration
 * @code{.xml}
    <service uid="..." type="::uiMeasurement::action::AddLandmark" >
       <inout key="image" uid="..."/>
    </service>
   @endcode
 *
 *  * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to which the landmarks are added.
 *
 */
class UIMEASUREMENT_CLASS_API AddLandmark : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (AddLandmark)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API AddLandmark() noexcept;

    UIMEASUREMENT_API virtual ~AddLandmark() noexcept;

protected:

    void configuring() override;

    void starting() override;

    void updating() override;

    void stopping() override;

    UIMEASUREMENT_API void info(std::ostream& _sstream ) override;

private:
    int m_actionCheckId;
};

} // namespace action
} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_ADDLANDMARK_HPP__
