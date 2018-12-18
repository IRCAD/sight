/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwRuntime/BundleElement.hpp"

#include "fwRuntime/Bundle.hpp"

namespace fwRuntime
{

BundleElement::BundleElement() :
    m_bundle( Bundle::getLoadingBundle() ),
    m_enable(true)
{
    // Post-condition
    SLM_ASSERT("bundle '" << m_bundle.lock()->getIdentifier() << "' not initialized", m_bundle.lock() != nullptr );
}

//------------------------------------------------------------------------------

BundleElement::BundleElement( std::shared_ptr< Bundle > bundle ) :
    m_bundle( bundle ),
    m_enable(true)
{
    // Post-condition
    SLM_ASSERT("bundle '" << m_bundle.lock()->getIdentifier() << "' not initialized", m_bundle.lock() != nullptr );
}

//------------------------------------------------------------------------------

std::shared_ptr<Bundle> BundleElement::getBundle() const
{
    return m_bundle.lock();
}

//------------------------------------------------------------------------------

bool BundleElement::isEnable() const
{
    // Pre-condition.
    SLM_ASSERT("bundle '" << m_bundle.lock()->getIdentifier() << "' not initialized", m_bundle.lock() != nullptr );
    return m_bundle.lock()->isEnable() && m_enable;
}

//------------------------------------------------------------------------------

void BundleElement::setEnable(const bool enable)
{
    m_enable = enable;
}

} // namespace fwRuntime
