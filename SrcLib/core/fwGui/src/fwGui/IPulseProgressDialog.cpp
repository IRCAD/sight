/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/IPulseProgressDialog.hpp"

namespace fwGui
{

const IPulseProgressDialog::FactoryRegistryKeyType IPulseProgressDialog::REGISTRY_KEY =  "::fwGui::PulseProgressDialog";

IPulseProgressDialog::IPulseProgressDialog()
{}


IPulseProgressDialog::~IPulseProgressDialog()
{}

void IPulseProgressDialog::setStuff(Stuff stuff)
{
    m_stuff = stuff;
}

void IPulseProgressDialog::setFrequence(MilliSecond frequence)
{
    m_frequence = frequence;
}

} // namespace fwGui



