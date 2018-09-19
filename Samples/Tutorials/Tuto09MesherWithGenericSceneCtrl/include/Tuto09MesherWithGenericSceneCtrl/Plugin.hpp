/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "Tuto09MesherWithGenericSceneCtrl/config.hpp"

#include <fwCom/SlotBase.hpp>

#include <fwRuntime/Plugin.hpp>

#include <fwServices/AppManager.hpp>

namespace Tuto09MesherWithGenericSceneCtrl
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class TUTO09MESHERWITHGENERICSCENECTRL_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /// Constructor.
    TUTO09MESHERWITHGENERICSCENECTRL_API Plugin() noexcept;

    /// Destructor. Do nothing.
    TUTO09MESHERWITHGENERICSCENECTRL_API ~Plugin() noexcept;

    /// Overrides start method. .
    TUTO09MESHERWITHGENERICSCENECTRL_API void start();

    /// Overrides stop method. Do nothing
    TUTO09MESHERWITHGENERICSCENECTRL_API void stop() noexcept;

    TUTO09MESHERWITHGENERICSCENECTRL_API void initialize();

    TUTO09MESHERWITHGENERICSCENECTRL_API void uninitialize() noexcept;

private:

    std::unique_ptr< ::fwServices::AppManager > m_appManager;

    ::fwCom::SlotBase::sptr m_slotRecSelected;
    ::fwCom::SlotBase::sptr m_slotEmptySelection;
};

} // namespace Tuto02DataServiceBasicCtrl
