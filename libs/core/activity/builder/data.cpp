/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include <activity/builder/data.hpp>

#include <data/factory/new.hpp>

#include <service/extension/Factory.hpp>
#include <service/IXMLParser.hpp>

namespace sight::activity::detail::data
{

//------------------------------------------------------------------------------

sight::data::Object::sptr create(const std::string& _type, const activity::extension::ConfigType& _config)
{
    auto object = sight::data::factory::New(_type);

    // Get the object parser associated with the object type
    const auto srvFactory = service::extension::Factory::getDefault();

    const std::string srvImpl = srvFactory->getDefaultImplementationIdFromObjectAndType(
        object->getClassname(),
        "sight::service::IXMLParser"
    );

    service::IService::sptr srv = srvFactory->create(srvImpl);
    auto objParser              = service::IXMLParser::dynamicCast(srv);

    objParser->setObjectConfig(_config);
    objParser->createConfig(object);
    objParser->startConfig();
    objParser->updateConfig();
    objParser->stopConfig();
    objParser->destroyConfig();

    return object;
}

} // namespace sight::activity::detail::data
