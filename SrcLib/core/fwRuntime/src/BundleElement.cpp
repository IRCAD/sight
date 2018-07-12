/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
