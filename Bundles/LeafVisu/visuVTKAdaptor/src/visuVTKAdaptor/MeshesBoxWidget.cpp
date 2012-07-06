/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#include <limits>

#include <boost/foreach.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Mesh.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/TransformationMatrix3DMsg.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwServices/registry/ObjectService.hpp>

#include <vtkIO/vtk.hpp>
#include <vtkIO/helper/Mesh.hpp>

#include <vtkPolyData.h>
#include <vtkBoxRepresentation.h>
#include <vtkBoxWidget2.h>
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
    addNewHandledEvent(::fwComEd::CompositeMsg::ADDED_KEYS);
    addNewHandledEvent(::fwComEd::CompositeMsg::REMOVED_KEYS);
    addNewHandledEvent(::fwComEd::CompositeMsg::CHANGED_KEYS);
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

    vtkBoxRepresentation *boxRep = vtkBoxRepresentation::New();
    boxRep->SetPlaceFactor(1.0);

    m_vtkBoxWidget = vtkBoxWidget2::New();
    m_vtkBoxWidget->SetInteractor( this->getInteractor() );
    m_vtkBoxWidget->SetRepresentation( boxRep );

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
        vtkBoxRepresentation *boxRep = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
        boxRep->PlaceWidget(m_assembly->GetBounds());
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
        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast(elt.second);
        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        fieldTransform = mesh->getField< ::fwData::TransformationMatrix3D > ("TransformMatrix");
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
        if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::REMOVED_KEYS))
        {
            BOOST_FOREACH(::fwData::Composite::value_type elt, *compositeMsg->getRemovedKeys())
            {
                ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast(elt.second);
                m_meshMap[elt.first]->Delete();
                m_meshMap.erase(elt.first);

                ::fwData::TransformationMatrix3D::sptr fieldTransform;
                fieldTransform = mesh->getField< ::fwData::TransformationMatrix3D > ("TransformMatrix");
                ::fwServices::unregisterCommunicationChannel(fieldTransform, this->getSptr());
            }
        }
        if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::ADDED_KEYS))
        {
            this->updateMeshMapFromComposite(compositeMsg->getAddedKeys());
        }
        if (compositeMsg->hasEvent(::fwComEd::CompositeMsg::CHANGED_KEYS))
        {
            this->updateMeshMapFromComposite(compositeMsg->getNewChangedKeys());
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

    vtkBoxRepresentation *boxRep = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
    vtkTransform * boxTransform = vtkTransform::New();
    boxRep->GetTransform(boxTransform);

    BOOST_FOREACH(::fwData::Composite::value_type elt, *composite)
    {
        ::fwData::Mesh::sptr triangularMesh = ::fwData::Mesh::dynamicCast(elt.second);
        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        SLM_ASSERT("Triangular mesh must have a TransformMatrix field", triangularMesh->getField("TransformMatrix"))
        fieldTransform = triangularMesh->getField< ::fwData::TransformationMatrix3D > ("TransformMatrix");

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
        transform->Delete();
    }
    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
    boxTransform->Delete();
}

//-----------------------------------------------------------------------------

void MeshesBoxWidget::updateMeshMapFromComposite(::fwData::Composite::sptr composite)
{
    BOOST_FOREACH(::fwData::Composite::value_type elt, *composite)
    {
        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast(elt.second);
        vtkSmartPointer<vtkPolyData> vtkMesh = vtkSmartPointer<vtkPolyData>::New();
        ::vtkIO::helper::Mesh::toVTKMesh( mesh, vtkMesh);

        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        fieldTransform = mesh->setDefaultField("TransformMatrix", ::fwData::TransformationMatrix3D::New());

        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->Identity();
        for(int lt=0; lt<4; lt++)
        {
            for(int ct=0; ct<4; ct++)
            {
                transform->GetMatrix()->SetElement(lt,ct, fieldTransform->getCoefficient(lt,ct));
            }
        }

        vtkSmartPointer<vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
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
        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast(elt.second);

        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        SLM_ASSERT("Triangular mesh must have a TransformMatrix field", mesh->getField("TransformMatrix"))
        fieldTransform = mesh->getField< ::fwData::TransformationMatrix3D > ("TransformMatrix");

        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
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
