/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
