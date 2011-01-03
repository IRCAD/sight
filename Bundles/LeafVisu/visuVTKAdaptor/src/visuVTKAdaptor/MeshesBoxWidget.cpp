/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include <limits>

#include <boost/foreach.hpp>

#include <fwData/Composite.hpp>
#include <fwData/TriangularMesh.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwServices/ObjectServiceRegistry.hpp>

#include <vtkIO/vtk.hpp>

#include <vtkPolyData.h>
#include <vtkBoxRepresentation.h>
#include <vtkBoxWidget.h>
#include <vtkTransform.h>
#include <vtkCommand.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkProp3DCollection.h>

#include "visuVTKAdaptor/MeshesBoxWidget.hpp"


class MeshesBoxClallback : public ::vtkCommand
{
public:

    static MeshesBoxClallback* New(::visuVTKAdaptor::MeshesBoxWidget* adaptor)
    {
        MeshesBoxClallback *cb = new MeshesBoxClallback;
        cb->m_adaptor = adaptor;
        return cb;
    }

    MeshesBoxClallback() {}
    ~MeshesBoxClallback() {}

    virtual void Execute( ::vtkObject* pCaller, unsigned long, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKAdaptor::MeshesBoxWidget *m_adaptor;
};

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::MeshesBoxWidget, ::fwData::Composite ) ;

namespace visuVTKAdaptor
{

//-----------------------------------------------------------------------------

MeshesBoxWidget::MeshesBoxWidget() throw()
{
    addNewHandledEvent(::fwComEd::CompositeMsg::ADDED_FIELDS);
    addNewHandledEvent(::fwComEd::CompositeMsg::REMOVED_FIELDS);
    addNewHandledEvent(::fwComEd::CompositeMsg::SWAPPED_FIELDS);
    addNewHandledEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED);

    m_boxWidgetCommand = MeshesBoxClallback::New(this);
}

//-----------------------------------------------------------------------------

MeshesBoxWidget::~MeshesBoxWidget() throw()
{
}

//-----------------------------------------------------------------------------

void MeshesBoxWidget::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

//-----------------------------------------------------------------------------

void MeshesBoxWidget::doStart() throw(fwTools::Failed)
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    m_assembly = vtkAssembly::New();

    m_vtkBoxWidget = vtkBoxWidget::New();
    m_vtkBoxWidget->SetInteractor( this->getInteractor() );
    m_vtkBoxWidget->SetPlaceFactor( 1.0 );

    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );

    this->updateMeshMapFromComposite(composite);
    this->doUpdate();
}

//-----------------------------------------------------------------------------

void MeshesBoxWidget::doUpdate() throw(fwTools::Failed)
{
    m_assembly->GetParts()->RemoveAllItems();
    if (!m_meshMap.empty())
    {
        BOOST_FOREACH(MeshMapType::value_type elt, m_meshMap)
        {
            m_assembly->AddPart( elt.second );
        }
        m_vtkBoxWidget->SetProp3D( m_assembly );
        m_vtkBoxWidget->PlaceWidget();
        m_vtkBoxWidget->On();
    }
    else
    {
        m_vtkBoxWidget->Off();
    }
    this->setVtkPipelineModified();
}

//-----------------------------------------------------------------------------

void MeshesBoxWidget::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//-----------------------------------------------------------------------------

void MeshesBoxWidget::doStop() throw(fwTools::Failed)
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    BOOST_FOREACH(::fwData::Composite::value_type elt, *composite)
    {
        ::fwData::TriangularMesh::sptr mesh = ::fwData::TriangularMesh::dynamicCast(elt.second);
        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        fieldTransform = mesh->getFieldSingleElement< ::fwData::TransformationMatrix3D > ("TransformMatrix");
        ::fwServices::unregisterCommunicationChannel(fieldTransform, this->getSptr());
    }
    m_meshMap.clear();

    m_assembly->Delete();
    m_assembly = 0;

    m_vtkBoxWidget->Off();
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    m_vtkBoxWidget->Delete();
    m_vtkBoxWidget = 0;
}

//-----------------------------------------------------------------------------

void MeshesBoxWidget::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast( msg ) ;
    if (compositeMsg)
    {
        if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::REMOVED_FIELDS))
        {
            BOOST_FOREACH(::fwData::Composite::value_type elt, *compositeMsg->getRemovedFields())
            {
                ::fwData::TriangularMesh::sptr mesh = ::fwData::TriangularMesh::dynamicCast(elt.second);
                m_meshMap.erase(elt.first);

                ::fwData::TransformationMatrix3D::sptr fieldTransform;
                fieldTransform = mesh->getFieldSingleElement< ::fwData::TransformationMatrix3D > ("TransformMatrix");
                ::fwServices::unregisterCommunicationChannel(fieldTransform, this->getSptr());
            }
        }
        if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::ADDED_FIELDS))
        {
            this->updateMeshMapFromComposite(compositeMsg->getAddedFields());
        }
        if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::SWAPPED_FIELDS))
        {
            this->updateMeshMapFromComposite(compositeMsg->getSwappedNewFields());
        }
        this->updateMeshTransform();
        this->doUpdate();
    }
    else if (msg->hasEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED))
    {
        this->updateMeshTransform();
        this->doUpdate();
    }
}

