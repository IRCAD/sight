/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SLandmarks.hpp"

#include "visuVTKAdaptor/PointLabel.hpp"
#include "visuVTKAdaptor/PointList.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Image.hpp>
#include <fwData/Landmarks.hpp>
#include <fwData/Material.hpp>
#include <fwData/Point.hpp>
#include <fwData/PointList.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwRenderVTK/vtk/fwHandleRepresentation3D.hpp>
#include <fwRenderVTK/vtk/Helpers.hpp>
#include <fwRenderVTK/vtk/MarkedSphereHandleRepresentation.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <vtkActor.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTextActor.h>
#include <vtkTextMapper.h>
#include <vtkTextProperty.h>

#include <algorithm>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::SLandmarks, ::fwData::Landmarks );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_ADD_POINT_SLOT      = "addPoint";
static const ::fwCom::Slots::SlotKeyType s_INSERT_POINT_SLOT   = "insertPoint";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_POINT_SLOT   = "removePoint";
static const ::fwCom::Slots::SlotKeyType s_ADD_GROUP_SLOT      = "addGroup";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_GROUP_SLOT   = "removeGroup";
static const ::fwCom::Slots::SlotKeyType s_MODIFY_GROUP_SLOT   = "modifyGroup";
static const ::fwCom::Slots::SlotKeyType s_MODIFY_POINT_SLOT   = "modifyPoint";
static const ::fwCom::Slots::SlotKeyType s_RENAME_GROUP_SLOT   = "renameGroup";
static const ::fwCom::Slots::SlotKeyType s_SELECT_POINT_SLOT   = "selectPoint";
static const ::fwCom::Slots::SlotKeyType s_DESELECT_POINT_SLOT = "deselectPoint";
static const ::fwCom::Slots::SlotKeyType s_SHOW_SLOT           = "show";

//------------------------------------------------------------------------------

class vtkLandmarkUpdateCallBack : public vtkCommand
{

public:

//------------------------------------------------------------------------------

    static vtkLandmarkUpdateCallBack* New( ::fwData::Landmarks::sptr landmarks,
                                           const ::fwCom::SlotBase::sptr& pointModifiedSlot,
                                           const std::string& groupName,
                                           size_t pointIndex,
                                           vtkAbstractPropPicker* picker,
                                           vtkRenderer* renderer,
                                           SLandmarks::LabelActorType& labelActor)
    {
        return new vtkLandmarkUpdateCallBack(landmarks, pointModifiedSlot, groupName, pointIndex, picker, renderer,
                                             labelActor);
    }

//------------------------------------------------------------------------------

    vtkLandmarkUpdateCallBack( ::fwData::Landmarks::sptr landmarks,
                               const ::fwCom::SlotBase::sptr& pointModifiedSlot,
                               const std::string& groupName,
                               size_t pointIndex,
                               vtkAbstractPropPicker* picker,
                               vtkRenderer* renderer,
                               SLandmarks::LabelActorType& labelActor) :
        m_landmarks(landmarks),
        m_pointModifiedSlot(pointModifiedSlot),
        m_groupName(groupName),
        m_pointIndex(pointIndex),
        m_picker(picker),
        m_renderer(renderer),
        m_labelActor(labelActor)
    {
    }

//------------------------------------------------------------------------------

    void updateInfo(const std::string& groupName, size_t pointIndex)
    {
        m_groupName  = groupName;
        m_pointIndex = pointIndex;
    }

//------------------------------------------------------------------------------

