/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/SMeshesBoxWidget.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Mesh.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/helper/Mesh.hpp>
#include <fwVtkIO/vtk.hpp>

#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkBoxRepresentation.h>
#include <vtkBoxWidget2.h>
#include <vtkCommand.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProp3DCollection.h>
#include <vtkTransform.h>

#include <limits>

class MeshesBoxClallback : public ::vtkCommand
{
public:

    //------------------------------------------------------------------------------

    static MeshesBoxClallback* New(::visuVTKAdaptor::SMeshesBoxWidget* adaptor)
    {
        MeshesBoxClallback* cb = new MeshesBoxClallback;
        cb->m_adaptor = adaptor;
        return cb;
    }

    MeshesBoxClallback() :
        m_adaptor(NULL)
    {
    }
    ~MeshesBoxClallback()
    {
    }

    //------------------------------------------------------------------------------

    virtual void Execute( ::vtkObject* pCaller, unsigned long, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKAdaptor::SMeshesBoxWidget* m_adaptor;
};

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SMeshesBoxWidget);

static const ::fwServices::IService::KeyType s_COMPOSITE_INPUT = "composite";

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_MATRICES_SLOT = "updateMatrices";
static const ::fwCom::Slots::SlotKeyType s_ADD_OBJECTS_SLOT     = "addObject";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_OBJECTS_SLOT  = "changeObject";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_OBJECTS_SLOT  = "removeObjects";

//-----------------------------------------------------------------------------

SMeshesBoxWidget::SMeshesBoxWidget() noexcept :
    m_assembly(nullptr),
    m_boxWidgetCommand(MeshesBoxClallback::New(this)),
    m_vtkBoxWidget(nullptr)
{
    newSlot(s_UPDATE_MATRICES_SLOT, &SMeshesBoxWidget::updateMatrices, this);
    newSlot(s_ADD_OBJECTS_SLOT, &SMeshesBoxWidget::addObjects, this);
    newSlot(s_CHANGE_OBJECTS_SLOT, &SMeshesBoxWidget::changeObjects, this);
    newSlot(s_REMOVE_OBJECTS_SLOT, &SMeshesBoxWidget::removeObjects, this);
}

//-----------------------------------------------------------------------------

SMeshesBoxWidget::~SMeshesBoxWidget() noexcept
{
}

//-----------------------------------------------------------------------------

void SMeshesBoxWidget::configuring()
{
    this->configureParams();
}

//-----------------------------------------------------------------------------

void SMeshesBoxWidget::starting()
{
    this->initialize();

    ::fwData::Composite::csptr composite = this->getInput< ::fwData::Composite >(s_COMPOSITE_INPUT);
    SLM_ASSERT("Missing composite", composite);

    m_assembly = vtkAssembly::New();

    vtkBoxRepresentation* boxRep = vtkBoxRepresentation::New();
    boxRep->SetPlaceFactor(1.0);

    m_vtkBoxWidget = vtkBoxWidget2::New();
    m_vtkBoxWidget->SetInteractor( this->getInteractor() );
    m_vtkBoxWidget->SetRepresentation( boxRep );

    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );

    this->updateMeshMapFromComposite(composite->getContainer());
    this->updating();
}

//-----------------------------------------------------------------------------

