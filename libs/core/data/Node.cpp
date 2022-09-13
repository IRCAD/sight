/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

//FIXME :This needs to be include first for GCC
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
//
#include "data/Node.hpp"
#include "data/registry/macros.hpp"
#include "data/Exception.hpp"
#include "data/Port.hpp"

SIGHT_REGISTER_DATA(sight::data::Node);

namespace sight::data
{

const core::com::Signals::SignalKeyType Node::s_UPDATED_SIG = "updated";

//------------------------------------------------------------------------------

Node::Node(data::Object::Key key) :
    m_sigUpdated(UpdatedSignalType::New())
{
    SIGHT_NOT_USED(key);
    m_signals(s_UPDATED_SIG, m_sigUpdated);
}

//------------------------------------------------------------------------------

void Node::addInputPort(const data::Port::sptr& port)
{
    m_inputs.push_back(port);
}

//------------------------------------------------------------------------------

void Node::addOutputPort(const data::Port::sptr& port)
{
    m_outputs.push_back(port);
}

//------------------------------------------------------------------------------

Node::PortContainer& Node::getInputPorts()
{
    return m_inputs;
}

//------------------------------------------------------------------------------

const Node::PortContainer& Node::getInputPorts() const
{
    return m_inputs;
}

//------------------------------------------------------------------------------

Node::PortContainer& Node::getOutputPorts()
{
    return m_outputs;
}

//------------------------------------------------------------------------------

const Node::PortContainer& Node::getOutputPorts() const
{
    return m_outputs;
}

//------------------------------------------------------------------------------

void Node::setObject(const data::Object::sptr& object)
{
    m_object = object;
}

//------------------------------------------------------------------------------

data::Object::sptr Node::getObject() const
{
    return m_object;
}

//------------------------------------------------------------------------------

Port::sptr Node::findPort(const std::string& identifier, bool modeInput) const
{
    if(modeInput)
    {
        for(const auto& input : m_inputs)
        {
            if(input->getIdentifier() == identifier)
            {
                return input;
            }
        }
    }
    else
    {
        for(const auto& output : m_outputs)
        {
            if(output->getIdentifier() == identifier)
            {
                return output;
            }
        }
    }

    return {};
}

//------------------------------------------------------------------------------

void Node::shallowCopy(const Object::csptr& source)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !other
    );

    m_inputs.clear();
    m_outputs.clear();

    if(other->getObject())
    {
        core::tools::Object::sptr object = data::factory::New(other->getObject()->getClassname());
        SIGHT_ASSERT("The instantiation of '" << other->getObject()->getClassname() << "' failed", object);
        m_object = data::Object::dynamicCast(object);
        m_object->shallowCopy(other->m_object);
    }

    m_inputs  = other->m_inputs;
    m_outputs = other->m_outputs;

    BaseClass::shallowCopy(other);
}

//------------------------------------------------------------------------------

void Node::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    Node::csptr other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
        ),
        !bool(other)
    );

    m_inputs.clear();
    m_outputs.clear();

    m_object = data::Object::copy(other->m_object, cache);

    for(const data::Port::sptr& port : other->m_inputs)
    {
        this->addInputPort(data::Object::copy(port, cache));
    }

    for(const data::Port::sptr& port : other->m_outputs)
    {
        this->addOutputPort(data::Object::copy(port, cache));
    }

    BaseClass::deepCopy(other, cache);
}

//------------------------------------------------------------------------------

bool Node::operator==(const Node& other) const noexcept
{
    if(!core::tools::is_equal(m_inputs, other.m_inputs)
       || !core::tools::is_equal(m_outputs, other.m_outputs)
       || !core::tools::is_equal(m_object, other.m_object))
    {
        return false;
    }

    // Super class last
    return BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool Node::operator!=(const Node& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
