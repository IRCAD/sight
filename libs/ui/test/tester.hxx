/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

namespace sight::ui::test
{

//------------------------------------------------------------------------------

template<typename T>
void tester::take(
    const std::string& _component_description,
    std::function<T()> _graphic_component,
    std::function<bool(T)> _condition,
    int _timeout
)
{
    if(m_verbose_mode)
    {
        qDebug() << "Waiting up to" << _timeout << "ms for" << QString::fromStdString(_component_description)
        << "to appear";
    }

    T component;
    const bool ok = wait_for_asynchronously(
        [&_graphic_component, &_condition, &component]() -> bool
        {
            component = _graphic_component();
            return component != nullptr && _condition(component);
        },
        _timeout
    );
    if(!ok)
    {
        fail("\"" + _component_description + "\" has never showed up");
    }

    m_graphic_component     = component;
    m_component_description = _component_description;
}

//------------------------------------------------------------------------------

template<typename T>
void tester::doubt(const std::string& _result_description, std::function<bool(T)> _result, int _timeout)
{
    if(m_graphic_component == nullptr)
    {
        return;
    }

    m_result_description = _result_description;
    T component = get<T>();

    if(m_verbose_mode)
    {
        qDebug() << "Waiting up to" << _timeout << "ms for" << QString::fromStdString(_result_description)
        << "to become true";
    }

    const bool ok = wait_for_asynchronously(
        [&_result, component]() -> bool
        {
            return _result(component);
        },
        _timeout
    );
    if(!ok)
    {
        fail(
            "The interaction on the component \"" + m_component_description + "\" didn't yield the result \""
            + _result_description
            + '"'
        );
    }

    m_interactions.clear();
    m_result_description = "";
}

//------------------------------------------------------------------------------

template<typename T>
void tester::yields(
    const std::string& _component_description,
    std::function<T(QObject*)> _graphic_component,
    std::function<bool(T)> _condition,
    int _timeout
)
{
    if(m_graphic_component == nullptr)
    {
        return;
    }

    m_result_description = _component_description + " appears";

    if(m_verbose_mode)
    {
        qDebug() << "Waiting up to" << _timeout << "ms for" << QString::fromStdString(_component_description)
        << "to appear";
    }

    T new_graphic_component = nullptr;
    const bool ok           = wait_for_asynchronously(
        [this, &_graphic_component, &_condition, &new_graphic_component]() -> bool
        {
            new_graphic_component = _graphic_component(m_graphic_component);
            return new_graphic_component != nullptr && _condition(new_graphic_component);
        },
        _timeout
    );
    if(!ok)
    {
        fail("\"" + m_component_description + " didn't yield the component \"" + _component_description + '"');
    }

    m_graphic_component     = new_graphic_component;
    m_component_description = _component_description;
    m_result_description    = "";
}

//------------------------------------------------------------------------------

template<typename T>
void tester::yields(
    const std::string& _component_description,
    const std::string& _object_name,
    std::function<bool(QObject*)> _condition,
    int _timeout
)
{
    yields(
        _component_description,
        [&_object_name](QObject* _old)
        {
            return _old->findChild<T>(_object_name == "" ? QString() : QString::fromStdString(_object_name));
        },
        _condition,
        _timeout
    );
}

//------------------------------------------------------------------------------

template<typename T>
void tester::maybe_take(
    const std::string& _component_description,
    std::function<T()> _graphic_component,
    std::function<bool(T)> _condition,
    int _timeout
)
{
    if(m_verbose_mode)
    {
        qDebug() << "Waiting up to" << _timeout << "ms for" << QString::fromStdString(_component_description)
        << "to appear";
    }

    T component;
    const bool ok = wait_for_asynchronously(
        [&_graphic_component, &_condition, &component]() -> bool
        {
            component = _graphic_component();
            return component != nullptr && _condition(component);
        },
        _timeout
    );
    if(!ok)
    {
        m_graphic_component = nullptr;
    }

    m_graphic_component     = component;
    m_component_description = _component_description;
}

//------------------------------------------------------------------------------

template<typename T>
void tester::do_something(std::function<void(T)> _f)
{
    _f(get<T>());
}

//------------------------------------------------------------------------------

template<typename T>
void tester::do_something_asynchronously(std::function<void(T)> _f)
{
    T component = get<T>();
    qApp->postEvent(qApp, new test_event([component, _f]{_f(component);}));
}

//------------------------------------------------------------------------------

template<typename T>
bool tester::is_a()
{
    return qobject_cast<T>(m_graphic_component) != nullptr;
}

//------------------------------------------------------------------------------

template<typename T>
T tester::get()
{
    T res = qobject_cast<T>(m_graphic_component);
    if(res == nullptr)
    {
        fail("Couldn't cast \"" + m_component_description + "\" to \"" + typeid(T).name() + '"');
    }

    return res;
}

} // namespace sight::ui::testCore
