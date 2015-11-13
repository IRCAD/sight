/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiSpline/SSplinePointsEditor.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>
#include <fwComEd/InteractionMsg.hpp>
#include <fwComEd/PointListMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include <fwGuiQt/container/QtContainer.hpp>
#include <fwGui/dialog/InputDialog.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <QGridLayout>
#include <QLineEdit>
#include <QList>
#include <QSlider>
#include <QString>
#include <QWidget>

fwServicesRegisterMacro(::gui::editor::IEditor, ::uiSpline::SSplinePointsEditor, ::fwData::PointList);

namespace uiSpline
{

const ::fwCom::Signals::SignalKeyType SSplinePointsEditor::s_POINT_SELECTED_SIG       = "pointSelected";
const ::fwCom::Signals::SignalKeyType SSplinePointsEditor::s_INDEX_POINT_SELECTED_SIG = "indexPointSelected";
const ::fwCom::Slots::SlotKeyType SSplinePointsEditor::s_UPDATE_POINTLIST_SLOT        = "updatePointList";
const std::string SSplinePointsEditor::s_FIELD_NAME                                   = "PointName";

SSplinePointsEditor::SSplinePointsEditor() throw() : m_numberOfPoints(0), m_countPoint(0)
{
    m_connectObj = ::navigation::ConnectPoints::New();

    m_sigPointSelected      = PointSelectedSignalType::New();
    m_sigIndexPointSelected = IndexPointSelectedSignalType::New();

    ::fwCom::HasSignals::m_signals(s_POINT_SELECTED_SIG, m_sigPointSelected);
    ::fwCom::HasSignals::m_signals(s_INDEX_POINT_SELECTED_SIG, m_sigIndexPointSelected);

    m_slotUpdatePointList = ::fwCom::newSlot(&SSplinePointsEditor::updatePointList,this);
    ::fwCom::HasSlots::m_slots(s_UPDATE_POINTLIST_SLOT, m_slotUpdatePointList);

    this->setWorker(::fwServices::registry::ActiveWorkers::getDefault()->
                    getWorker(::fwServices::registry::ActiveWorkers::s_DEFAULT_WORKER));
}

//------------------------------------------------------------------------------

SSplinePointsEditor::~SSplinePointsEditor() throw()
{
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());
    QWidget* container = qtContainer->getQtContainer();
    SLM_ASSERT("container not instanced", container);

    m_renamePointButton = new QPushButton(QObject::tr("Rename point"), container);
    m_renamePointButton->setEnabled(false);

    m_removePointButton = new QPushButton(QObject::tr("Remove point"), container);
    m_removePointButton->setEnabled(false);

    m_removeAllPointsButton = new QPushButton(QObject::tr("Remove all point"), container);

