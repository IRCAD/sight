/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/PlaneSelector.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PlaneSelector, ::fwData::Object );

namespace visuVTKAdaptor
{

PlaneSelector::PlaneSelector() throw()
{
}

//------------------------------------------------------------------------------

PlaneSelector::~PlaneSelector() throw()
{
}

//------------------------------------------------------------------------------

void PlaneSelector::doConfigure() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    assert(m_configuration->getName() == "config");
}

//------------------------------------------------------------------------------

void PlaneSelector::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Object::sptr object = ::fwData::Object::dynamicCast(this->getObject());

    if (object)
    {
        m_currentObject = object;
    }
    this->selectObject(object);
}

//------------------------------------------------------------------------------

void PlaneSelector::doUpdate() throw(fwTools::Failed)
{
    SLM_ASSERT("NOT IMPLEMENTED", false);
}

//------------------------------------------------------------------------------

void PlaneSelector::doSwap() throw(fwTools::Failed)
{
    ::fwData::Object::sptr object = ::fwData::Object::dynamicCast(this->getObject());
    this->selectObject(object);
}

//------------------------------------------------------------------------------

void PlaneSelector::doStop() throw(fwTools::Failed)
{
    this->selectObject( ::fwData::Object::sptr() );
}

//------------------------------------------------------------------------------

void PlaneSelector::selectObject( ::fwData::Object::sptr object )
{
    ::fwData::Object::sptr oldObject = m_currentObject.lock();

    if (oldObject != object)
    {
        if (oldObject)
        {
            auto sig = oldObject->signal< ::fwData::Plane::SelectedSignalType >(
                ::fwData::Plane::s_SELECTED_SIG);
            sig->asyncEmit(false);
        }

        m_currentObject.reset();

        if (object)
        {
            auto sig = object->signal< ::fwData::Plane::SelectedSignalType >(
                ::fwData::Plane::s_SELECTED_SIG);
            sig->asyncEmit(true);
            m_currentObject = object;
        }
    }
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

#endif //ANDROID

