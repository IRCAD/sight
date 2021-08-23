/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "SViewer.hpp"

#include <core/base.hpp>

#include <data/reflection/getObject.hpp>
#include <data/Series.hpp>
#include <data/String.hpp>

#include <service/extension/AppConfig.hpp>
#include <service/macros.hpp>

namespace sight::module::ui::qt::series
{

//------------------------------------------------------------------------------

SViewer::SViewer()
{
}

//------------------------------------------------------------------------------

SViewer::~SViewer() noexcept
{
}

//------------------------------------------------------------------------------

void SViewer::info(std::ostream& _sstream)
{
    // Update message
    _sstream << std::string("SViewer");
}

//------------------------------------------------------------------------------

void SViewer::starting()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SViewer::stopping()
{
    if(m_configTemplateManager)
    {
        m_configTemplateManager->stopAndDestroy();
        m_configTemplateManager.reset();
    }
}

//------------------------------------------------------------------------------

void SViewer::updating()
{
    const auto vector = m_series.lock();
    SIGHT_ASSERT("The input key '" << s_SERIES << "' is not defined.", vector);

    if(m_configTemplateManager)
    {
        m_configTemplateManager->stopAndDestroy();
        m_configTemplateManager.reset();
    }

    if(vector->size() == 1)
    {
        data::Object::sptr obj            = vector->front();
        std::string classname             = obj->getClassname();
        SeriesConfigMapType::iterator itr = m_seriesConfigs.find(classname);

        if(itr != m_seriesConfigs.end())
        {
            SeriesConfigInfo info = itr->second;
            std::string configId  = info.configId;

            std::map<std::string, std::string> replaceMap;
            // Generate generic UID
            std::string genericUidAdaptor = service::extension::AppConfig::getUniqueIdentifier(this->getID());
            replaceMap["GENERIC_UID"] = genericUidAdaptor;
            replaceMap["WID_PARENT"]  = m_parentView;
            replaceMap["objectID"]    = obj->getID();

            for(const ReplaceValuesMapType::value_type& elt : info.extractValues)
            {
                data::Object::sptr object = data::reflection::getObject(obj, elt.second);
                SIGHT_ASSERT("Object from name " << elt.second << " not found", object);
                replaceMap[elt.first] = object->getID();
            }

            for(const ReplaceValuesMapType::value_type& elt : info.parameters)
            {
                SIGHT_ASSERT(
                    "Value '" << elt.first << "' already used in extracted values.",
                    replaceMap.find(elt.first) == replaceMap.end()
                );
                replaceMap[elt.first] = elt.second;
            }

            // Init manager
            m_configTemplateManager = service::IAppConfigManager::New();
            m_configTemplateManager->setConfig(configId, replaceMap);

            // Launch config
            m_configTemplateManager->launch();
        }
    }
}

//------------------------------------------------------------------------------

void SViewer::configuring()
{
    std::vector<core::runtime::ConfigurationElement::sptr> viewCfg = m_configuration->find("parentView");
    SIGHT_ASSERT("Missing tag 'parentView'", viewCfg.size() == 1);

    m_parentView = viewCfg[0]->getAttributeValue("wid");
    SIGHT_ASSERT("'wid' attribute missing for tag 'parentView'.", !m_parentView.empty());

    std::vector<core::runtime::ConfigurationElement::sptr> configsCfg = m_configuration->find("configs");
    SIGHT_ASSERT("Missing tag 'configs'", configsCfg.size() == 1);

    std::vector<core::runtime::ConfigurationElement::sptr> config = configsCfg[0]->find("config");
    SIGHT_ASSERT("Missing tag 'config'", !config.empty());

    for(const core::runtime::ConfigurationElement::sptr& elt : config)
    {
        SeriesConfigInfo info;
        info.configId = elt->getAttributeValue("id");
        SIGHT_ASSERT("'id' attribute must not be empty", !info.configId.empty());
        std::string seriesType = elt->getAttributeValue("type");
        SIGHT_ASSERT("'type' attribute must not be empty", !seriesType.empty());
        SIGHT_ASSERT(
            "Type " << seriesType << " is already defined.",
            m_seriesConfigs.find(seriesType) == m_seriesConfigs.end()
        );

        for(const core::runtime::ConfigurationElement::sptr& extractElt : elt->find("extract"))
        {
            std::string path = extractElt->getAttributeValue("path");
            SIGHT_ASSERT("'path' attribute must not be empty", !path.empty());
            std::string pattern = extractElt->getAttributeValue("pattern");
            SIGHT_ASSERT("'pattern' attribute must not be empty", !pattern.empty());
            info.extractValues[pattern] = path;
        }

        for(const core::runtime::ConfigurationElement::sptr& param : elt->find("parameter"))
        {
            std::string replace = param->getAttributeValue("replace");
            SIGHT_ASSERT("'replace' attribute must not be empty", !replace.empty());
            std::string by = param->getAttributeValue("by");
            if(by.empty())
            {
                by = param->getAttributeValue("uid");
            }

            SIGHT_ASSERT("'by' attribute must not be empty", !by.empty());
            info.parameters[replace] = by;
        }

        m_seriesConfigs[seriesType] = info;
    }
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SViewer::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_SERIES, data::Vector::s_ADDED_OBJECTS_SIG, s_UPDATE_SLOT);
    connections.push(s_SERIES, data::Vector::s_REMOVED_OBJECTS_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::series