//-----------------------------------------------------------------------------

void MeshesBoxWidget::updateFromVtk()
{
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    vtkTransform * boxTransform = vtkTransform::New();
    m_vtkBoxWidget->GetTransform(boxTransform);

    BOOST_FOREACH(::fwData::Composite::value_type elt, *composite)
    {
        ::fwData::TriangularMesh::sptr triangularMesh = ::fwData::TriangularMesh::dynamicCast(elt.second);
        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        SLM_ASSERT("Triangular mesh must have a TransformMatrix field", triangularMesh->getFieldSize("TransformMatrix"))
        fieldTransform = triangularMesh->getFieldSingleElement< ::fwData::TransformationMatrix3D > ("TransformMatrix");

        vtkTransform * transform = vtkTransform::New();
        vtkLinearTransform * meshTransform = m_meshMap[elt.first]->GetUserTransform();
        transform->Concatenate(boxTransform);
        transform->Concatenate(meshTransform);

        vtkMatrix4x4* mat = transform->GetMatrix();

        for(int lt=0; lt<4; lt++)
        {
            for(int ct=0; ct<4; ct++)
            {
                fieldTransform->setCoefficient(lt,ct, mat->GetElement(lt,ct));
            }
        }

        ::fwComEd::TransformationMatrix3DMsg::NewSptr msg;
        msg->addEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED ) ;
        ::fwServices::IEditionService::notify(this->getSptr(), fieldTransform, msg);
    }

    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
}

//-----------------------------------------------------------------------------

void MeshesBoxWidget::updateMeshMapFromComposite(::fwData::Composite::sptr composite)
{
    BOOST_FOREACH(::fwData::Composite::value_type elt, *composite)
    {
        ::fwData::TriangularMesh::sptr mesh = ::fwData::TriangularMesh::dynamicCast(elt.second);
        vtkPolyData * vtkMesh = ::vtkIO::toVTKMesh(mesh);

        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        if (mesh->getFieldSize("TransformMatrix"))
        {
            fieldTransform = mesh->getFieldSingleElement< ::fwData::TransformationMatrix3D > ("TransformMatrix");
        }
        else
        {
            fieldTransform = ::fwData::TransformationMatrix3D::New();
            mesh->setFieldSingleElement("TransformMatrix", fieldTransform);
        }

        vtkTransform * transform = vtkTransform::New();
        transform->Identity();
        for(int lt=0; lt<4; lt++)
        {
            for(int ct=0; ct<4; ct++)
            {
                transform->GetMatrix()->SetElement(lt,ct, fieldTransform->getCoefficient(lt,ct));
            }
        }

        vtkPolyDataMapper *meshMapper = vtkPolyDataMapper::New();
        meshMapper->SetInput(vtkMesh);

        vtkActor *meshActor = vtkActor::New();
        meshActor->SetMapper(meshMapper);
        meshActor->SetUserTransform(transform);

        if (m_meshMap.find(elt.first) == m_meshMap.end())
        {
            ::fwServices::registerCommunicationChannel(fieldTransform, this->getSptr())->start();
        }

        m_meshMap[elt.first] = meshActor;
    }
}

//-----------------------------------------------------------------------------

void MeshesBoxWidget::updateMeshTransform()
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    BOOST_FOREACH(::fwData::Composite::value_type elt, *composite)
    {
        ::fwData::TriangularMesh::sptr mesh = ::fwData::TriangularMesh::dynamicCast(elt.second);

        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        SLM_ASSERT("Triangular mesh must have a TransformMatrix field", mesh->getFieldSize("TransformMatrix"))
        fieldTransform = mesh->getFieldSingleElement< ::fwData::TransformationMatrix3D > ("TransformMatrix");

        vtkTransform * transform = vtkTransform::New();
        transform->Identity();
        for(int lt=0; lt<4; lt++)
        {
            for(int ct=0; ct<4; ct++)
            {
                transform->GetMatrix()->SetElement(lt,ct, fieldTransform->getCoefficient(lt,ct));
            }
        }

        vtkActor *meshActor = m_meshMap[elt.first];
        meshActor->SetUserTransform(transform);
    }
}

//-----------------------------------------------------------------------------

} //namespace visuVTKAdaptor
