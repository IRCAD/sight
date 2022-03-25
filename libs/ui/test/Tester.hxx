/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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
void Tester::take(
    const std::string& componentDescription,
    std::function<T()> graphicComponent,
    std::function<bool(T)> condition,
    int timeout
)
{
    if(m_verboseMode)
    {
        qDebug() << "Waiting up to" << timeout << "ms for" << QString::fromStdString(componentDescription)
        << "to appear";
    }

    T component;
    const bool ok = waitForAsynchronously(
        [&graphicComponent, &condition, &component]() -> bool
        {
            component = graphicComponent();
            return component != nullptr && condition(component);
        },
        timeout
    );
    if(!ok)
    {
        fail("\"" + componentDescription + "\" has never showed up");
    }

    m_graphicComponent     = component;
    m_componentDescription = componentDescription;
}

//------------------------------------------------------------------------------

template<typename T>
void Tester::doubt(const std::string& resultDescription, std::function<bool(T)> result, int timeout)
{
    if(m_graphicComponent == nullptr)
    {
        return;
    }

    m_resultDescription = resultDescription;
    T component = qobject_cast<T>(m_graphicComponent);
    if(component == nullptr)
    {
        fail("Couldn't cast \"" + m_componentDescription + "\" to \"" + typeid(T).name() + "\"");
    }

    if(m_verboseMode)
    {
        qDebug() << "Waiting up to" << timeout << "ms for" << QString::fromStdString(resultDescription)
        << "to become true";
    }

    const bool ok = waitForAsynchronously(
        [&result, component]() -> bool
        {
            return result(component);
        },
        timeout
    );
    if(!ok)
    {
        fail(
            "The interaction on the component \"" + m_componentDescription + "\" didn't yield the result \"" + resultDescription
            + '"'
        );
    }

    m_interactions.clear();
    m_resultDescription = "";
}

//------------------------------------------------------------------------------

template<typename T>
void Tester::yields(
    const std::string& componentDescription,
    std::function<T(QObject*)> graphicComponent,
    std::function<bool(T)> condition,
    int timeout
)
{
    if(m_graphicComponent == nullptr)
    {
        return;
    }

    m_resultDescription = componentDescription + " appears";

    if(m_verboseMode)
    {
        qDebug() << "Waiting up to" << timeout << "ms for" << QString::fromStdString(componentDescription)
        << "to appear";
    }

    T newGraphicComponent = nullptr;
    const bool ok         = waitForAsynchronously(
        [this, &graphicComponent, &condition, &newGraphicComponent]() -> bool
        {
            newGraphicComponent = graphicComponent(m_graphicComponent);
            return newGraphicComponent != nullptr && condition(newGraphicComponent);
        },
        timeout
    );
    if(!ok)
    {
        fail("\"" + m_componentDescription + " didn't yield the component \"" + componentDescription + '"');
    }

    m_graphicComponent     = newGraphicComponent;
    m_componentDescription = componentDescription;
    m_resultDescription    = "";
}

//------------------------------------------------------------------------------

template<typename T>
void Tester::maybeTake(
    const std::string& componentDescription,
    std::function<T()> graphicComponent,
    std::function<bool(T)> condition,
    int timeout
)
{
    if(m_verboseMode)
    {
        qDebug() << "Waiting up to" << timeout << "ms for" << QString::fromStdString(componentDescription)
        << "to appear";
    }

    T component;
    const bool ok = waitForAsynchronously(
        [&graphicComponent, &condition, &component]() -> bool
        {
            component = graphicComponent();
            return component != nullptr && condition(component);
        },
        timeout
    );
    if(!ok)
    {
        m_graphicComponent = nullptr;
    }

    m_graphicComponent     = component;
    m_componentDescription = componentDescription;
}

//------------------------------------------------------------------------------

template<typename T>
void Tester::doSomething(std::function<void(T)> f)
{
    T component = qobject_cast<T>(m_graphicComponent);
    if(component == nullptr)
    {
        fail("Couldn't cast \"" + m_componentDescription + "\" to \"" + typeid(T).name() + '"');
    }

    f(component);
}

//------------------------------------------------------------------------------

template<typename T>
void Tester::doSomethingAsynchronously(std::function<void(T)> f)
{
    T component = qobject_cast<T>(m_graphicComponent);
    if(component == nullptr)
    {
        fail("Couldn't cast \"" + m_componentDescription + "\" to \"" + typeid(T).name() + '"');
    }

    qApp->postEvent(qApp, new TestEvent([component, f]{f(component);}));
}

//------------------------------------------------------------------------------

template<typename T>
T Tester::get()
{
    return qobject_cast<T>(m_graphicComponent);
}

} // namespace sight::ui::test
