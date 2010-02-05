/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/PlaneListMsg.hpp>
#include <fwComEd/PlaneMsg.hpp>

#include <fwServices/Factory.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/macros.hpp>

#include <fwTools/Object.hpp>
#include <fwData/None.hpp>

#include "visuVTKAdaptor/PlaneSelector.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PlaneSelector, ::fwTools::Object ) ;

namespace visuVTKAdaptor
{

PlaneSelector::PlaneSelector() throw()
{
    handlingEventOff();
}

PlaneSelector::~PlaneSelector() throw()
{

}

void PlaneSelector::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    assert(m_configuration->getName() == "config");
    //assert(m_configuration->hasAttribute("planelist"));
    //assert(m_configuration->hasAttribute("planeselection"));

    //this->setPlaneListId( m_configuration->getAttributeValue("planelist") );
    //this->setPlaneSelectionId( m_configuration->getAttributeValue("planeselection") );

}

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

void PlaneSelector::doUpdate() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    SLM_ASSERT("NOT IMPLEMENTED",false);
}

void PlaneSelector::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Object::sptr object = ::fwData::Object::dynamicCast(this->getObject());
    this->selectObject(object);

}

void PlaneSelector::doStop() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->selectObject( ::fwData::None::New() );
}


void PlaneSelector::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

void PlaneSelector::selectObject( ::fwData::Object::sptr object )
{
    SLM_TRACE_FUNC();

    ::fwData::Object::sptr oldObject;

    if ( ! m_currentObject.expired() )
    {
        oldObject = m_currentObject.lock() ;
    }

    if (oldObject != object)
    {
        if (oldObject && !::fwData::None::isNone(oldObject) )
        {
            ::fwComEd::PlaneMsg::NewSptr deselectMsg;
            deselectMsg->addEvent( ::fwComEd::PlaneMsg::WAS_DESELECTED );
            ::fwServices::IEditionService::notify( this->getSptr(), oldObject, deselectMsg); //TODO: remove option
        }

        if (object)
        {
            if (!::fwData::None::isNone(object) )
            {
                ::fwComEd::PlaneMsg::NewSptr selectMsg;
                selectMsg->addEvent( ::fwComEd::PlaneMsg::WAS_SELECTED );
                ::fwServices::IEditionService::notify( this->getSptr(), object, selectMsg); //TODO: remove option
            }
            m_currentObject = object;
        }
    }
}





} //namespace visuVTKAdaptor


