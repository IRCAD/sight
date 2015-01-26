/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/IApplication.hpp"

namespace fwGui
{

const IApplication::FactoryRegistryKeyType IApplication::REGISTRY_KEY =  "::fwGui::Application";

//-----------------------------------------------------------------------------

IApplication::IApplication() : m_confirm(false)
{}

//-----------------------------------------------------------------------------

IApplication::~IApplication()
{}

//-----------------------------------------------------------------------------

} // namespace fwGui



