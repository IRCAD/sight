/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "fwWX/IGuiContainer.hpp"

namespace fwWX
{

//-----------------------------------------------------------------------------

wxContainer::wxContainer() throw() : m_container(0)
{}

//-----------------------------------------------------------------------------

wxContainer::~wxContainer() throw()
{}

//-----------------------------------------------------------------------------

void wxContainer::clean()
{
    SLM_ASSERT("Sorry, wxWindows not yet initialized, cleaning impossible", m_container);

    m_container->SetSizer(NULL);
    m_container->DestroyChildren();
}

//-----------------------------------------------------------------------------

void wxContainer::destroyContainer()
{
    SLM_ASSERT("Sorry, wxWindows not yet initialized", m_container);

    m_container->Destroy();
    m_container = 0;
}

//-----------------------------------------------------------------------------

FWWX_API virtual void setWxContainer(wxWindows* container)
{
    this->m_container = container;
}

//-----------------------------------------------------------------------------

FWWX_API virtual wxWindows* getWxContainer()
{
    return this->m_container;
}

//-----------------------------------------------------------------------------

}
