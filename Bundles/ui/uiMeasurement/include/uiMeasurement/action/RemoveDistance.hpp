/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_REMOVEDISTANCE_HPP__
#define __UIMEASUREMENT_ACTION_REMOVEDISTANCE_HPP__

#include "uiMeasurement/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/PointList.hpp>

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{

namespace action
{

/**
 * @brief   This action removes distances.
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

    UIMEASUREMENT_API void info(std::ostream& _sstream ) override;

private:
    void notifyNewDistance(const ::fwData::Image::csptr& image, const ::fwData::PointList::sptr& distance) const;

    void notifyDeleteDistance(const ::fwData::Image::csptr& image, const ::fwData::PointList::csptr& distance) const;
};

} // namespace action

} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_REMOVEDISTANCE_HPP__