    virtual void Execute( vtkObject* caller, unsigned long eventId, void*)
    {
        vtkHandleWidget* handler = vtkHandleWidget::SafeDownCast(caller);
        if (!handler)
        {
            return;
        }

        if ( eventId == vtkCommand::StartInteractionEvent)
        {
            handler->AddObserver(vtkCommand::EndInteractionEvent, this );
            handler->AddObserver(vtkCommand::InteractionEvent, this );

        }
        else if ( eventId == vtkCommand::EndInteractionEvent )
        {
            handler->RemoveObservers(vtkCommand::EndInteractionEvent, this );
            handler->RemoveObservers(vtkCommand::InteractionEvent, this );
        }

        vtkHandleRepresentation* representation = handler->GetHandleRepresentation();
        SLM_ASSERT("handler not instanced", handler);
        double* world = representation->GetWorldPosition();

        if ( eventId == vtkCommand::InteractionEvent)
        {
            double display[3];
            int x, y;
            handler->GetInteractor()->GetLastEventPosition(x, y);
            display[0] = x;
            display[1] = y;
            display[2] = 0;

            if ( m_picker && m_picker->Pick( display, m_renderer ) )
            {
                ::fwRenderVTK::vtk::getNearestPickedPosition(m_picker, m_renderer, world);
            }

            auto& point = m_landmarks->getPoint(m_groupName, m_pointIndex);
            std::copy( world, world+3, point.begin() );

            representation->SetWorldPosition(world);
            m_labelActor->GetPositionCoordinate()->SetValue(world);

            auto sig = m_landmarks->signal< ::fwData::Landmarks::PointModifiedSigType >
                           ( ::fwData::Landmarks::s_POINT_MODIFIED_SIG );
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_pointModifiedSlot));
                sig->asyncEmit(m_groupName, m_pointIndex);
            }
        }
        else if (eventId == vtkCommand::StartInteractionEvent)
        {
            auto sig = m_landmarks->signal< ::fwData::Landmarks::PointSelectedSignalType >
                           (::fwData::Landmarks::s_POINT_SELECTED_SIG);
            sig->asyncEmit(m_groupName, m_pointIndex);
        }
    }

protected:

    ::fwData::Landmarks::sptr m_landmarks;

    const ::fwCom::SlotBase::sptr m_pointModifiedSlot;

    std::string m_groupName;

    size_t m_pointIndex;

    vtkAbstractPropPicker* m_picker;

    vtkRenderer* m_renderer;

    SLandmarks::LabelActorType m_labelActor;
};

//------------------------------------------------------------------------------

class vtkDeselectLandmarksCallBack : public vtkCommand
{

public:

//------------------------------------------------------------------------------

    static vtkDeselectLandmarksCallBack* New()
    {
        return new vtkDeselectLandmarksCallBack();
    }

//------------------------------------------------------------------------------

    vtkDeselectLandmarksCallBack()
    {
    }

    //------------------------------------------------------------------------------

    void setAdaptor(const SLandmarks::sptr& adaptor)
    {
        m_adaptor = adaptor;
    }

//------------------------------------------------------------------------------

    virtual void Execute( vtkObject* caller, unsigned long eventId, void*)
    {
        m_adaptor->deselect();
    }

protected:

    SLandmarks::sptr m_adaptor;
};

//------------------------------------------------------------------------------

SLandmarks::SLandmarks() throw() :
    m_noSelectionCommand(nullptr),
    m_count(0)
{
    this->newSlot(s_ADD_POINT_SLOT, &SLandmarks::addPoint, this);
    this->newSlot(s_INSERT_POINT_SLOT, &SLandmarks::insertPoint, this);
    this->newSlot(s_REMOVE_POINT_SLOT, &SLandmarks::removePoint, this);
    this->newSlot(s_MODIFY_POINT_SLOT, &SLandmarks::modifyPoint, this);
    this->newSlot(s_ADD_GROUP_SLOT, &SLandmarks::addGroup, this);
    this->newSlot(s_REMOVE_GROUP_SLOT, &SLandmarks::removeGroup, this);
    this->newSlot(s_MODIFY_GROUP_SLOT, &SLandmarks::modifyGroup, this);
    this->newSlot(s_RENAME_GROUP_SLOT, &SLandmarks::renameGroup, this);
    this->newSlot(s_SELECT_POINT_SLOT, &SLandmarks::selectPoint, this);
    this->newSlot(s_DESELECT_POINT_SLOT, &SLandmarks::deselectPoint, this);
    this->newSlot(s_SHOW_SLOT, &SLandmarks::show, this);
}

//------------------------------------------------------------------------------

SLandmarks::~SLandmarks() throw()
{
}

//------------------------------------------------------------------------------