void SMeshesBoxWidget::updating()
{
    m_assembly->GetParts()->RemoveAllItems();
    if (!m_meshMap.empty())
    {
        for(MeshMapType::value_type elt :  m_meshMap)
        {
            m_assembly->AddPart( elt.second );
        }
        vtkBoxRepresentation* boxRep = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
        boxRep->PlaceWidget(m_assembly->GetBounds());
        m_vtkBoxWidget->On();
    }
    else
    {
        m_vtkBoxWidget->Off();
    }
    this->setVtkPipelineModified();
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SMeshesBoxWidget::stopping()
{
    ::fwData::Composite::csptr composite = this->getInput< ::fwData::Composite >(s_COMPOSITE_INPUT);
    SLM_ASSERT("Missing composite", composite);

    for(::fwData::Composite::value_type elt :  *composite)
    {
        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast(elt.second);
        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        fieldTransform = mesh->getField< ::fwData::TransformationMatrix3D > ("TransformMatrix");
        m_connections[elt.first].disconnect();
        m_connections.erase(elt.first);
    }
    m_meshMap.clear();

    m_assembly->Delete();
    m_assembly = nullptr;

    m_vtkBoxWidget->Off();
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    m_vtkBoxWidget->Delete();
    m_vtkBoxWidget = nullptr;
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SMeshesBoxWidget::updateFromVtk()
{
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );

    ::fwData::Composite::csptr composite = this->getInput< ::fwData::Composite >(s_COMPOSITE_INPUT);
    SLM_ASSERT("Missing composite", composite);

    vtkBoxRepresentation* boxRep = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
    vtkTransform* boxTransform   = vtkTransform::New();
    boxRep->GetTransform(boxTransform);

    for(const ::fwData::Composite::value_type& elt :  composite->getContainer())
    {
        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast(elt.second);
        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        SLM_ASSERT("Mesh must have a TransformMatrix field", mesh->getField("TransformMatrix"));
        fieldTransform = mesh->getField< ::fwData::TransformationMatrix3D > ("TransformMatrix");

        vtkTransform* transform           = vtkTransform::New();
        vtkLinearTransform* meshTransform = m_meshMap[elt.first]->GetUserTransform();
        transform->Concatenate(boxTransform);
        transform->Concatenate(meshTransform);

        vtkMatrix4x4* mat = transform->GetMatrix();
        for(int lt = 0; lt < 4; lt++)
        {
            for(int ct = 0; ct < 4; ct++)
            {
                fieldTransform->setCoefficient(static_cast<size_t>(lt), static_cast<size_t>(ct),
                                               mat->GetElement(lt, ct));
            }
        }

        auto sig = fieldTransform->signal< ::fwData::Object::ModifiedSignalType >(
            ::fwData::Object::s_MODIFIED_SIG);
        sig->asyncEmit();

        transform->Delete();
    }
    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
    boxTransform->Delete();
}

//-----------------------------------------------------------------------------

void SMeshesBoxWidget::updateMeshMapFromComposite(::fwData::Composite::ContainerType objects)
{
    for(const ::fwData::Composite::value_type& elt :  objects)
    {
        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast(elt.second);
        SLM_ASSERT("Composite must only contain meshes", mesh);

        vtkSmartPointer<vtkPolyData> vtkMesh = vtkSmartPointer<vtkPolyData>::New();
        ::fwVtkIO::helper::Mesh::toVTKMesh( mesh, vtkMesh);

        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        fieldTransform = mesh->setDefaultField("TransformMatrix", ::fwData::TransformationMatrix3D::New());

        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->Identity();

        vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();

        for(int lt = 0; lt < 4; lt++)
        {
            for(int ct = 0; ct < 4; ct++)
            {
                matrix->SetElement(lt, ct,
                                   fieldTransform->getCoefficient(static_cast<size_t>(lt), static_cast<size_t>(ct)));
            }
        }

        transform->SetMatrix(matrix);
        vtkSmartPointer<vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        meshMapper->SetInputData(vtkMesh);

        vtkActor* meshActor = vtkActor::New();
        meshActor->SetMapper(meshMapper);
        meshActor->SetUserTransform(transform);

        if (m_meshMap.find(elt.first) == m_meshMap.end())
        {
            ::fwCom::Connection connection = fieldTransform->signal(::fwData::Object::s_MODIFIED_SIG)->
                                             connect(this->slot(s_UPDATE_MATRICES_SLOT));
            m_connections[elt.first] = connection;
        }

        m_meshMap[elt.first] = meshActor;
    }
}

//-----------------------------------------------------------------------------

void SMeshesBoxWidget::updateMeshTransform()
{
    ::fwData::Composite::csptr composite = this->getInput< ::fwData::Composite >(s_COMPOSITE_INPUT);
    SLM_ASSERT("Missing composite", composite);

    for(const ::fwData::Composite::value_type& elt :  composite->getContainer())
    {
        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast(elt.second);

        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        SLM_ASSERT("Triangular mesh must have a TransformMatrix field", mesh->getField("TransformMatrix"));
        fieldTransform = mesh->getField< ::fwData::TransformationMatrix3D > ("TransformMatrix");

        vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
        transform->Identity();

        vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();

        for(int lt = 0; lt < 4; lt++)
        {
            for(int ct = 0; ct < 4; ct++)
            {
                matrix->SetElement(lt, ct,
                                   fieldTransform->getCoefficient(static_cast<size_t>(lt), static_cast<size_t>(ct)));
            }
        }

        transform->SetMatrix(matrix);
        vtkActor* meshActor = m_meshMap[elt.first];
        meshActor->SetUserTransform(transform);
    }
}

//-----------------------------------------------------------------------------

void SMeshesBoxWidget::updateMatrices()
{
    this->updateMeshTransform();
    this->updating();
}

//------------------------------------------------------------------------------

void SMeshesBoxWidget::addObjects(::fwData::Composite::ContainerType objects)
{
    this->updateMeshMapFromComposite(objects);
    this->updateMeshTransform();
    this->updating();
}

//------------------------------------------------------------------------------

void SMeshesBoxWidget::changeObjects(::fwData::Composite::ContainerType newObjects,
                                     ::fwData::Composite::ContainerType /*oldObjects*/)
{
    this->updateMeshMapFromComposite(newObjects);
    this->updateMeshTransform();
    this->updating();
}

//------------------------------------------------------------------------------

void SMeshesBoxWidget::removeObjects(::fwData::Composite::ContainerType objects)
{
    for(const ::fwData::Composite::value_type& elt :  objects)
    {
        ::fwData::Mesh::sptr mesh = ::fwData::Mesh::dynamicCast(elt.second);
        m_meshMap[elt.first]->Delete();
        m_meshMap.erase(elt.first);

        ::fwData::TransformationMatrix3D::sptr fieldTransform;
        fieldTransform = mesh->getField< ::fwData::TransformationMatrix3D > ("TransformMatrix");
        m_connections[elt.first].disconnect();
        m_connections.erase(elt.first);
    }

    this->updateMeshTransform();
    this->updating();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMeshesBoxWidget::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_COMPOSITE_INPUT, ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_ADD_OBJECTS_SLOT);
    connections.push(s_COMPOSITE_INPUT, ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_CHANGE_OBJECTS_SLOT);
    connections.push(s_COMPOSITE_INPUT, ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_REMOVE_OBJECTS_SLOT);

    return connections;
}

//-----------------------------------------------------------------------------

} //namespace visuVTKAdaptor

#endif // ANDROID
