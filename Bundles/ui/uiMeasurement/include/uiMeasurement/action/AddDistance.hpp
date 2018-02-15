/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_ADDDISTANCE_HPP__
#define __UIMEASUREMENT_ACTION_ADDDISTANCE_HPP__

#include "uiMeasurement/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action adds distance on image.
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

    UIMEASUREMENT_API void info(std::ostream& _sstream ) override;

private:
    int m_actionCheckId;
};

} // namespace action

} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_ADDDISTANCE_HPP__
