/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "dataReg/config.hpp"

#include <fwServices/IAppConfigManager.hpp>
#include <fwServices/IXMLParser.hpp>

#include <fwTools/Object.hpp>

namespace dataReg
{
namespace parser
{

/**
 * @brief Specific ::fwTools::Object parser. Does nothing as no IService method is specialized: IXMLParser base class
 *  methods are therefore considered.
 *
 * @note This implementation is only written to avoid the mix between the service type IXMLParser and the
 * ::fwTools::Object implementation (for homogeneity).
 *
 */
class DATAREG_CLASS_API Object : public ::fwServices::IXMLParser
{
public:
    fwCoreServiceMacro(Object, ::fwServices::IXMLParser);

    DATAREG_API Object( );
    DATAREG_API virtual ~Object();

    DATAREG_API void createConfig( ::fwTools::Object::sptr _obj ) override;
    DATAREG_API void startConfig() override;
    DATAREG_API void updateConfig() override;
    DATAREG_API void stopConfig() override;
    DATAREG_API void destroyConfig() override;

protected:

    bool refObjectValidator( ::fwRuntime::ConfigurationElement::csptr _cfgElement );
    void updating() override;

    std::vector< ::fwServices::IAppConfigManager::sptr > m_ctmContainer;
};

} //namespace parser
} //namespace dataReg
