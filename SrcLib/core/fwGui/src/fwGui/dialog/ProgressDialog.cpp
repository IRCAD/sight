/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/ActiveWorkers.hpp>
#include "fwGui/dialog/ProgressDialog.hpp"

#include <boost/function.hpp>

namespace fwGui
{
namespace dialog
{
//-----------------------------------------------------------------------------

ProgressDialog::ProgressDialog(const std::string &title,const std::string &message)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(IProgressDialog::REGISTRY_KEY);
                m_implementation = ::fwGui::dialog::IProgressDialog::dynamicCast(guiObj);
                if(m_implementation)
                {
                    m_implementation->setTitle(title);
                    m_implementation->setMessage(message);
                }
            } ).wait();
}

//-----------------------------------------------------------------------------

ProgressDialog::~ProgressDialog()
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>( [&] { m_implementation.reset(); } ).wait();
}

//-----------------------------------------------------------------------------

void ProgressDialog::setTitle(const std::string &title)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                if(m_implementation)
                {
                    m_implementation->setTitle(title);
                }
            } ).wait();
}

//-----------------------------------------------------------------------------

void ProgressDialog::setMessage(const std::string &msg)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                if(m_implementation)
                {
                    m_implementation->setMessage(msg);
                }
            } ).wait();
}

//-----------------------------------------------------------------------------

void ProgressDialog::operator()(float percent,std::string msg)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                if(m_implementation)
                {
                    (*m_implementation)(percent,msg);
                }
            } ).wait();
}


//-----------------------------------------------------------------------------

void ProgressDialog::setCancelCallback(CancelCallbackType callback)
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                if(m_implementation)
                {
                    m_implementation->setCancelCallback(callback);
                }
            } ).wait();
}

//-----------------------------------------------------------------------------

void ProgressDialog::cancelPressed()
{
    assert(0);
}

//-----------------------------------------------------------------------------

void ProgressDialog::hideCancelButton()
{
    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(
        [&] {
                m_implementation->hideCancelButton();
            } ).wait();
}

} //namespace dialog
} //namespace fwGui
