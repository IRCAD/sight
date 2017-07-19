/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_ADDDISTANCE_HPP__
#define __UIMEASUREMENT_ACTION_ADDDISTANCE_HPP__

#include <fwGui/IActionSrv.hpp>

#include "uiMeasurement/config.hpp"

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action adds distance on image.
 * @class   AddDistance
 */
class UIMEASUREMENT_CLASS_API AddDistance : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (AddDistance)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API AddDistance() noexcept;

    UIMEASUREMENT_API virtual ~AddDistance() noexcept;

protected:

    void configuring();

    void starting();

    void updating();

    void stopping();

    UIMEASUREMENT_API void info(std::ostream &_sstream );

private:
    int m_actionCheckId;
};

} // namespace action

} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_ADDDISTANCE_HPP__
