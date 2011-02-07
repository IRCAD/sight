/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwGui/dialog/ProgressDialog.hpp"

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

ProgressDialog::ProgressDialog(const std::string &title,const std::string &message)
{
    m_implementation = ::fwTools::ClassFactoryRegistry::create< ::fwGui::dialog::IProgressDialog>( ::fwGui::dialog::IProgressDialog::REGISTRY_KEY);
    m_implementation->setTitle(title);
    m_implementation->setMessage(message);
}

//-----------------------------------------------------------------------------

void ProgressDialog::setTitle(const std::string &title)
{
    m_implementation->setTitle(title);
}

//-----------------------------------------------------------------------------

void ProgressDialog::setMessage(const std::string &msg)
{
    m_implementation->setMessage(msg);
}

//-----------------------------------------------------------------------------

void ProgressDialog::operator()(float percent,std::string msg)
{
    (*m_implementation)(percent,msg);
}

} //namespace dialog
} //namespace fwGui
