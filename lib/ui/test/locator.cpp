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

#include "locator.hpp"

#include "detail/locator.hpp"
#include "detail/widget_text.hpp"
#include "detail/widget_visibility.hpp"

#include <core/runtime/runtime.hpp>

#include <QAbstractItemView>
#include <QAction>
#include <QApplication>
#include <QThread>
#include <QWidget>

#include <algorithm>
#include <charconv>
#include <limits>
#include <optional>
#include <span>
#include <stdexcept>
#include <system_error>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace sight::ui::test
{

using detail::locator;
using detail::locator_catalog;
using detail::locator_catalog_entry;
using detail::locator_catalog_name_kind;
using detail::locator_segment;

// Services whose widgets are named after the XML keys of their items.
constexpr std::string_view SETTINGS_SERVICE_TYPE    = "sight::module::ui::qt::settings";
constexpr std::string_view PREFERENCES_SERVICE_TYPE = "sight::module::ui::qt::preferences_configuration";
constexpr std::string_view TOOLBOX_LAYOUT_TYPE      = "sight::ui::layout::toolbox";

//------------------------------------------------------------------------------

/// Appends a part of a name, with the characters a locator gives a meaning to escaped.
static void append_escaped(std::string& _name, std::string_view _part)
{
    for(const char character : _part)
    {
        if(character == '/' || character == '#' || character == '\\')
        {
            _name += '\\';
        }

        _name += character;
    }
}

//------------------------------------------------------------------------------

/// The widget and all its descendants, depth first.
static void collect_widget_tree(QWidget* _widget, std::vector<QWidget*>& _widgets)
{
    _widgets.push_back(_widget);
    for(QObject* child : _widget->children())
    {
        if(auto* const child_widget = qobject_cast<QWidget*>(child); child_widget != nullptr)
        {
            collect_widget_tree(child_widget, _widgets);
        }
    }
}

//------------------------------------------------------------------------------

/// Whether a widget lies strictly inside one of the scopes.
static bool is_in_scope(QWidget& _widget, const std::vector<QWidget*>& _scopes)
{
    for(QWidget* parent = _widget.parentWidget() ; parent != nullptr ; parent = parent->parentWidget())
    {
        if(std::ranges::find(_scopes, parent) != _scopes.end())
        {
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------

/// Whether a widget is one of the scopes, or lies inside one.
static bool is_in_or_in_scope(QWidget& _widget, const std::vector<QWidget*>& _scopes)
{
    return std::ranges::find(_scopes, &_widget) != _scopes.end() || is_in_scope(_widget, _scopes);
}

//------------------------------------------------------------------------------

/// The application, checking that the caller runs on its thread: widgets are only safe to read there.
static QApplication& gui_application()
{
    auto* const application = qobject_cast<QApplication*>(QCoreApplication::instance());
    if(application == nullptr)
    {
        throw std::logic_error("GUI locator resolution requires a QApplication");
    }

    if(QThread::currentThread() != application->thread())
    {
        throw std::logic_error("GUI locators must be resolved on the application thread");
    }

    return *application;
}

//------------------------------------------------------------------------------

/// The object picked by a role ordinal, once every other condition is applied; every match without one.
template<typename Object>
static std::vector<Object*> pick_occurrence(
    std::vector<Object*> _matches,
    const std::optional<std::size_t>& _occurrence
)
{
    if(!_occurrence.has_value())
    {
        return _matches;
    }

    return *_occurrence > _matches.size() ? std::vector<Object*> {} : std::vector<Object*> {_matches[*_occurrence - 1]};
}

//------------------------------------------------------------------------------

/// The widgets of every window, popups and dialogs included.
static std::vector<QWidget*> widgets_of_all_windows()
{
    std::vector<QWidget*> widgets;
    for(QWidget* top_level : QApplication::topLevelWidgets())
    {
        if(top_level->parentWidget() == nullptr)
        {
            collect_widget_tree(top_level, widgets);
        }
    }

    return widgets;
}

//------------------------------------------------------------------------------

/// The widgets named by the first segments, inside the given scopes when there are some: an empty scope list means no
/// scope. Ordinals are not applied.
static std::vector<QWidget*> resolve_named_widgets(
    const std::vector<locator_segment>& _segments,
    std::size_t _count,
    bool _include_hidden,
    std::vector<QWidget*> _scopes
)
{
    static_cast<void>(gui_application());

    const std::vector<QWidget*> widgets = widgets_of_all_windows();
    std::vector<QWidget*> matches       = std::move(_scopes);
    for(std::size_t index = 0 ; index < _count ; ++index)
    {
        // Each segment is looked for inside the matches of the previous one.
        std::vector<QWidget*> scoped_matches;
        const QString object_name = QString::fromStdString(_segments[index].object_name);
        for(QWidget* widget : widgets)
        {
            if(widget->objectName() == object_name && (_include_hidden || detail::is_visible(*widget))
               && ((index == 0 && matches.empty()) || is_in_scope(*widget, matches)))
            {
                scoped_matches.push_back(widget);
            }
        }

        matches = std::move(scoped_matches);
    }

    return matches;
}

//------------------------------------------------------------------------------

/// The whitespace of the C locale, as std::isspace without its undefined behaviour on negative characters.
static bool is_space(char _character)
{
    return _character == ' ' || _character == '\t' || _character == '\n' || _character == '\r' || _character == '\f'
           || _character == '\v';
}

//------------------------------------------------------------------------------

/// Rejects an XML name that no locator could write: empty, padded, with control characters or a scope separator.
static void validate_catalog_component(std::string_view _component, std::string_view _configuration_id)
{
    const bool has_control_character = std::ranges::any_of(
        _component,
        [](char _character)
        {
            const auto code = static_cast<unsigned char>(_character);
            return code < 0x20 || code == 0x7F;
        });
    if(_component.empty() || is_space(_component.front()) || is_space(_component.back()) || has_control_character
       || _component.find(">>") != std::string_view::npos)
    {
        throw std::invalid_argument(
                  "invalid GUI name '" + std::string(_component) + "' in configuration '"
                  + std::string(_configuration_id) + "'"
        );
    }
}

//------------------------------------------------------------------------------

/// Adds one XML name, after checking that a locator can write it.
static void add_catalog_entry(
    std::vector<locator_catalog_entry>& _entries,
    std::string_view _configuration_id,
    std::vector<std::string> _components,
    locator_catalog_name_kind _kind,
    std::string _service_type = {})
{
    for(const std::string& component : _components)
    {
        validate_catalog_component(component, _configuration_id);
    }

    _entries.push_back(
        {
            .configuration_id = std::string(_configuration_id),
            .components       = std::move(_components),
            .kind             = _kind,
            .service_type     = std::move(_service_type),
        });
}

//------------------------------------------------------------------------------

/// The key that names the widget of a settings item: its own key, or the last part of the data it edits.
static std::optional<std::string> get_settings_key(
    const core::runtime::config_t& _item,
    const std::unordered_map<std::string, std::string>& _service_bindings
)
{
    if(const auto key = _item.get_optional<std::string>("<xmlattr>.key"); key.has_value())
    {
        return *key;
    }

    const auto data = _item.get_optional<std::string>("<xmlattr>.data");
    if(!data.has_value())
    {
        return std::nullopt;
    }

    // `${uid}` names an object passed to the service, `${uid}.key` an item of it.
    const std::string_view reference(*data);
    const std::size_t variable_start = reference.find("${");
    if(variable_start != std::string_view::npos)
    {
        const std::size_t variable_end = reference.find('}', variable_start + 2);
        if(variable_end == std::string_view::npos)
        {
            return std::nullopt;
        }

        const std::string_view variable = reference.substr(variable_start + 2, variable_end - variable_start - 2);
        const std::string_view suffix   = reference.substr(variable_end + 1);
        if(suffix.empty())
        {
            // The variable may be the key of an input of the service, which then names the object.
            if(const auto binding = _service_bindings.find(std::string(variable));
               binding != _service_bindings.end())
            {
                return binding->second;
            }

            return std::string(variable);
        }

        if(suffix.front() == '.')
        {
            return std::string(suffix.substr(suffix.find_last_of('.') + 1));
        }

        return std::nullopt;
    }

    // A plain reference: its last part.
    const std::size_t separator = reference.find_last_of('.');
    return std::string(separator == std::string_view::npos ? reference : reference.substr(separator + 1));
}

//------------------------------------------------------------------------------

/// Adds the names a configuration tree declares, walking it with the service each node belongs to.
static void collect_catalog_entries(
    const core::runtime::config_t& _node,
    std::string_view _configuration_id,
    std::string_view _service_uid,
    std::string_view _service_type,
    const std::unordered_map<std::string, std::string>& _service_bindings,
    std::vector<locator_catalog_entry>& _entries
)
{
    // One dialog per preferences service, however many preferences it has.
    bool preference_dialog_added = false;
    for(const auto& [tag, child] : _node)
    {
        std::string service_uid(_service_uid);
        std::string service_type(_service_type);
        std::unordered_map<std::string, std::string> service_bindings_storage;
        const std::unordered_map<std::string, std::string>* service_bindings = &_service_bindings;
        if(tag == "service")
        {
            // A service names its widget, and its children inherit it with its data bindings.
            if(const auto uid = child.get_optional<std::string>("<xmlattr>.uid"); uid.has_value())
            {
                service_uid  = *uid;
                service_type = child.get<std::string>("<xmlattr>.type", "");
                add_catalog_entry(
                    _entries,
                    _configuration_id,
                    {service_uid},
                    locator_catalog_name_kind::service,
                    service_type
                );
            }

            for(const auto& [binding_tag, binding] : child)
            {
                if(binding_tag == "in" || binding_tag == "inout" || binding_tag == "out")
                {
                    const auto key = binding.get_optional<std::string>("<xmlattr>.key");
                    const auto uid = binding.get_optional<std::string>("<xmlattr>.uid");
                    if(key.has_value() && uid.has_value())
                    {
                        service_bindings_storage.insert_or_assign(*key, *uid);
                    }
                }
            }

            service_bindings = &service_bindings_storage;

            // The header of each section of a toolbox is named after the view of that section.
            const auto registry = child.get_child_optional("registry");
            if(child.get<std::string>("gui.layout.<xmlattr>.type", "") == TOOLBOX_LAYOUT_TYPE && registry.has_value())
            {
                for(const auto& [view_tag, view] : *registry)
                {
                    for(const std::string_view attribute : {"sid", "wid"})
                    {
                        if(const auto name = view.get_optional<std::string>("<xmlattr>." + std::string(attribute));
                           view_tag == "view" && name.has_value())
                        {
                            add_catalog_entry(
                                _entries,
                                _configuration_id,
                                {*name, "toggle"},
                                locator_catalog_name_kind::view
                            );
                        }
                    }
                }
            }
        }

        // A view of a layout is named after the service or the window id it registers.
        if(tag == "view" || tag == "editor" || tag == "slideView")
        {
            for(const std::string_view attribute : {"sid", "wid"})
            {
                if(const auto name = child.get_optional<std::string>("<xmlattr>." + std::string(attribute));
                   name.has_value())
                {
                    add_catalog_entry(
                        _entries,
                        _configuration_id,
                        {*name},
                        locator_catalog_name_kind::view,
                        {
                        });
                }
            }
        }
        else if(tag == "menu_item" || tag == "menuItem")
        {
            if(const auto name = child.get_optional<std::string>("<xmlattr>.name");
               !service_uid.empty() && name.has_value())
            {
                add_catalog_entry(
                    _entries,
                    _configuration_id,
                    {service_uid, *name},
                    locator_catalog_name_kind::menu_item,
                    {
                    });
            }
        }
        else if(tag == "item" && service_type == SETTINGS_SERVICE_TYPE && !service_uid.empty()
                && child.get_optional<std::string>("<xmlattr>.data").has_value())
        {
            const auto key = get_settings_key(child, *service_bindings);
            if(!key.has_value())
            {
                throw std::invalid_argument(
                          "settings item in service '" + service_uid + "' has no XML key in configuration '"
                          + std::string(_configuration_id) + "'"
                );
            }

            add_catalog_entry(
                _entries,
                _configuration_id,
                {service_uid, *key},
                locator_catalog_name_kind::parameter_key,
                {
                });
        }
        // A preference lives in the dialog of its service, with the buttons of that dialog.
        else if(tag == "preference" && service_type == PREFERENCES_SERVICE_TYPE && !service_uid.empty())
        {
            if(!preference_dialog_added)
            {
                add_catalog_entry(
                    _entries,
                    _configuration_id,
                    {service_uid, "dialog"},
                    locator_catalog_name_kind::dialog,
                    {
                    });
                for(const std::string_view role : {"ok", "cancel", "apply"})
                {
                    add_catalog_entry(
                        _entries,
                        _configuration_id,
                        {service_uid, "dialog", std::string(role)},
                        locator_catalog_name_kind::dialog_role,
                        {
                        });
                }

                preference_dialog_added = true;
            }

            const auto key = child.get_optional<std::string>("key");
            if(!key.has_value())
            {
                throw std::invalid_argument(
                          "preference in service '" + service_uid + "' has no key in configuration '"
                          + std::string(_configuration_id) + "'"
                );
            }

            add_catalog_entry(
                _entries,
                _configuration_id,
                {service_uid, "dialog", *key},
                locator_catalog_name_kind::parameter_key,
                {
                });
        }

        // Deeper nodes belong to the service found here, or to the one above.
        collect_catalog_entries(
            child,
            _configuration_id,
            service_uid,
            service_type,
            *service_bindings,
            _entries
        );
    }
}

//------------------------------------------------------------------------------

/// Objects are not widgets: they stay apart from the names a locator accepts.
static void collect_object_uids(const core::runtime::config_t& _node, std::set<std::string, std::less<> >& _objects)
{
    for(const auto& [tag, child] : _node)
    {
        // An object declared here, or passed under a parameter name.
        if(tag == "object" || tag == "param")
        {
            const auto uid = child.get_optional<std::string>(tag == "object" ? "<xmlattr>.uid" : "<xmlattr>.name");
            if(uid.has_value() && !uid->starts_with("${"))
            {
                _objects.insert(*uid);
            }
        }

        collect_object_uids(child, _objects);
    }
}

//------------------------------------------------------------------------------

bool detail::locator_catalog::knows_object(std::string_view _uid) const
{
    return m_objects.contains(_uid);
}

//------------------------------------------------------------------------------

void detail::locator_catalog::add_configuration(
    std::string_view _configuration_id,
    const core::runtime::config_t& _configuration
)
{
    add_configurations({{std::string(_configuration_id), _configuration}});
}

//------------------------------------------------------------------------------

void detail::locator_catalog::add_configurations(
    const std::vector<std::pair<std::string, core::runtime::config_t> >& _configurations
)
{
    // A configuration may be launched several times, in several views: its names are the same in each.
    std::unordered_set<std::string> configuration_ids;
    const std::unordered_map<std::string, std::string> no_service_bindings;
    for(const auto& [configuration_id, configuration] : _configurations)
    {
        if(!configuration_ids.insert(configuration_id).second)
        {
            throw std::runtime_error("duplicate application configuration id '" + configuration_id + "'");
        }

        collect_object_uids(configuration, m_objects);
        collect_catalog_entries(configuration, configuration_id, {}, {}, no_service_bindings, m_entries);
    }
}

//------------------------------------------------------------------------------

locator_catalog detail::locator_catalog::from_loaded_modules()
{
    constexpr std::string_view config_extension_point = "sight::app::extension::config";
    std::vector<std::pair<std::string, core::runtime::config_t> > configurations;
    for(const auto& extension : core::runtime::get_all_extensions_for_point(std::string(config_extension_point)))
    {
        const core::runtime::config_t& extension_config = extension->get_config();
        const auto configuration                        = extension_config.get_child_optional("config");
        if(!configuration.has_value())
        {
            throw std::runtime_error(
                      "application configuration extension '" + extension->identifier() + "' has no <config> node"
            );
        }

        const std::string configuration_id = extension_config.get<std::string>("id", extension->identifier());
        configurations.emplace_back(configuration_id, *configuration);
    }

    locator_catalog catalog;
    catalog.add_configurations(configurations);
    return catalog;
}

//------------------------------------------------------------------------------

const std::vector<locator_catalog_entry>& detail::locator_catalog::entries() const noexcept
{
    return m_entries;
}

//------------------------------------------------------------------------------

bool detail::locator_catalog::contains(const locator_segment& _segment) const
{
    return std::ranges::any_of(
        m_entries,
        [&_segment](const locator_catalog_entry& _entry)
        {
            return _entry.components == _segment.components;
        });
}

//------------------------------------------------------------------------------

bool detail::locator_catalog::accepts(const locator_segment& _segment) const
{
    // A name of the XML, or a part a service adds below its own name at runtime.
    if(contains(_segment))
    {
        return true;
    }

    if(_segment.components.size() < 2)
    {
        return false;
    }

    const std::string& root_name = _segment.components.front();
    const auto service           = std::ranges::find_if(
        m_entries,
        [&root_name](const locator_catalog_entry& _entry)
        {
            return _entry.kind == locator_catalog_name_kind::service && _entry.components.size() == 1
                   && _entry.components.front() == root_name;
        });
    if(service == m_entries.end())
    {
        return false;
    }

    // A menu names all its items in the XML: a part of it would be a typo.
    const bool is_menu_owner = service->service_type.find("toolbar") != std::string::npos
                               || service->service_type.find("menu") != std::string::npos;
    if(is_menu_owner)
    {
        return false;
    }

    // A settings part is below a known key, as in `settings/key/slider`.
    if(service->service_type == SETTINGS_SERVICE_TYPE)
    {
        return std::ranges::any_of(
            m_entries,
            [&_segment, &root_name](const locator_catalog_entry& _entry)
            {
                return _entry.kind == locator_catalog_name_kind::parameter_key && _entry.components.size() >= 2
                       && _entry.components[0] == root_name && _entry.components[1] == _segment.components[1];
            });
    }

    // A preferences part is below a known key of its dialog, as in `preferences/dialog/key/line_edit`.
    if(service->service_type == PREFERENCES_SERVICE_TYPE)
    {
        if(_segment.components.size() < 3 || _segment.components[1] != "dialog")
        {
            return false;
        }

        return std::ranges::any_of(
            m_entries,
            [&_segment, &root_name](const locator_catalog_entry& _entry)
            {
                return _entry.kind == locator_catalog_name_kind::parameter_key && _entry.components.size() >= 3
                       && _entry.components[0] == root_name && _entry.components[1] == "dialog"
                       && _entry.components[2] == _segment.components[2];
            });
    }

    // Any other service names its parts in its code: `service/button`.
    return true;
}

//------------------------------------------------------------------------------

/// Whether the components begin with the prefix.
static bool starts_with(std::span<const std::string> _components, std::span<const std::string> _prefix)
{
    return _prefix.size() <= _components.size() && std::ranges::equal(_prefix, _components.first(_prefix.size()));
}

//------------------------------------------------------------------------------

/// A catalog name as a locator writes it.
static std::string format_catalog_name(const std::vector<std::string>& _components)
{
    std::string name;
    for(const std::string& component : _components)
    {
        if(!name.empty())
        {
            name += '/';
        }

        append_escaped(name, component);
    }

    return name;
}

//------------------------------------------------------------------------------

std::vector<std::string> detail::locator_catalog::candidates(const locator_segment& _segment) const
{
    constexpr std::size_t most    = 8;
    const std::size_t parent_size = _segment.components.empty() ? 0 : _segment.components.size() - 1;
    const auto parent             = std::span(_segment.components).first(parent_size);

    // The names below the same parent first, any name when none shares it.
    std::vector<std::string> names;
    for(const bool same_parent_only : {true, false})
    {
        for(const locator_catalog_entry& entry : m_entries)
        {
            const bool same_parent = entry.components.size() > parent_size && starts_with(entry.components, parent);
            if(same_parent_only && !same_parent && parent_size != 0)
            {
                continue;
            }

            if(std::string name = format_catalog_name(entry.components); std::ranges::find(names, name) == names.end())
            {
                names.push_back(std::move(name));
            }

            if(names.size() == most)
            {
                return names;
            }
        }

        if(!names.empty())
        {
            break;
        }
    }

    return names;
}

//------------------------------------------------------------------------------

/// The text between two `>>`, without the spaces that surround a separator.
static std::string_view trim_separator_spaces(std::string_view _text, bool _trim_front, bool _trim_back)
{
    while(_trim_front && !_text.empty() && _text.front() == ' ')
    {
        _text.remove_prefix(1);
    }

    while(_trim_back && !_text.empty() && _text.back() == ' ')
    {
        _text.remove_suffix(1);
    }

    return _text;
}

//------------------------------------------------------------------------------

[[noreturn]] static void invalid_locator(std::string_view _expression, std::string_view _reason)
{
    throw std::invalid_argument("invalid GUI locator '" + std::string(_expression) + "': " + std::string(_reason));
}

//------------------------------------------------------------------------------

/// The ordinal after `#`: a positive integer, nothing else.
static std::size_t parse_role_occurrence(std::string_view _number, std::string_view _expression)
{
    std::size_t occurrence = 0;
    const auto [end, error] = std::from_chars(_number.data(), _number.data() + _number.size(), occurrence);
    if(_number.empty() || error != std::errc {} || end != _number.data() + _number.size() || occurrence == 0)
    {
        invalid_locator(_expression, "a role occurrence must be a positive integer after '#'");
    }

    return occurrence;
}

//------------------------------------------------------------------------------

/// One scope of a locator: its name parts, split on unescaped `/`, and its ordinal when it is the last one.
static locator_segment parse_segment(std::string_view _source, std::string_view _expression, bool _last)
{
    locator_segment segment;
    std::string component;

    const auto close_component =
        [&]
        {
            if(component.empty() || is_space(component.front()) || is_space(component.back()))
            {
                invalid_locator(_expression, "name parts must be non-empty and cannot start or end with whitespace");
            }

            segment.components.push_back(std::move(component));
            component.clear();
        };

    for(std::size_t i = 0 ; i < _source.size() ; ++i)
    {
        const char character = _source[i];
        if(static_cast<unsigned char>(character) < 0x20 || character == 0x7F)
        {
            invalid_locator(_expression, "names cannot contain control characters");
        }

        // The object name keeps the escape, as Qt names do; the component drops it, as the XML does.
        if(character == '\\')
        {
            if(++i == _source.size())
            {
                invalid_locator(_expression, "a name ends with an incomplete escape");
            }

            const char escaped_character = _source[i];
            if(escaped_character != '/' && escaped_character != '#' && escaped_character != '\\')
            {
                invalid_locator(_expression, "only '/', '#' and '\\' can be escaped");
            }

            segment.object_name += character;
            segment.object_name += escaped_character;
            component           += escaped_character;
        }
        else if(character == '#')
        {
            if(!_last)
            {
                invalid_locator(_expression, "a role occurrence can only end the locator");
            }

            segment.role_occurrence = parse_role_occurrence(_source.substr(i + 1), _expression);
            break;
        }
        else
        {
            if(character == '/')
            {
                close_component();
            }
            else
            {
                component += character;
            }

            segment.object_name += character;
        }
    }

    close_component();
    return segment;
}

/// The next `>>` from a position, past the text of a filter, which may hold anything.
static std::size_t find_separator(std::string_view _expression, std::size_t _start)
{
    const std::size_t part = _expression.find_first_not_of(' ', _start);
    if(part == std::string_view::npos || !_expression.substr(part).starts_with("@text("))
    {
        return _expression.find(">>", _start);
    }

    // Inside `@text(...)`, an escaped `)` does not close it.
    for(std::size_t index = part ; index < _expression.size() ; ++index)
    {
        if(_expression[index] == '\\')
        {
            ++index;
        }
        else if(_expression[index] == ')')
        {
            return _expression.find(">>", index);
        }
    }

    invalid_locator(_expression, "the text filter is not closed");
}

//------------------------------------------------------------------------------

/// The argument of `@filter(...)`, unescaped.
static std::string filter_argument(std::string_view _source, std::string_view _expression)
{
    const std::size_t open = _source.find('(');
    if(open == std::string_view::npos || _source.back() != ')' || open + 2 == _source.size())
    {
        invalid_locator(_expression, "a filter takes a non-empty argument between parentheses");
    }

    std::string argument;
    const std::string_view source = _source.substr(open + 1, _source.size() - open - 2);
    for(std::size_t index = 0 ; index < source.size() ; ++index)
    {
        if(source[index] == '\\' && index + 1 < source.size())
        {
            ++index;
        }

        argument += source[index];
    }

    return argument;
}

//------------------------------------------------------------------------------

detail::locator::locator(std::string_view _expression)
{
    if(_expression.empty())
    {
        invalid_locator(_expression, "the expression is empty");
    }

    // `>>` is the only scope separator; slashes stay part of the object name.
    std::size_t start = 0;
    bool filtered     = false;
    while(start < _expression.size())
    {
        const std::size_t separator   = find_separator(_expression, start);
        const std::size_t end         = separator == std::string_view::npos ? _expression.size() : separator;
        const std::string_view source = trim_separator_spaces(
            _expression.substr(start, end - start),
            start != 0,
            separator != std::string_view::npos
        );
        if(source.empty() || source.front() == '>' || source.back() == '>')
        {
            invalid_locator(_expression, "scopes must be separated by exactly '>>'");
        }

        // A reserved part: the modal dialog, or a filter of what the names before it found.
        if(source.front() == '@')
        {
            if(source == "@dialog")
            {
                if(start != 0)
                {
                    invalid_locator(_expression, "@dialog can only start a locator");
                }

                m_in_modal_dialog = true;
            }
            else
            {
                if(m_segments.empty() && !m_in_modal_dialog)
                {
                    invalid_locator(_expression, "a filter applies to a name or to @dialog");
                }

                filtered = true;
                if(source == "@hidden")
                {
                    m_hidden = true;
                }
                else if(source.starts_with("@type("))
                {
                    m_type = filter_argument(source, _expression);
                }
                else if(source.starts_with("@text("))
                {
                    m_text = filter_argument(source, _expression);
                }
                else
                {
                    invalid_locator(_expression, "unknown filter: only @type(), @text() and @hidden exist");
                }
            }
        }
        else
        {
            if(filtered)
            {
                invalid_locator(_expression, "filters end a locator");
            }

            // An ordinal counts what the filters keep, so it stays valid in front of them.
            const std::size_t next = separator == std::string_view::npos
                                     ? std::string_view::npos
                                     : _expression.find_first_not_of(' ', separator + 2);
            const bool last = next == std::string_view::npos || _expression[next] == '@';
            m_segments.push_back(parse_segment(source, _expression, last));
        }

        if(separator == std::string_view::npos)
        {
            break;
        }

        start = separator + 2;
        if(start == _expression.size())
        {
            invalid_locator(_expression, "the expression cannot end with '>>'");
        }
    }
}

//------------------------------------------------------------------------------

const std::vector<locator_segment>& detail::locator::segments() const noexcept
{
    return m_segments;
}

//------------------------------------------------------------------------------

bool detail::locator::in_modal_dialog() const noexcept
{
    return m_in_modal_dialog;
}

//------------------------------------------------------------------------------

bool detail::locator::passes_filters(const QWidget& _widget) const
{
    // Each filter that is present must hold.
    return (!m_hidden || !detail::is_visible(_widget))
           && (!m_type.has_value() || _widget.inherits(m_type->c_str()))
           && (!m_text.has_value() || detail::displayed_text(_widget) == QString::fromStdString(*m_text));
}

//------------------------------------------------------------------------------

/// The scopes that `@dialog` starts from: the innermost modal dialog, none without one. Empty without `@dialog`.
static std::optional<std::vector<QWidget*> > dialog_scopes(bool _in_modal_dialog)
{
    static_cast<void>(gui_application());
    if(!_in_modal_dialog)
    {
        return std::vector<QWidget*> {};
    }

    QWidget* const modal = QApplication::activeModalWidget();
    return modal == nullptr ? std::nullopt : std::optional(std::vector<QWidget*> {modal});
}

//------------------------------------------------------------------------------

std::vector<QWidget*> detail::locator::resolve_widgets(bool _include_hidden) const
{
    auto scopes = dialog_scopes(m_in_modal_dialog);
    if(!scopes.has_value())
    {
        return {};
    }

    const bool include_hidden = _include_hidden || m_hidden;
    std::vector<QWidget*> widgets;
    if(!m_segments.empty())
    {
        widgets = resolve_named_widgets(m_segments, m_segments.size(), include_hidden, std::move(*scopes));
    }
    else if(m_type.has_value() || m_text.has_value() || m_hidden)
    {
        // Nothing but filters: what they keep inside the dialog, its unnamed widgets included.
        for(QWidget* widget : widgets_of_all_windows())
        {
            if((include_hidden || detail::is_visible(*widget)) && is_in_scope(*widget, *scopes))
            {
                widgets.push_back(widget);
            }
        }
    }
    else
    {
        // `@dialog` alone: the dialog itself.
        widgets = std::move(*scopes);
    }

    std::erase_if(widgets, [this](const QWidget* _widget){return !passes_filters(*_widget);});
    return pick_occurrence(std::move(widgets), m_segments.empty() ? std::nullopt : m_segments.back().role_occurrence);
}

//------------------------------------------------------------------------------

std::size_t detail::locator::count_hidden_widgets() const
{
    // Ordinals describe visible widgets, so hidden candidates are counted by name instead.
    locator unnumbered(*this);
    if(!unnumbered.m_segments.empty())
    {
        unnumbered.m_segments.back().role_occurrence.reset();
    }

    const auto widgets = unnumbered.resolve_widgets(true);
    return static_cast<std::size_t>(
        std::ranges::count_if(widgets, [](const QWidget* _widget){return !detail::is_visible(*_widget);})
    );
}

//------------------------------------------------------------------------------

std::vector<QAction*> detail::locator::resolve_actions() const
{
    // An action has a name, and no state hidden from a user that a filter would tell apart.
    auto scopes = dialog_scopes(m_in_modal_dialog);
    if(m_segments.empty() || m_hidden || !scopes.has_value())
    {
        return {};
    }

    // The scopes are widgets: only the last segment names the action.
    const locator_segment& target = m_segments.back();
    *scopes = resolve_named_widgets(m_segments, m_segments.size() - 1, false, std::move(*scopes));

    // An action is found through the widgets it is added to, each once.
    std::vector<QAction*> matches;
    const QString object_name = QString::fromStdString(target.object_name);
    for(QWidget* widget : widgets_of_all_windows())
    {
        for(QAction* action : widget->actions())
        {
            if(action->objectName() != object_name || std::ranges::find(matches, action) != matches.end()
               || (m_type.has_value() && !action->inherits(m_type->c_str()))
               || (m_text.has_value() && action->text() != QString::fromStdString(*m_text)))
            {
                continue;
            }

            // In scope when one of the widgets that show it is.
            if(!scopes->empty())
            {
                const bool in_scope = std::ranges::any_of(
                    action->associatedObjects(),
                    [&scopes](QObject* _object)
                    {
                        QWidget* const associated_widget = qobject_cast<QWidget*>(_object);
                        return associated_widget != nullptr && is_in_or_in_scope(*associated_widget, *scopes);
                    });
                if(!in_scope)
                {
                    continue;
                }
            }

            matches.push_back(action);
        }
    }

    return pick_occurrence(std::move(matches), target.role_occurrence);
}

//------------------------------------------------------------------------------

locator_expression::locator_expression(std::string _expression) :
    m_expression(std::move(_expression))
{
    // A typo fails where it is written, not at the first verb.
    static_cast<void>(locator(m_expression));
}

//------------------------------------------------------------------------------

locator_expression locator_expression::operator>>(std::string_view _name) const
{
    return locator_expression(m_expression + " >> " + std::string(_name));
}

//------------------------------------------------------------------------------

locator_expression locator_expression::type(std::string_view _class_name) const
{
    return locator_expression(m_expression + " >> @type(" + std::string(_class_name) + ")");
}

//------------------------------------------------------------------------------

locator_expression locator_expression::text(std::string_view _text) const
{
    // Only what would end the filter is escaped.
    std::string escaped;
    for(const char character : _text)
    {
        if(character == ')' || character == '\\')
        {
            escaped += '\\';
        }

        escaped += character;
    }

    return locator_expression(m_expression + " >> @text(" + escaped + ")");
}

//------------------------------------------------------------------------------

locator_expression locator_expression::hidden() const
{
    return locator_expression(m_expression + " >> @hidden");
}

//------------------------------------------------------------------------------

std::string_view locator_expression::expression() const noexcept
{
    return m_expression;
}

//------------------------------------------------------------------------------

locator_expression::operator std::string_view() const noexcept
{
    return m_expression;
}

//------------------------------------------------------------------------------

locator_expression loc(std::string_view _expression)
{
    return locator_expression(std::string(_expression));
}

//------------------------------------------------------------------------------

locator_expression dialog()
{
    return locator_expression("@dialog");
}

//------------------------------------------------------------------------------

std::string detail::compose_name(std::initializer_list<std::string_view> _components)
{
    std::string result;
    for(const std::string_view component : _components)
    {
        if(!result.empty())
        {
            result += '/';
        }

        append_escaped(result, component);
    }

    return result;
}

//------------------------------------------------------------------------------

std::string menu_item(std::string_view _menu, std::string_view _item)
{
    return detail::compose_name({_menu, _item});
}

//------------------------------------------------------------------------------

/// The displayed texts of an item path, split on unescaped `/`.
static std::vector<std::string> parse_item_path(std::string_view _path)
{
    if(_path.empty())
    {
        throw std::invalid_argument("item path must not be empty");
    }

    std::vector<std::string> components;
    std::string component;
    for(std::size_t index = 0 ; index < _path.size() ; ++index)
    {
        const char character = _path[index];
        if(character == '\\')
        {
            ++index;
            if(index == _path.size() || (_path[index] != '/' && _path[index] != '\\'))
            {
                throw std::invalid_argument("item path contains an invalid escape");
            }

            component += _path[index];
        }
        else if(character == '/')
        {
            if(component.empty())
            {
                throw std::invalid_argument("item path components must not be empty");
            }

            components.push_back(std::move(component));
            component.clear();
        }
        else
        {
            component += character;
        }
    }

    if(component.empty())
    {
        throw std::invalid_argument("item path components must not be empty");
    }

    components.push_back(std::move(component));
    return components;
}

//------------------------------------------------------------------------------

item_locator::item_locator(std::string_view _view, std::string_view _path, bool _by_row) :
    m_view_expression(_view),
    m_path(parse_item_path(_path)),
    m_by_row(_by_row)
{
    // A typo in the view fails here, as for a locator.
    static_cast<void>(locator(_view));
    if(!_by_row)
    {
        return;
    }

    for(const std::string& row : m_path)
    {
        // The text of a row number, as the comparison sees it: "01" could never match.
        int value       = 0;
        const char* end = row.data() + row.size();
        const auto [position, error] = std::from_chars(row.data(), end, value);
        if(error != std::errc {} || position != end || std::to_string(value) != row || value < 0)
        {
            throw std::invalid_argument("item row '" + row + "' is not a zero-based row number");
        }
    }
}

//------------------------------------------------------------------------------

item_locator item_locator::column(std::size_t _column) const
{
    if(_column > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
        throw std::invalid_argument("item column is too large");
    }

    item_locator result(*this);
    result.m_column = _column;
    return result;
}

//------------------------------------------------------------------------------

std::string_view item_locator::view_expression() const noexcept
{
    return m_view_expression;
}

//------------------------------------------------------------------------------

std::size_t item_locator::depth() const noexcept
{
    return m_path.size();
}

//------------------------------------------------------------------------------

std::vector<QModelIndex> item_locator::matching_indexes(QAbstractItemView& _view) const
{
    return matching_prefix(_view, m_path.size());
}

//------------------------------------------------------------------------------

std::vector<QModelIndex> item_locator::matching_prefix(QAbstractItemView& _view, std::size_t _depth) const
{
    static_cast<void>(gui_application());
    if(_depth == 0 || _depth > m_path.size())
    {
        throw std::invalid_argument("item path depth is out of range");
    }

    QAbstractItemModel* const model = _view.model();
    if(model == nullptr)
    {
        return {};
    }

    std::vector<QModelIndex> parents {_view.rootIndex()};
    // Each component is matched among the children of the previous component.
    for(std::size_t depth = 0 ; depth < _depth ; ++depth)
    {
        const QString text = QString::fromStdString(m_path[depth]);
        std::vector<QModelIndex> matches;
        for(const QModelIndex& parent : parents)
        {
            for(int row = 0 ; row < model->rowCount(parent) ; ++row)
            {
                const QModelIndex candidate = model->index(row, 0, parent);
                const bool matching         = m_by_row
                                              ? std::to_string(row) == m_path[depth]
                                              : candidate.data(Qt::DisplayRole).toString() == text;
                if(!matching)
                {
                    continue;
                }

                if(depth + 1 == _depth)
                {
                    // A prefix keeps the tree's first column; only a complete path applies column().
                    const std::size_t column = _depth == m_path.size() ? m_column : 0;
                    if(std::cmp_greater_equal(column, model->columnCount(parent)))
                    {
                        throw std::invalid_argument("item column is outside the view model");
                    }

                    matches.push_back(model->index(row, static_cast<int>(column), parent));
                }
                else
                {
                    matches.push_back(candidate);
                }
            }
        }

        parents = std::move(matches);
        if(parents.empty())
        {
            break;
        }
    }

    return parents;
}

} // namespace sight::ui::test
