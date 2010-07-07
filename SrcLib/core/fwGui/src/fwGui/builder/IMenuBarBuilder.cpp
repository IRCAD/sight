/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwGui/builder/IMenuBarBuilder.hpp"

namespace fwGui
{
namespace builder
{

//-----------------------------------------------------------------------------

IMenuBarBuilder::IMenuBarBuilder()
{}

//-----------------------------------------------------------------------------

IMenuBarBuilder::~IMenuBarBuilder()
{}

//-----------------------------------------------------------------------------

void IMenuBarBuilder::initialize( ::fwRuntime::ConfigurationElement::sptr configuration)
{

}

//-----------------------------------------------------------------------------

::fwGui::fwMenuBar::sptr IMenuBarBuilder::getMenuBar()
{
    return this->m_menuBar;
}

//-----------------------------------------------------------------------------

} // namespace builder
} // namespace fwGui



