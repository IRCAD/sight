/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "service/IXMLParser.hpp"

namespace sight::service
{

IXMLParser::IXMLParser()
{
}

IXMLParser::~IXMLParser()
{
}

//------------------------------------------------------------------------------

void IXMLParser::starting()
{
}

//------------------------------------------------------------------------------

void IXMLParser::stopping()
{
}

//------------------------------------------------------------------------------

void IXMLParser::configuring()
{
}

//------------------------------------------------------------------------------

void IXMLParser::updating()
{
    core::runtime::ConfigurationElement::Iterator configEltIter;
    for(configEltIter = this->m_configuration->begin() ; !(configEltIter == this->m_configuration->end()) ;
        ++configEltIter)
    {
        SIGHT_ASSERT("ACH => still used ?", (*configEltIter)->getName() != "object");
    }
}

//-----------------------------------------------------------------------------

void IXMLParser::setObjectConfig(core::runtime::ConfigurationElement::csptr _cfgElem)
{
    m_cfg = _cfgElem;
}

//-----------------------------------------------------------------------------

void IXMLParser::createConfig(core::tools::Object::sptr)
{
}

//-----------------------------------------------------------------------------

void IXMLParser::startConfig()
{
}

//-----------------------------------------------------------------------------

void IXMLParser::updateConfig()
{
}

//-----------------------------------------------------------------------------

void IXMLParser::stopConfig()
{
}

//-----------------------------------------------------------------------------

void IXMLParser::destroyConfig()
{
}

//-----------------------------------------------------------------------------

} // namespace sight::service
