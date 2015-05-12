/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_SADDLABELEDPOINT_HPP__
#define __UIMEASUREMENT_ACTION_SADDLABELEDPOINT_HPP__

#include <fwGui/IActionSrv.hpp>

#include "uiMeasurement/config.hpp"

namespace uiMeasurement
{
namespace action
{

/**
 * @brief   This action adds landmaks on image.
 * @class   SAddLabeledPoint
 *
 * @date    2010.
 */
class UIMEASUREMENT_CLASS_API SAddLabeledPoint : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SAddLabeledPoint)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API SAddLabeledPoint() throw();

    UIMEASUREMENT_API virtual ~SAddLabeledPoint() throw();

protected:


    UIMEASUREMENT_API void configuring() throw (::fwTools::Failed);

    UIMEASUREMENT_API void starting() throw (::fwTools::Failed);

    UIMEASUREMENT_API void updating() throw (::fwTools::Failed);

    UIMEASUREMENT_API void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed);

    UIMEASUREMENT_API void stopping() throw (::fwTools::Failed);

    UIMEASUREMENT_API void info(std::ostream &_sstream );

private:

    /// Helper to build point label
    bool defineLabel(std::string &name);

    int m_actionCheckId;

    unsigned int m_count;
};

} // namespace action
} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_SADDLABELEDPOINT_HPP__
