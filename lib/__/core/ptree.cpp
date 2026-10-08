/************************************************************************
 *
 * Copyright (C) 2025-2026 IRCAD France
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

#include <core/ptree.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <map>

namespace sight::core::ptree
{

//------------------------------------------------------------------------------

std::string to_string(const boost::property_tree::ptree& _pt)
{
    std::stringstream ss;

    // We specify xml settings for pretty printing
    boost::property_tree::write_xml(
        ss,
        _pt,
        boost::property_tree::xml_parser::xml_writer_settings<std::string>(' ', 4)
    );

    auto str = ss.str();
    str.erase(0, str.find('\n') + 1); // Remove the <xml?> tag which adds unnecessary noise
    return str;
}

//------------------------------------------------------------------------------

static void flatten_rec(
    const boost::property_tree::ptree& _tree,
    const std::string& _prefix,
    const std::set<std::string, std::less<> >& _reserved,
    std::vector<flat_entry>& _out
)
{
    std::map<std::string, std::size_t, std::less<> > ranks;

    for(const auto& [name, child] : _tree)
    {
        if(name == "<xmlattr>" || (_prefix.empty() && _reserved.contains(name)))
        {
            continue;
        }

        const std::size_t index = ranks[name]++;
        const std::string key   = _prefix.empty() ? name : _prefix + "." + name;

        if(const auto& attributes = child.get_child_optional("<xmlattr>"); attributes.has_value())
        {
            for(const auto& [attribute, value] : *attributes)
            {
                _out.emplace_back(key + "." + attribute, value.get_value<std::string>(), index);
            }
        }

        flatten_rec(child, key, _reserved, _out);
    }
}

//------------------------------------------------------------------------------

std::vector<flat_entry> flatten(
    const boost::property_tree::ptree& _tree,
    const std::set<std::string, std::less<> >& _reserved
)
{
    std::vector<flat_entry> entries;
    flatten_rec(_tree, "", _reserved, entries);
    return entries;
}

//------------------------------------------------------------------------------

void merge(boost::property_tree::ptree& _destination, const boost::property_tree::ptree& _source)
{
    for(const auto& source_child : _source)
    {
        auto destination_child = _destination.end();
        for(auto it = _destination.begin() ; it != _destination.end() ; ++it)
        {
            if(it->first == source_child.first)
            {
                destination_child = it;
                break;
            }
        }

        if(destination_child == _destination.end())
        {
            _destination.push_back(source_child);
        }
        else
        {
            merge(destination_child->second, source_child.second);
        }
    }
}

} // namespace sight::core::ptree
