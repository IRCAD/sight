/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/PulseProgressDialog.hpp"

namespace fwGui
{

//-----------------------------------------------------------------------------

PulseProgressDialog::PulseProgressDialog( const std::string &title, ::fwGui::IPulseProgressDialog::Stuff stuff, const std::string &msg, ::fwGui::IPulseProgressDialog::MilliSecond frequenceRefresh )
{
    m_implementation = ::fwTools::ClassFactoryRegistry::create< ::fwGui::IPulseProgressDialog>( ::fwGui::IPulseProgressDialog::REGISTRY_KEY);
    m_implementation->setTitle(title);
    m_implementation->setMessage(msg);
    m_implementation->setStuff(stuff);
    m_implementation->setFrequence(frequenceRefresh);
}

//-----------------------------------------------------------------------------

void PulseProgressDialog::setTitle(const std::string &title)
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

void PulseProgressDialog::setMessage(const std::string &msg)
{
    m_implementation->setMessage(msg);
}

//-----------------------------------------------------------------------------

void PulseProgressDialog::show()
{
    m_implementation->show();
}

//-----------------------------------------------------------------------------


} //namespace fwGui
