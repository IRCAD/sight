/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/Model.hpp>
#include <fwData/Material.hpp>

#include <fwComEd/ModelMsg.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include "visuVTKAdaptor/Model.hpp"
#include "visuVTKAdaptor/TriangularMesh.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Model, ::fwData::Model ) ;

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

Model::Model() throw()
{
     addNewHandledEvent( ::fwComEd::ModelMsg::NEW_MODEL );
}

//------------------------------------------------------------------------------

Model::~Model() throw()
{

}

//------------------------------------------------------------------------------

void Model::configuring() throw(fwTools::Failed)
{

    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    if(m_configuration->hasAttribute("transform") )
    {
        this->setTransformId( m_configuration->getAttributeValue("transform") );
    }
}

//------------------------------------------------------------------------------

void Model::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Model::doUpdate() throw(fwTools::Failed)
{
    doStop();

    ::fwData::Model::sptr model = this->getObject< ::fwData::Model >();
    ::fwData::Model::Container::iterator iter;
    ::fwData::Model::Container mapModel = model->getCRefMap();

    for (iter = mapModel.begin(); iter != mapModel.end(); ++iter)
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr service =
            ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >
                 ( iter->first, "::visuVTKAdaptor::TriangularMesh" );
        SLM_ASSERT("AdaptorService", service);

        ::visuVTKAdaptor::TriangularMesh::sptr trianAdaptor =
                ::visuVTKAdaptor::TriangularMesh::dynamicCast(service);
        SLM_ASSERT("dynamicCast TriangularMesh fails", trianAdaptor);

        trianAdaptor->setMaterial(iter->second);
        trianAdaptor->setTransformId( this->getTransformId() );
        trianAdaptor->setRenderId( this->getRenderId() );
        trianAdaptor->setPickerId( this->getPickerId() );
        trianAdaptor->setRenderService(this->getRenderService());
        trianAdaptor->start();

        this->registerService(trianAdaptor);
    }
}

//------------------------------------------------------------------------------

void Model::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Model::doStop() throw(fwTools::Failed)
{
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void Model::doUpdate( ::fwServices::ObjectMsg::csptr _msg) throw(fwTools::Failed)
{
    ::fwComEd::ModelMsg::csptr msg = ::fwComEd::ModelMsg::dynamicConstCast(_msg);

    if ( msg && msg->hasEvent( ::fwComEd::ModelMsg::NEW_MODEL ) )
    {
        doUpdate();
    }
}



} //namespace visuVTKAdaptor
