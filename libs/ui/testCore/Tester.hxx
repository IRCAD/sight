/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

namespace sight::ui::test_core
{

//------------------------------------------------------------------------------

template<typename T>
void Tester::take(
    const std::string& _component_description,
    std::function<T()> _graphic_component,
    std::function<bool(T)> _condition,
    int _timeout
)
{
    if(m_verboseMode)
    {
        qDebug() << "Waiting up to" << _timeout << "ms for" << QString::fromStdString(_component_description)
        << "to appear";
    }

    T component;
    const bool ok = waitForAsynchronously(
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

    m_graphicComponent     = component;
    m_componentDescription = _component_description;
}

//------------------------------------------------------------------------------

template<typename T>
void Tester::doubt(const std::string& _result_description, std::function<bool(T)> _result, int _timeout)
{
    if(m_graphicComponent == nullptr)
    {
        return;
    }

    m_resultDescription = _result_description;
    T component = get<T>();

    if(m_verboseMode)
    {
        qDebug() << "Waiting up to" << _timeout << "ms for" << QString::fromStdString(_result_description)
        << "to become true";
    }

    const bool ok = waitForAsynchronously(
        [&_result, component]() -> bool
        {
            return _result(component);
        },
        _timeout
    );
    if(!ok)
    {
        fail(
            "The interaction on the component \"" + m_componentDescription + "\" didn't yield the result \"" + _result_description
            + '"'
        );
    }

    m_interactions.clear();
    m_resultDescription = "";
}

//------------------------------------------------------------------------------

template<typename T>
void Tester::yields(
    const std::string& _component_description,
    std::function<T(QObject*)> _graphic_component,
    std::function<bool(T)> _condition,
    int _timeout
)
{
    if(m_graphicComponent == nullptr)
    {
        return;
    }

    m_resultDescription = _component_description + " appears";

    if(m_verboseMode)
    {
        qDebug() << "Waiting up to" << _timeout << "ms for" << QString::fromStdString(_component_description)
        << "to appear";
    }

    T new_graphic_component = nullptr;
    const bool ok           = waitForAsynchronously(
        [this, &_graphic_component, &_condition, &new_graphic_component]() -> bool
        {
            new_graphic_component = _graphic_component(m_graphicComponent);
            return new_graphic_component != nullptr && _condition(new_graphic_component);
        },
        _timeout
    );
    if(!ok)
    {
        fail("\"" + m_componentDescription + " didn't yield the component \"" + _component_description + '"');
    }

    m_graphicComponent     = new_graphic_component;
    m_componentDescription = _component_description;
    m_resultDescription    = "";
}

//------------------------------------------------------------------------------

template<typename T>
void Tester::yields(
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
void Tester::maybeTake(
    const std::string& _component_description,
    std::function<T()> _graphic_component,
    std::function<bool(T)> _condition,
    int _timeout
)
{
    if(m_verboseMode)
    {
        qDebug() << "Waiting up to" << _timeout << "ms for" << QString::fromStdString(_component_description)
        << "to appear";
    }

    T component;
    const bool ok = waitForAsynchronously(
        [&_graphic_component, &_condition, &component]() -> bool
        {
            component = _graphic_component();
            return component != nullptr && _condition(component);
        },
        _timeout
    );
    if(!ok)
    {
        m_graphicComponent = nullptr;
    }

    m_graphicComponent     = component;
    m_componentDescription = _component_description;
}

//------------------------------------------------------------------------------

template<typename T>
void Tester::doSomething(std::function<void(T)> _f)
{
    _f(get<T>());
}

//------------------------------------------------------------------------------

template<typename T>
void Tester::doSomethingAsynchronously(std::function<void(T)> _f)
{
    T component = get<T>();
    qApp->postEvent(qApp, new TestEvent([component, _f]{_f(component);}));
}

//------------------------------------------------------------------------------

template<typename T>
bool Tester::is_a()
{
    return qobject_cast<T>(m_graphicComponent) != nullptr;
}

//------------------------------------------------------------------------------

template<typename T>
T Tester::get()
{
    T res = qobject_cast<T>(m_graphicComponent);
    if(res == nullptr)
    {
        fail("Couldn't cast \"" + m_componentDescription + "\" to \"" + typeid(T).name() + '"');
    }

    return res;
}

} // namespace sight::ui::testCore
