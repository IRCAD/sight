/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include "fwWX/convert.hpp"
#include "fwWX/widgets/fwProgressDialog.hpp"
#include "fwWX/ProgressDialog.hpp"


REGISTER_BINDING( ::fwGui::IProgressDialog, ::fwWX::ProgressDialog, ::fwGui::IProgressDialog::FactoryRegistryKeyType , ::fwGui::IProgressDialog::REGISTRY_KEY );


namespace fwWX
{

//------------------------------------------------------------------------------

ProgressDialog::ProgressDialog( const std::string title, const std::string message)
{
    fwProgressDialog *wxpd = new fwProgressDialog(
                                    ::fwWX::std2wx(title),
                                    ::fwWX::std2wx(message),
                                    100 /*percent*/,
                                    NULL, wxPD_AUTO_HIDE | wxPD_APP_MODAL //| wxPD_REMAINING_TIME
                            );
    m_pdialog = ::boost::shared_ptr<fwProgressDialog>(wxpd);
    m_pdialog->Show();
    m_pdialog->Update();
}

//------------------------------------------------------------------------------

ProgressDialog::~ProgressDialog()
{
    // auto clean dialog
}

//------------------------------------------------------------------------------

FWWX_API void ProgressDialog::operator()(float percent,std::string msg)
{
    assert(m_pdialog);
    int value = (int)(percent*100);
    OSLM_TRACE( "ProgressDialog msg" << msg << " : " << value <<"%");
    m_pdialog->Show(true); // can be hidden if previous load as finished
    m_pdialog->Update(value, ::fwWX::std2wx(msg) );
}

//------------------------------------------------------------------------------

void ProgressDialog::setTitle(const std::string &title)
{
    m_pdialog->SetTitle(::fwWX::std2wx(title));
}

//------------------------------------------------------------------------------

void ProgressDialog::setMessage(const std::string &msg)
{
    m_pdialog->UpdateMessage( ::fwWX::std2wx(msg) );
}



}
