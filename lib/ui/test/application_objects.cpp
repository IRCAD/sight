/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "detail/application_objects.hpp"

#include "detail/locator.hpp"

#include <service/registry.hpp>

#include <ui/__/service.hpp>
#include <ui/qt/container/widget.hpp>

#include <QWidget>

#include <algorithm>
#include <set>
#include <stdexcept>

namespace sight::ui::test::detail
{

//------------------------------------------------------------------------------

/// What comes before the uid in the id of an object or a service: the configuration it was created by.
static std::string id_prefix(const core::object& _object)
{
    const std::string id = _object.get_id();
    return id.substr(0, id.size() - _object.base_id().size());
}

//------------------------------------------------------------------------------

/// The configurations with a service whose widgets are inside the scope. Without a scope, every configuration.
static std::set<std::string> prefixes_in_scope(
    const sight::service::service_vector_t& _services,
    std::string_view _scope
)
{
    std::set<std::string> prefixes;
    if(_scope.empty())
    {
        for(const auto& running : _services)
        {
            prefixes.insert(id_prefix(*running));
        }

        return prefixes;
    }

    // By the widget of each service: the same uid is launched in several views, under the same widget names.
    const std::vector<QWidget*> roots = locator(_scope).resolve_widgets(true);
    for(const auto& running : _services)
    {
        auto* const with_widget = dynamic_cast<ui::service*>(running.get());
        if(with_widget == nullptr)
        {
            continue;
        }

        const auto container =
            std::dynamic_pointer_cast<ui::qt::container::widget>(with_widget->get_container());
        const QWidget* const widget = container == nullptr ? nullptr : container->get_qt_container();
        if(widget != nullptr
           && std::ranges::any_of(
               roots,
               [widget](const QWidget* _root){return _root == widget || _root->isAncestorOf(widget);}))
        {
            prefixes.insert(id_prefix(*running));
        }
    }

    return prefixes;
}

//------------------------------------------------------------------------------

scoped_uid parse_scoped_uid(std::string_view _expression)
{
    const std::size_t separator = _expression.rfind(">>");
    const locator checked(_expression);
    if(checked.segments().empty() || checked.segments().back().components.size() != 1
       || checked.segments().back().role_occurrence.has_value())
    {
        throw std::invalid_argument("'" + std::string(_expression) + "' does not end with the uid of the XML");
    }

    if(separator == std::string_view::npos)
    {
        return {.scope = {}, .uid = checked.segments().back().object_name};
    }

    // The locator has trimmed the name; the scope is checked again when it is resolved.
    const std::string_view scope = _expression.substr(0, _expression.find_last_not_of(' ', separator - 1) + 1);
    return {.scope = std::string(scope), .uid = checked.segments().back().object_name};
}

//------------------------------------------------------------------------------

std::vector<sight::service::base::sptr> find_services(const scoped_uid& _name)
{
    // One copy: workers of the application register and remove services meanwhile.
    const sight::service::service_vector_t services = sight::service::get_services();
    const std::set<std::string> prefixes            = prefixes_in_scope(services, _name.scope);

    std::vector<sight::service::base::sptr> found;
    for(const auto& running : services)
    {
        if(running->base_id() == _name.uid && prefixes.contains(id_prefix(*running)))
        {
            found.push_back(running);
        }
    }

    return found;
}

//------------------------------------------------------------------------------

object_lookup look_up_objects(const scoped_uid& _name)
{
    // One copy: workers of the application register and remove services meanwhile.
    object_lookup lookup {.ids = {}, .created = {}, .services = sight::service::get_services()};
    for(const std::string& prefix : prefixes_in_scope(lookup.services, _name.scope))
    {
        lookup.ids.push_back(prefix + _name.uid);

        // Created with the configuration, it has this id.
        auto created = std::dynamic_pointer_cast<const data::object>(core::id::get_object(lookup.ids.back()));
        if(created != nullptr)
        {
            lookup.created.push_back(std::move(created));
        }
    }

    return lookup;
}

//------------------------------------------------------------------------------

std::vector<data::object::csptr> deferred_objects(
    const sight::service::service_vector_t& _services,
    const std::vector<std::string>& _ids
)
{
    // Deferred, only the services waiting for it know it by its id.
    std::vector<data::object::csptr> found;
    for(const auto& running : _services)
    {
        for(const std::string& id : _ids)
        {
            if(auto object = running->deferred_object(id); object != nullptr)
            {
                found.push_back(std::move(object));
            }
        }
    }

    return found;
}

} // namespace sight::ui::test::detail
