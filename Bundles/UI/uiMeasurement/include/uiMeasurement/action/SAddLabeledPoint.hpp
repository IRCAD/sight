/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_SADDLABELEDPOINT_HPP__
#define __UIMEASUREMENT_ACTION_SADDLABELEDPOINT_HPP__

#include "uiMeasurement/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action adds landmaks on image.
 */
class UIMEASUREMENT_CLASS_API SAddLabeledPoint : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro( (SAddLabeledPoint)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API SAddLabeledPoint() noexcept;

    UIMEASUREMENT_API virtual ~SAddLabeledPoint() noexcept;

protected:

    UIMEASUREMENT_API void configuring() override;

    UIMEASUREMENT_API void starting() override;

    UIMEASUREMENT_API void updating() override;

    UIMEASUREMENT_API void stopping() override;

    UIMEASUREMENT_API void info(std::ostream& _sstream ) override;

private:

    /// Helper to build point label
    bool defineLabel(std::string& name);

    int m_actionCheckId;

    unsigned int m_count;
};

} // namespace action
} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_SADDLABELEDPOINT_HPP__