void SLandmarks::doConfigure() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SLandmarks::doStart() throw(fwTools::Failed)
{
    vtkDeselectLandmarksCallBack* callback = vtkDeselectLandmarksCallBack::New();
    callback->setAdaptor(this->getSptr());
    m_noSelectionCommand = callback;
    this->getInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent, m_noSelectionCommand);
    this->getInteractor()->AddObserver(vtkCommand::RightButtonReleaseEvent, m_noSelectionCommand);

    this->doUpdate();
}

//------------------------------------------------------------------------------

void SLandmarks::doSwap() throw(fwTools::Failed)
{
    this->doStop();
    this->doStart();
}

//------------------------------------------------------------------------------

void SLandmarks::doUpdate() throw(fwTools::Failed)
{
    this->clearLandmarks();

    ::fwData::Landmarks::csptr landmarks = this->getObject< ::fwData::Landmarks >();

    ::fwData::Landmarks::GroupNameContainer groupNames = landmarks->getGroupNames();

    for(auto& name : groupNames)
    {
        this->addGroup(name);
    }

    this->getRenderer()->ResetCameraClippingRange();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SLandmarks::doStop() throw(fwTools::Failed)
{
    this->clearLandmarks();
    if (m_noSelectionCommand)
    {
        this->getInteractor()->RemoveObserver(m_noSelectionCommand);
        m_noSelectionCommand->Delete();
        m_noSelectionCommand = nullptr;
    }

}

//------------------------------------------------------------------------------

void SLandmarks::addPoint(std::string groupName)
{
    ::fwData::Landmarks::sptr landmarks = this->getObject< ::fwData::Landmarks >();

    for(size_t i = m_handles[groupName].size(); i < landmarks->getNumberOfPoints(groupName); ++i)
    {
        auto handle = this->newHandle(landmarks, groupName, i);
        m_handles[groupName].push_back(handle);
    }

    this->getRenderer()->ResetCameraClippingRange();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::insertPoint(std::string groupName, size_t index)
{
    ::fwData::Landmarks::sptr landmarks = this->getObject< ::fwData::Landmarks >();

    auto handle = this->newHandle(landmarks, groupName, index);

    LandmarksWidgetContainerType& landmarkHandleGroup = m_handles[groupName];
    landmarkHandleGroup.insert(
        landmarkHandleGroup.begin() + static_cast<GroupWidgetsMapType::difference_type>(index), handle);

    this->getRenderer()->ResetCameraClippingRange();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::removePoint(std::string groupName, size_t index)
{
    this->deselect();
    LandmarksWidgetContainerType& landmarkHandleGroup = m_handles[groupName];

    auto& handleToRemove = landmarkHandleGroup[index];

    LabelActorType& label = m_labels[handleToRemove];
    this->getRenderer()->RemoveViewProp(label);
    label->Delete();

    vtkCommand* command = m_commands[handleToRemove];
    handleToRemove->RemoveObserver(command);
    command->Delete();

    handleToRemove->Off();
    handleToRemove->SetInteractor(nullptr);

    this->m_propCollection->RemoveItem(handleToRemove->GetRepresentation());

    m_commands.erase(handleToRemove);
    m_labels.erase(handleToRemove);
    landmarkHandleGroup.erase(landmarkHandleGroup.begin() + static_cast<GroupWidgetsMapType::difference_type>(index));

    for(size_t i = index; i < landmarkHandleGroup.size(); ++i)
    {
        auto& handle                               = landmarkHandleGroup.at(i);
        vtkLandmarkUpdateCallBack* commandToUpdate = dynamic_cast<vtkLandmarkUpdateCallBack* >(m_commands[handle]);
        commandToUpdate->updateInfo(groupName, i);

        LabelActorType& labelToUpdate             = m_labels[handle];
        vtkSmartPointer<vtkTextMapper> textMapper = vtkTextMapper::SafeDownCast(labelToUpdate->GetMapper());
        const std::string label                   = groupName + "_" + std::to_string(i);
        textMapper->SetInput(label.c_str());
    }

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::modifyGroup(std::string groupName)
{
    ::fwData::Landmarks::csptr landmarks = this->getObject< ::fwData::Landmarks >();
    const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(groupName);

    LandmarksWidgetContainerType& landmarkHandleGroup = m_handles[groupName];

    for(auto& handle : landmarkHandleGroup)
    {
        const ::fwData::Landmarks::ColorType& color = group.m_color;

        vtkSmartPointer< ::fwRenderVTK::vtk::fwHandleRepresentation3D> rep =
            ::fwRenderVTK::vtk::fwHandleRepresentation3D::SafeDownCast(handle->GetHandleRepresentation());

        if (group.m_shape == ::fwData::Landmarks::Shape::CUBE)
        {
            rep->SetShapeRepresentation(::fwRenderVTK::vtk::fwHandleRepresentation3D::CUBE);
        }
        else
        {
            rep->SetShapeRepresentation(::fwRenderVTK::vtk::fwHandleRepresentation3D::SPHERE);
        }

        rep->GetSelectedProperty()->SetOpacity(color[3]);
        rep->GetMarkerProperty()->SetOpacity(color[3]);
        rep->GetProperty()->SetColor(color[0], color[1], color[2]);
        rep->GetProperty()->SetOpacity(color[3]);
        rep->SetHandleSize(group.m_size);
        rep->SetVisibility(group.m_visibility);

        LabelActorType& textActor                 = m_labels[handle];
        vtkSmartPointer<vtkTextMapper> textMapper = vtkTextMapper::SafeDownCast(textActor->GetMapper());
        textMapper->GetTextProperty()->SetColor(color[0], color[1], color[2]);
        textActor->SetVisibility(group.m_visibility);
    }

    this->getRenderer()->ResetCameraClippingRange();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::removeGroup(std::string groupName)
{
    const size_t nbPoints = m_handles[groupName].size();

    // It is more efficient to walk through our vector backwards.
    // That way we avoid useless label renaming.
    for(size_t i = nbPoints; i > 0; --i)
    {
        this->removePoint(groupName, i - 1);
    }

    m_handles.erase(groupName);

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::addGroup(std::string groupName)
{
    const ::fwData::Landmarks::sptr& landmarks = this->getObject< ::fwData::Landmarks >();
    const size_t ptNumber                      = landmarks->getNumberOfPoints(groupName);

    for(size_t index = 0; index < ptNumber; ++index)
    {
        auto handle = this->newHandle(landmarks, groupName, index);
        m_handles[groupName].push_back(handle);
    }

    this->getRenderer()->ResetCameraClippingRange();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::modifyPoint(std::string groupName, size_t index)
{
    const ::fwData::Landmarks::sptr& landmarks = this->getObject< ::fwData::Landmarks >();

    ::fwData::Landmarks::PointType& point = landmarks->getPoint(groupName, index);
    LandmarkWidgetType& widget = m_handles.at(groupName).at(index);

    vtkHandleRepresentation* handleRep = vtkHandleRepresentation::SafeDownCast(widget->GetRepresentation());
    handleRep->SetWorldPosition(point.data());

    LabelActorType textActor = m_labels[widget];
    textActor->GetPositionCoordinate()->SetValue(point.data());

    this->getRenderer()->ResetCameraClippingRange();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::renameGroup(std::string oldName, std::string newName)
{
    LandmarksWidgetContainerType& landmarkHandleGroup = m_handles[oldName];
    m_handles.insert(std::make_pair(newName, landmarkHandleGroup));

    size_t count = 0;
    for(auto& handle : landmarkHandleGroup)
    {
        LabelActorType& textActor = m_labels[handle];

        vtkCommand* command                       = m_commands[handle];
        vtkLandmarkUpdateCallBack* updateCallback = dynamic_cast<vtkLandmarkUpdateCallBack* >(command);
        SLM_ASSERT("Missing landmark command", updateCallback);
        updateCallback->updateInfo(newName, count);

        vtkSmartPointer<vtkTextMapper> textMapper = vtkTextMapper::SafeDownCast(textActor->GetMapper());
        const std::string label                   = newName + "_" + std::to_string(count);
        textMapper->SetInput(label.c_str());
        ++count;
    }

    m_handles.erase(oldName);

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::selectPoint(std::string groupName, size_t index)
{
    // deselect the current selected point
    if (m_timer)
    {
        this->deselectPoint(m_selectedPoint.first, m_selectedPoint.second);
    }

    m_selectedPoint.first  = groupName;
    m_selectedPoint.second = index;

    LandmarkWidgetType& widget =
        m_handles.at(groupName).at(index);

    vtkSmartPointer< ::fwRenderVTK::vtk::fwHandleRepresentation3D> rep =
        ::fwRenderVTK::vtk::fwHandleRepresentation3D::SafeDownCast(widget->GetHandleRepresentation());

    const double* color = rep->GetProperty()->GetColor();

    m_timer = m_associatedWorker->createTimer();

    ::fwThread::Timer::TimeDurationType duration = ::boost::chrono::milliseconds(500);

    const std::array<double, 3> color1 = {{color[0], color[1], color[2]}};
    const std::array<double, 3> color2 = {{0., 1., 0.}};

    m_count = 0;
    m_timer->setFunction(std::bind(&SLandmarks::changeColor, this, rep, color1, color2));
    m_timer->setDuration(duration);
    m_timer->start();

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::deselectPoint(std::string groupName, size_t index)
{
    if(m_timer)
    {
        m_timer->stop();
        m_timer.reset();
    }

    const ::fwData::Landmarks::sptr& landmarks = this->getObject< ::fwData::Landmarks >();

    LandmarkWidgetType& widget = m_handles.at(groupName).at(index);

    vtkSmartPointer< ::fwRenderVTK::vtk::fwHandleRepresentation3D> rep =
        ::fwRenderVTK::vtk::fwHandleRepresentation3D::SafeDownCast(widget->GetHandleRepresentation());

    ::fwData::Landmarks::ColorType& color = landmarks->getGroup(groupName).m_color;

    double castedColour[3] = { static_cast< double >(color[0]),
                               static_cast< double >(color[1]),
                               static_cast< double >(color[2]) };

    rep->GetProperty()->SetColor(castedColour);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::deselect()
{
    if (m_timer)
    {
        const ::fwData::Landmarks::csptr& landmarks = this->getObject< ::fwData::Landmarks >();

        this->deselectPoint(m_selectedPoint.first, m_selectedPoint.second);
        auto sig = landmarks->signal< ::fwData::Landmarks::PointDeselectedSignalType >(
            ::fwData::Landmarks::s_POINT_DESELECTED_SIG);
        ::fwCom::Connection::Blocker block(sig->getConnection(this->slot(s_DESELECT_POINT_SLOT)));
        sig->asyncEmit(m_selectedPoint.first, m_selectedPoint.second);
    }
}

//------------------------------------------------------------------------------

void SLandmarks::changeColor(const vtkSmartPointer< ::fwRenderVTK::vtk::fwHandleRepresentation3D >& rep,
                             const std::array<double, 3>& color1, const std::array<double, 3>& color2)
{
    std::array<double, 3> color;
    if (m_count%2 == 0)
    {
        color = color2;
    }
    else
    {
        color = color1;
    }

    rep->GetProperty()->SetColor(color.data());

    ++m_count;
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLandmarks::clearLandmarks()
{
    while (!m_handles.empty())
    {
        const std::string name = m_handles.begin()->first;
        this->removeGroup(name);
    }
}

//------------------------------------------------------------------------------

vtkSmartPointer< vtkHandleWidget > SLandmarks::newHandle(const ::fwData::Landmarks::sptr& landmarks,
                                                         const std::string& groupName,
                                                         size_t pointIndex)
{
    const ::fwData::Landmarks::LandmarksGroup& group = landmarks->getGroup(groupName);
    ::fwData::Landmarks::PointType& point = landmarks->getPoint(groupName, pointIndex);

    vtkSmartPointer< vtkHandleWidget > handle = vtkHandleWidget::New();

    const ::fwData::Landmarks::ColorType& color = group.m_color;

    // TODO add option for cube representation
    vtkSmartPointer< ::fwRenderVTK::vtk::fwHandleRepresentation3D > pointRep =
        ::fwRenderVTK::vtk::fwHandleRepresentation3D::New();

    if (group.m_shape == ::fwData::Landmarks::Shape::SPHERE)
    {

        pointRep->SetShapeRepresentation(::fwRenderVTK::vtk::fwHandleRepresentation3D::SPHERE);
    }
    else
    {
        pointRep->SetShapeRepresentation(::fwRenderVTK::vtk::fwHandleRepresentation3D::CUBE);
    }
    pointRep->GetSelectedProperty()->SetOpacity(color[3]);
    pointRep->GetMarkerProperty()->SetOpacity(color[3]);
    pointRep->GetProperty()->SetColor(color[0], color[1], color[2]);
    pointRep->GetProperty()->SetOpacity(color[3]);
    pointRep->SetHandleSize(group.m_size);
    pointRep->SetVisibility(group.m_visibility);

    pointRep->SetWorldPosition(point.data());

    handle->SetRepresentation(pointRep);
    handle->SetPriority(0.8f);

    handle->SetInteractor( this->getInteractor() );
    handle->KeyPressActivationOff();

    handle->On();

    // We don't want to add m_representation to the renderer, m_handle
    // is already managing that.
    this->registerProp(pointRep);

    // create label
    const std::string label = groupName + "_" + std::to_string(pointIndex);

    vtkSmartPointer<vtkActor2D> textActor     = vtkActor2D::New();
    vtkSmartPointer<vtkTextMapper> textMapper = vtkTextMapper::New();
    textMapper->GetTextProperty()->SetFontFamilyToCourier(); // Fixed-width font
    textMapper->GetTextProperty()->ShadowOn(); // better contrast
    textMapper->GetTextProperty()->BoldOn();
    textMapper->GetTextProperty()->SetFontSize(15);
    textMapper->GetTextProperty()->SetColor(color[0], color[1], color[2]);
    textMapper->SetInput(label.c_str());

    textActor->SetMapper(textMapper);
    textActor->GetPositionCoordinate()->SetCoordinateSystemToWorld();
    textActor->GetPosition2Coordinate()->SetCoordinateSystemToWorld();
    textActor->GetPositionCoordinate()->SetValue(point.data());

    m_labels.insert(std::make_pair(handle, textActor));

    this->addToRenderer(textActor);

    const auto& slot = this->slot(s_MODIFY_POINT_SLOT);

    vtkLandmarkUpdateCallBack* updateCallback;
    updateCallback = vtkLandmarkUpdateCallBack::New(landmarks, slot, groupName, pointIndex, this->getPicker(),
                                                    this->getRenderer(), textActor);

    m_commands.insert(std::make_pair(handle, updateCallback));

    handle->AddObserver(vtkCommand::StartInteractionEvent, updateCallback);

    return handle;
}

//------------------------------------------------------------------------------

void SLandmarks::show(bool b)
{
    if (b)
    {
        this->doStart();
    }
    else
    {
        this->doStop();
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SLandmarks::getObjSrvConnections() const
{
    KeyConnectionsType connections;

    connections.push_back(std::make_pair(::fwData::Landmarks::s_POINT_ADDED_SIG, s_ADD_POINT_SLOT));
    connections.push_back(std::make_pair(::fwData::Landmarks::s_POINT_INSERTED_SIG, s_INSERT_POINT_SLOT));
    connections.push_back(std::make_pair(::fwData::Landmarks::s_POINT_REMOVED_SIG, s_REMOVE_POINT_SLOT ));
    connections.push_back(std::make_pair(::fwData::Landmarks::s_GROUP_ADDED_SIG, s_ADD_GROUP_SLOT));
    connections.push_back(std::make_pair(::fwData::Landmarks::s_GROUP_REMOVED_SIG, s_REMOVE_GROUP_SLOT));
    connections.push_back(std::make_pair(::fwData::Landmarks::s_GROUP_MODIFIED_SIG, s_MODIFY_GROUP_SLOT));
    connections.push_back(std::make_pair(::fwData::Landmarks::s_POINT_MODIFIED_SIG, s_MODIFY_POINT_SLOT));
    connections.push_back(std::make_pair(::fwData::Landmarks::s_MODIFIED_SIG, s_UPDATE_SLOT));
    connections.push_back(std::make_pair(::fwData::Landmarks::s_GROUP_RENAMED_SIG, s_RENAME_GROUP_SLOT));
    connections.push_back(std::make_pair(::fwData::Landmarks::s_POINT_SELECTED_SIG, s_SELECT_POINT_SLOT));
    connections.push_back(std::make_pair(::fwData::Landmarks::s_POINT_DESELECTED_SIG, s_DESELECT_POINT_SLOT));

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
