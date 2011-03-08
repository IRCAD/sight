/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/ResectionDB.hpp>
#include <fwData/Resection.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwComEd/ResectionDBMsg.hpp>

#include "visuVTKAdaptor/Resection.hpp"
#include "visuVTKAdaptor/ResectionDB.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ResectionDB, ::fwData::ResectionDB ) ;

namespace visuVTKAdaptor
{


//------------------------------------------------------------------------------

ResectionDB::ResectionDB() throw()
{
    m_clippingPlanes = "";
    m_sharpEdgeAngle = 50;
    addNewHandledEvent( ::fwComEd::ResectionDBMsg::ADD_SAFE_PART );
    addNewHandledEvent( ::fwComEd::ResectionDBMsg::ADD_RESECTION );
    addNewHandledEvent( ::fwComEd::ResectionDBMsg::MODIFIED );
}

//------------------------------------------------------------------------------

ResectionDB::~ResectionDB() throw()
{

}

//------------------------------------------------------------------------------

void ResectionDB::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );

    this->setClippingPlanes( m_configuration->getAttributeValue("clippingplanes") );

    std::string sSharpEdgeAngle(m_configuration->getAttributeValue("sharpangle"));
    if ( !sSharpEdgeAngle.empty() )
    {
        double sharpEdgeAngle = ::boost::lexical_cast< double > (sSharpEdgeAngle);
        this->setSharpEdgeAngle( sharpEdgeAngle );
    }

    if(m_configuration->hasAttribute("transform") )
    {
        this->setTransformId( m_configuration->getAttributeValue("transform") );
    }

}

//------------------------------------------------------------------------------

void ResectionDB::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ResectionDB::doUpdate() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    doStop();

    ::fwData::ResectionDB::sptr resecDB = this->getObject< ::fwData::ResectionDB >();

    ::fwData::ResectionDB::ResectionIterator iter = resecDB->getResections().first;
    for ( ; iter != resecDB->getResections().second ; ++iter)
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr service =
                ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > ( *iter, "::visuVTKAdaptor::Resection" );
        SLM_ASSERT("service not instanced", service);

        service->setTransformId( this->getTransformId() );
        service->setRenderId( this->getRenderId() );
        service->setPickerId( this->getPickerId() );
        service->setRenderService(this->getRenderService());
        ::visuVTKAdaptor::Resection::dynamicCast(service)->setClippingPlanes( m_clippingPlanes );
        ::visuVTKAdaptor::Resection::dynamicCast(service)->setSharpEdgeAngle( m_sharpEdgeAngle );
        service->start();

        this->registerService(service);
    }

    // Safe part
    ::fwData::Resection::sptr resec = resecDB->getSafeResection();
    if(resec)
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr service =
                ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > ( resec, "::visuVTKAdaptor::Resection" );
        SLM_ASSERT("service not instanced", service);

        service->setTransformId( this->getTransformId() );
        service->setRenderId( this->getRenderId() );
        service->setPickerId( this->getPickerId() );
        service->setRenderService(this->getRenderService());
        service->start();

        this->registerService(service);
    }
}

//------------------------------------------------------------------------------

void ResectionDB::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ResectionDB::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void ResectionDB::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    ::fwComEd::ResectionDBMsg::csptr pResectionDBMsg = ::fwComEd::ResectionDBMsg::dynamicConstCast( msg ) ;
    if ( pResectionDBMsg ) 
    {
        if ( pResectionDBMsg->hasEvent(::fwComEd::ResectionDBMsg::ADD_SAFE_PART) ||
                pResectionDBMsg->hasEvent(::fwComEd::ResectionDBMsg::ADD_RESECTION) )
        {
            this->doUpdate();
        }
        if ( pResectionDBMsg->hasEvent(::fwComEd::ResectionDBMsg::MODIFIED) )
        {
            this->doUpdate();
        }
    }
}




} //namespace visuVTKAdaptor
