/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "viewer.hpp"

#include <data/series.hpp>
#include <data/string.hpp>

#include <service/extension/config.hpp>

#include <boost/range/iterator_range_core.hpp>

namespace sight::module::ui::qt::series
{

//------------------------------------------------------------------------------

void viewer::info(std::ostream& _sstream)
{
    // Update message
    _sstream << std::string("viewer");
}

//------------------------------------------------------------------------------

void viewer::starting()
{
    this->updating();
}

//------------------------------------------------------------------------------

void viewer::stopping()
{
    if(m_configTemplateManager)
    {
        m_configTemplateManager->stopAndDestroy();
        m_configTemplateManager.reset();
    }
}

//------------------------------------------------------------------------------

void viewer::updating()
{
    if(m_configTemplateManager)
    {
        m_configTemplateManager->stopAndDestroy();
        m_configTemplateManager.reset();
    }

    const auto vector = m_series.lock();
    SIGHT_ASSERT("The input key '" << s_SERIES << "' is not defined.", vector);
    if(vector->size() == 1)
    {
        data::object::sptr obj = vector->front();
        std::string classname  = obj->get_classname();
        auto itr               = m_seriesConfigs.find(classname);

        if(itr != m_seriesConfigs.end())
        {
            SeriesConfigInfo info = itr->second;
            std::string config_id = info.configId;

            std::map<std::string, std::string> replace_map;
            // Generate generic UID
            std::string generic_uid_adaptor = app::extension::config::getUniqueIdentifier(this->get_id());
            replace_map["GENERIC_UID"] = generic_uid_adaptor;
            replace_map["WID_PARENT"]  = m_parentView;
            replace_map["objectID"]    = obj->get_id();

            for(const replace_values_map_t::value_type& elt : info.parameters)
            {
                SIGHT_ASSERT(
                    "Value '" << elt.first << "' already used in extracted values.",
                    replace_map.find(elt.first) == replace_map.end()
                );
                replace_map[elt.first] = elt.second;
            }

            // Init manager
            m_configTemplateManager = app::config_manager::make();
            m_configTemplateManager->setConfig(config_id, replace_map);

            // Launch config
            m_configTemplateManager->launch();
        }
    }
}

//------------------------------------------------------------------------------

void viewer::configuring()
{
    const auto& config = this->get_config();
    m_parentView = config.get<std::string>("parentView.<xmlattr>.wid");

    const auto& configs = config.get_child("configs");
    for(const auto& elt : boost::make_iterator_range(configs.equal_range("config")))
    {
        SeriesConfigInfo info;
        info.configId = elt.second.get<std::string>("<xmlattr>.id", "");
        SIGHT_ASSERT("'id' attribute must not be empty", !info.configId.empty());

        const std::string series_type = elt.second.get<std::string>("<xmlattr>.type", "");
        SIGHT_ASSERT("'type' attribute must not be empty", !series_type.empty());
        SIGHT_ASSERT(
            "Type " << series_type << " is already defined.",
            m_seriesConfigs.find(series_type) == m_seriesConfigs.end()
        );

        for(const auto& param : boost::make_iterator_range(elt.second.equal_range("parameter")))
        {
            const std::string replace = param.second.get<std::string>("<xmlattr>.replace", "");
            SIGHT_ASSERT("'replace' attribute must not be empty", !replace.empty());
            std::string by = param.second.get<std::string>("<xmlattr>.by", "");
            if(by.empty())
            {
                by = param.second.get<std::string>("<xmlattr>.uid", "");
            }

            SIGHT_ASSERT("'by' attribute must not be empty", !by.empty());
            info.parameters[replace] = by;
        }

        m_seriesConfigs[series_type] = info;
    }
}

//------------------------------------------------------------------------------

service::connections_t viewer::auto_connections() const
{
    connections_t connections;

    connections.push(s_SERIES, data::vector::ADDED_OBJECTS_SIG, service::slots::UPDATE);
    connections.push(s_SERIES, data::vector::REMOVED_OBJECTS_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::series
