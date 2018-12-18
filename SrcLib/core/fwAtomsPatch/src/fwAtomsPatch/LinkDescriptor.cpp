/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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

#include "fwAtomsPatch/LinkDescriptor.hpp"

namespace fwAtomsPatch
{

LinkDescriptor::LinkDescriptor() :
    m_weight(1), m_context(""), m_originVersion(""), m_targetVersion("")
{
}

// ----------------------------------------------------------------------------

LinkDescriptor::LinkDescriptor(const std::string& context, const std::string& originVersion,
                               const std::string& targetVersion, const std::string& patcher,
                               LinksType links, int weight) :
    m_weight(weight), m_context(context), m_originVersion(originVersion), m_targetVersion(targetVersion),
    m_patcher(patcher), m_links(links)
{
}

// ----------------------------------------------------------------------------

LinkDescriptor::~LinkDescriptor()
{
}

} //namespace fwAtomsPatch
