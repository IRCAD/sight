/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Resection.hpp"
#include "visuVTKAdaptor/ResectionDB.hpp"

#include <fwData/ResectionDB.hpp>
#include <fwData/Resection.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>


fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::ResectionDB, ::fwData::ResectionDB );

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

ResectionDB::ResectionDB() noexcept : m_sharpEdgeAngle(50.)
{
}

//------------------------------------------------------------------------------

ResectionDB::~ResectionDB() noexcept
{
}

//------------------------------------------------------------------------------

void ResectionDB::doConfigure()
{
    assert(m_configuration->getName() == "config");

    this->setClippingPlanes( m_configuration->getAttributeValue("clippingplanes") );

    std::string sSharpEdgeAngle(m_configuration->getAttributeValue("sharpangle"));
    if ( !sSharpEdgeAngle.empty() )
    {
        double sharpEdgeAngle = ::boost::lexical_cast< double > (sSharpEdgeAngle);
        this->setSharpEdgeAngle( sharpEdgeAngle );
    }
}

//------------------------------------------------------------------------------

void ResectionDB::doStart()
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ResectionDB::doUpdate()
{
    SLM_TRACE_FUNC();
    doStop();

    ::fwData::ResectionDB::sptr resecDB = this->getObject< ::fwData::ResectionDB >();

    for( ::fwData::Resection::sptr resection :  resecDB->getResections() )
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr service =
            ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > ( resection, "::visuVTKAdaptor::Resection" );
        SLM_ASSERT("service not instanced", service);

        service->setTransformId( this->getTransformId() );
        service->setRenderId( this->getRenderId() );
        service->setPickerId( this->getPickerId() );
        service->setRenderService(this->getRenderService());
        service->setAutoRender( this->getAutoRender() );
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
        service->setAutoRender( this->getAutoRender() );
        service->start();

        this->registerService(service);
    }
}

//------------------------------------------------------------------------------

void ResectionDB::doSwap()
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void ResectionDB::doStop()
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType ResectionDB::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::ResectionDB::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::ResectionDB::s_SAFE_PART_ADDED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::ResectionDB::s_RESECTION_ADDED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