    m_list = new QListWidget();
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);

    QGridLayout* layout = new QGridLayout(container);
    layout->addWidget(m_renamePointButton, 0, 0, 1, 1);
    layout->addWidget(m_removePointButton, 0, 1, 1, 1);
    layout->addWidget(m_removeAllPointsButton, 0, 2, 1, 1);
    layout->addWidget(m_list, 1, 0, 1, 4);

    container->setLayout(layout);

    QObject::connect(m_renamePointButton, SIGNAL(clicked()), this, SLOT(onClickRenamePoint()));
    QObject::connect(m_removePointButton, SIGNAL(clicked()), this, SLOT(onClickRemovePoint()));
    QObject::connect(m_removeAllPointsButton, SIGNAL(clicked()), this, SLOT(onClickRemoveAllPoints()));
    QObject::connect(
        m_list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onDoubleClickItem(QListWidgetItem*)));
    QObject::connect(
        m_list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onClickItem(QListWidgetItem*)));

    // Get the points for activity export
    ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
    SLM_ASSERT("Invalid pointList object", pointList);

    for(::fwData::Point::sptr point :  pointList->getRefPoints())
    {
        SLM_ASSERT("Invalid point object", point);
        ::fwData::String::sptr text = ::fwData::String::dynamicCast(point->getField(s_FIELD_NAME));
        m_list->addItem(new QListWidgetItem(QString::fromStdString(text->value())) );
        m_countPoint++;
        m_numberOfPoints++;
    }

    if(m_numberOfPoints > 0)
    {
        OSLM_DEBUG("Connect all spline points");
        m_connectObj->connectAllSplinePoints(pointList, this->getSptr(), "updatePointList");
        m_removeAllPointsButton->setEnabled(true);
    }
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_renamePointButton, SIGNAL(clicked()), this,SLOT(onClickRenamePoint()));
    QObject::disconnect(m_removePointButton, SIGNAL(clicked()), this,SLOT(onClickRemovePoint()));
    QObject::disconnect(m_removeAllPointsButton, SIGNAL(clicked()), this,SLOT(onClickRemoveAllPoint()));
    QObject::disconnect(m_list, SIGNAL(itemClicked(QListWidgetItem *)),this, SLOT(onClickItem(QListWidgetItem*)));
    QObject::disconnect(
        m_list,SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onDoubleClickItem(QListWidgetItem*)));

    m_connectObj->disconnectSplinePoints();

    this->getContainer()->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::configuring() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    // Get the point list to store selected points
    const std::vector<ConfigurationType> pointsConfig = m_configuration->find("points");
    if(!pointsConfig.empty())
    {
        SLM_ASSERT("UID attribute is missing", pointsConfig.at(0)->hasAttribute("uid"));
        m_selectedPointsUID = pointsConfig.at(0)->getAttributeValue("uid");
    }
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::updating() throw(::fwTools::Failed)
{
    ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList >();

    ::fwServices::ObjectMsg::sptr msg = ::fwServices::ObjectMsg::New();
    msg->addEvent(::fwComEd::PointListMsg::ELEMENT_ADDED);
    msg->setSource(this->getSptr());
    msg->setSubject( pointList);
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = pointList->signal< ::fwData::Object::ObjectModifiedSignalType >(::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( msg);
    }

    m_sigIndexPointSelected->asyncEmit (m_numberOfPoints - 1);
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::receiving(::fwServices::ObjectMsg::csptr msg)
throw (::fwTools::Failed)
{
    ::fwComEd::InteractionMsg::csptr interactionmsg = ::fwComEd::InteractionMsg::dynamicConstCast(msg);

    if(interactionmsg && msg->hasEvent(::fwComEd::InteractionMsg::MOUSE_LEFT_UP) && interactionmsg->getControlStatus())
    {
        ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();

        // Add the point clicked on a negato in the PointList
        pointList->getRefPoints().push_back(::fwData::Point::constCast(interactionmsg->getEventPoint()));

        // Connect point
        ::fwData::Point::sptr point = ::fwData::Point::constCast(interactionmsg->getEventPoint());
        m_connectObj->connectPointToService(point,"objectModified",this->getSptr(),"updatePointList");

        m_sigPointSelected->asyncEmit(point);

        m_numberOfPoints++;
        m_countPoint++;

        QString name = QString("P%1").arg(m_countPoint);
        m_list->addItem(new QListWidgetItem(name));

        m_removeAllPointsButton->setEnabled(true);

        point->setField(s_FIELD_NAME, ::fwData::String::New(name.toStdString()));
        point->setField(::fwComEd::Dictionary::m_labelId, ::fwData::String::New(name.toStdString()));

        this->fillVisualizePointList(m_numberOfPoints - 1);
        this->updating();
    }
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::onClickItem(QListWidgetItem* item)
{
    m_renamePointButton->setEnabled(true);
    m_removePointButton->setEnabled(true);

    ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
    const int index = m_list->row(item);
    ::fwData::Point::sptr point = pointList->getRefPoints()[index];
    this->fillVisualizePointList(index);

    m_sigPointSelected->asyncEmit (point);
    m_sigIndexPointSelected->asyncEmit (index);
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::onDoubleClickItem(QListWidgetItem * item)
{
    ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
    const int index = m_list->row(item);
    ::fwData::Point::sptr point = pointList->getRefPoints()[index];

    m_sigPointSelected->asyncEmit(point);
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::onClickRenamePoint()
{
    ::fwGui::dialog::InputDialog dialogWindows;
    dialogWindows.setTitle("Rename view point");
    dialogWindows.setMessage("Enter new name");

    QListWidgetItem* item  = m_list->selectedItems().takeFirst();
    const std::string text = dialogWindows.getInput();
    const int index        = m_list->row(item);

    if(text.size() != 0)
    {
        item->setText(QString::fromUtf8(text.c_str()));

        ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
        pointList->getRefPoints()[index]->setField(s_FIELD_NAME,::fwData::String::New(text));
        pointList->getRefPoints()[index]->setField(::fwComEd::Dictionary::m_labelId, ::fwData::String::New(text));

        this->fillVisualizePointList(index);
    }

    // Send the signal m_sigIndexPointSelected to update point Label on the CPR image.
    m_sigIndexPointSelected->asyncEmit(index);

    m_renamePointButton->setEnabled(false);
    m_removePointButton->setEnabled(false);
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::onClickRemovePoint()
{
    QListWidgetItem* item = m_list->selectedItems().takeFirst();
    const int indexPoint  = m_list->row(item);

    m_numberOfPoints--;

    ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();
    ::fwData::Point::sptr point         = pointList->getRefPoints()[indexPoint];

    m_connectObj->disconnectPointToService(point);
    pointList->getRefPoints().erase(pointList->getRefPoints().begin() + indexPoint);

    m_list->removeItemWidget(item);
    delete(item);

    this->clearVisualizePointList();

    ::fwComEd::PointListMsg::sptr msg = ::fwComEd::PointListMsg::New();
    msg->addEvent(::fwComEd::PointListMsg::ELEMENT_REMOVED, point);
    msg->setSource(this->getSptr());
    msg->setSubject( pointList);
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = pointList->signal< ::fwData::Object::ObjectModifiedSignalType >(::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( msg );
    }

    if(m_numberOfPoints > 0)
    {
        this->fillVisualizePointList(m_numberOfPoints - 1);
    }

    m_renamePointButton->setEnabled(false);
    m_removePointButton->setEnabled(false);
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::onClickRemoveAllPoints()
{
    m_numberOfPoints = 0;
    m_countPoint     = 0;

    ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();

    this->clearVisualizePointList();

    pointList->getRefPoints().clear();
    m_list->clear();

    ::fwComEd::PointListMsg::sptr msg = ::fwComEd::PointListMsg::New();
    msg->addEvent(::fwComEd::PointListMsg::ELEMENT_REMOVED);
    msg->setSource(this->getSptr());
    msg->setSubject( pointList);
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = pointList->signal< ::fwData::Object::ObjectModifiedSignalType >(::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( msg);
    }

    m_renamePointButton->setEnabled(false);
    m_removePointButton->setEnabled(false);
    m_removeAllPointsButton->setEnabled(false);
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::fillVisualizePointList(int selectedPointIndex)
{
    ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();

    this->clearVisualizePointList();
    this->addPointToVisualizePointList(pointList, selectedPointIndex);

    // if the selected point is the last point, visualize the 2 points before
    if(selectedPointIndex == m_numberOfPoints - 1 )
    {
        if(m_numberOfPoints == 2)
        {
            this->addPointToVisualizePointList(pointList, selectedPointIndex - 1);
        }
        else if(m_numberOfPoints >= 3)
        {
            this->addPointToVisualizePointList(pointList, selectedPointIndex - 1);
            this->addPointToVisualizePointList(pointList, selectedPointIndex - 2);
        }
    }
    else // visualize selected point neighbors
    {
        if(selectedPointIndex > 0)
        {
            this->addPointToVisualizePointList(pointList, selectedPointIndex - 1);
            this->addPointToVisualizePointList(pointList, selectedPointIndex + 1);

        }
        else if(selectedPointIndex==0 && m_numberOfPoints > 1)
        {
            this->addPointToVisualizePointList(pointList, selectedPointIndex + 1);
        }
    }
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::addPointToVisualizePointList(
    const ::fwData::PointList::sptr& pointList, int selectedPointIndex)
{
    ::fwTools::Object::sptr pointListObject = ::fwTools::fwID::getObject(m_selectedPointsUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_selectedPointsUID << "'", pointListObject);
    ::fwData::PointList::sptr visualizePointList = ::fwData::PointList::dynamicCast(pointListObject);
    OSLM_ASSERT("Failed to retrieve point list", visualizePointList);

    ::fwData::Point::sptr pointToAdd = pointList->getRefPoints()[selectedPointIndex];
    visualizePointList->getRefPoints().push_back(pointToAdd);

    ::fwComEd::PointListMsg::sptr msg = ::fwComEd::PointListMsg::New();
    msg->addEvent(::fwComEd::PointListMsg::ELEMENT_ADDED);
    msg->setSource(this->getSptr());
    msg->setSubject( visualizePointList);
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = visualizePointList->signal< ::fwData::Object::ObjectModifiedSignalType >(
        ::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( msg);
    }
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::clearVisualizePointList()
{
    ::fwTools::Object::sptr pointListObject = ::fwTools::fwID::getObject(m_selectedPointsUID);
    OSLM_ASSERT("Failed to retrieve object with UID '" << m_selectedPointsUID << "'", pointListObject);
    ::fwData::PointList::sptr visualizePointList = ::fwData::PointList::dynamicCast(pointListObject);
    OSLM_ASSERT("Failed to retrieve point list", visualizePointList);

    if(visualizePointList && visualizePointList->getRefPoints().size() > 0)
    {
        // Messages to notify that points of visualizePointList are removed
        for(int i = 0; i < visualizePointList->getRefPoints().size(); i++)
        {
            ::fwData::Point::sptr point = visualizePointList->getRefPoints()[i];

            ::fwComEd::PointListMsg::sptr msg = ::fwComEd::PointListMsg::New();
            msg->addEvent(::fwComEd::PointListMsg::ELEMENT_REMOVED, point);
            msg->setSource(this->getSptr());
            msg->setSubject(visualizePointList);
            ::fwData::Object::ObjectModifiedSignalType::sptr sig;
            sig = visualizePointList->signal< ::fwData::Object::ObjectModifiedSignalType >(
                ::fwData::Object::s_OBJECT_MODIFIED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
                sig->asyncEmit( msg);
            }
        }

        // Clears the pointList
        visualizePointList->getRefPoints().clear();
    }
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::updatePointList()
{
    ::fwData::PointList::sptr pointList = this->getObject< ::fwData::PointList>();

    ::fwServices::ObjectMsg::sptr msg = ::fwServices::ObjectMsg::New();
    msg->addEvent(::fwComEd::PointListMsg::ELEMENT_MODIFIED );
    msg->setSource( this->getSptr());
    msg->setSubject(pointList);
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = pointList->signal< ::fwData::Object::ObjectModifiedSignalType >(::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( msg );
    }
}

} // namespace uiSpline

