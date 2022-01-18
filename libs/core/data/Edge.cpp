/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/Edge.hpp"

#include "data/Exception.hpp"
#include "data/registry/macros.hpp"

SIGHT_REGISTER_DATA(sight::data::Edge);

namespace sight::data
{

std::string Edge::NATURE_FLOW = "flow";
std::string Edge::NATURE_DATA = "data";

//------------------------------------------------------------------------------

Edge::Edge(data::Object::Key) :
    m_fromPortIdentifier("not defined"),
    m_toPortIdentifier("not defined"),
    m_nature("not defined")
{
}

//------------------------------------------------------------------------------

Edge::~Edge()
{
}

//------------------------------------------------------------------------------

void Edge::setIdentifiers(const std::string& fromPortIndentifier, const std::string& toPortIndentifier)
{
    m_fromPortIdentifier = fromPortIndentifier;
    m_toPortIdentifier   = toPortIndentifier;
}

//------------------------------------------------------------------------------

std::pair<std::string, std::string> Edge::getIdentifiers() const
{
    return make_pair(m_fromPortIdentifier, m_toPortIdentifier);
}

//------------------------------------------------------------------------------

std::string Edge::getFromPortID() const
{
    return m_fromPortIdentifier;
}

//------------------------------------------------------------------------------

std::string Edge::getToPortID() const
{
    return m_toPortIdentifier;
}

//------------------------------------------------------------------------------

std::string Edge::getPortID(bool upStream) const
{
    return upStream ? m_fromPortIdentifier : m_toPortIdentifier;
}

//------------------------------------------------------------------------------

void Edge::setNature(std::string nature)
{
    m_nature = nature;
}

//------------------------------------------------------------------------------

const std::string& Edge::getNature() const
{
    return m_nature;
}

//------------------------------------------------------------------------------
void Edge::shallowCopy(const Object::csptr& _source)
{
    Edge::csptr other = Edge::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );

    this->fieldShallowCopy(_source);
    m_fromPortIdentifier = other->m_fromPortIdentifier;
    m_toPortIdentifier   = other->m_toPortIdentifier;
    m_nature             = other->m_nature;
}

//------------------------------------------------------------------------------

void Edge::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Edge::csptr other = Edge::dynamicConstCast(_source);
    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
            + " to " + this->getClassname()
        ),
        !bool(other)
    );
    this->fieldDeepCopy(_source, cache);
    m_fromPortIdentifier = other->m_fromPortIdentifier;
    m_toPortIdentifier   = other->m_toPortIdentifier;
    m_nature             = other->m_nature;
}

//------------------------------------------------------------------------------

bool Edge::operator==(const Edge& other) const noexcept
{
    if(m_fromPortIdentifier != other.m_fromPortIdentifier
       || m_toPortIdentifier != other.m_toPortIdentifier
       || m_nature != other.m_nature)
    {
        return false;
    }

    // Super class last
    return Object::operator==(other);
}

//------------------------------------------------------------------------------

bool Edge::operator!=(const Edge& other) const noexcept
{
    return !(*this == other);
}

} // namespace sight::data
