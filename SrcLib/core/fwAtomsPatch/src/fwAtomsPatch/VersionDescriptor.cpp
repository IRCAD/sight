/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtomsPatch/VersionDescriptor.hpp"

namespace fwAtomsPatch
{

VersionDescriptor::VersionDescriptor()
: m_context(""), m_versionName("")
{
}

// ----------------------------------------------------------------------------

VersionDescriptor::VersionDescriptor(
        const std::string& context, const std::string& versionName, const VersionsType& versions)
: m_context(context), m_versionName(versionName), m_versions(versions)
{
}

// ----------------------------------------------------------------------------

VersionDescriptor::~VersionDescriptor()
{
}

} //namespace fwAtomsPatch
