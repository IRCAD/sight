/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwServices/config.hpp"
#include <fwServices/IService.hpp>

#include <array>
#include <cstdint>

namespace fwServices
{

/**
 * @brief   This interface defines a basic slots to receive parameter values from signals.
 *
 * It is designed to be used in conjunction with ::gui::editor::SParameters, to help the user to receive
 * parameters. Instead of redefining all slots, thus you can simply inherit from this interface and override the virtual
 * function you need. Then connect the ::gui::editor::SParameters signal(s) with the slot(s) of this interface in the
 * configuration.

 * @section Slots Slots
 * - \b setBoolParameter(bool, std::string) : Receive a boolean parameter along with the identifier.
 * - \b setColorParameter(std::array<std::uint8_t, 4>, std::string) : Receive a color along with the identifier.
 * - \b setDoubleParameter(double, std::string) : Receive a real parameter along with the identifier.
 * - \b setDouble2Parameter(double, double, std::string) : Receive two real parameters along with the identifier.
 * - \b setDouble3Parameter(double, double, double, std::string) : Receive three real parameters along with the
 * identifier.
 * - \b setIntParameter(int, std::string) : Receive an integer parameter along with the identifier.
 * - \b setInt2Parameter(int, int, std::string) : Receive two integer parameters along with the identifier.
 * - \b setInt3Parameter(int, int, int, std::string) : Receive three integer parameters along with the identifier.
 * - \b setEnumParameter(std::string, std::string) : Receive an enum parameter along with the identifier.
 */
class FWSERVICES_CLASS_API IParametersService : public ::fwServices::IService
{

public:

    fwCoreServiceClassDefinitionsMacro( (IParametersService)(::fwServices::IService) );

    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_BOOL_PARAMETER_SLOT;
    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_COLOR_PARAMETER_SLOT;
    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE_PARAMETER_SLOT;
    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE2_PARAMETER_SLOT;
    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_DOUBLE3_PARAMETER_SLOT;
    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_INT_PARAMETER_SLOT;
    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_INT2_PARAMETER_SLOT;
    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_INT3_PARAMETER_SLOT;
    FWSERVICES_API static const ::fwCom::Slots::SlotKeyType s_SET_ENUM_PARAMETER_SLOT;

protected:

    /// Constructor. Create all slots.
    FWSERVICES_API IParametersService();

    /// Destructor. Do nothing.
    FWSERVICES_API virtual ~IParametersService();

    /// Slot: This method is used to set an boolean parameter.
    FWSERVICES_API virtual void setBoolParameter(bool val, std::string key);

    /// Slot: This method is used to set a color parameter.
    FWSERVICES_API virtual void setColorParameter(std::array<std::uint8_t, 4> color, std::string key);

    /// Slot: This method is used to set a double parameter.
    FWSERVICES_API virtual void setDoubleParameter(double val, std::string key);

    /// Slot: This method is used to set two double parameters.
    FWSERVICES_API virtual void setDouble2Parameter(double val0, double val1, std::string key);

    /// Slot: This method is used to set three double parameters.
    FWSERVICES_API virtual void setDouble3Parameter(double val0, double val1, double val2, std::string key);

    /// Slot: This method is used to set an integer parameter.
    FWSERVICES_API virtual void setIntParameter(int val, std::string key);

    /// Slot: This method is used to set two int parameters.
    FWSERVICES_API virtual void setInt2Parameter(int val0, int val1, std::string key);

    /// Slot: This method is used to set three int parameters.
    FWSERVICES_API virtual void setInt3Parameter(int val0, int val1, int val2, std::string key);

    /// Slot: This method is used to set an enum parameter.
    FWSERVICES_API virtual void setEnumParameter(std::string val, std::string key);
};

}
