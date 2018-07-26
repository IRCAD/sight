/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiSpline/SSplinePointsEditor.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/PointList.hpp>
#include <fwData/String.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>

#include <fwGui/dialog/InputDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <QGridLayout>
#include <QLineEdit>
#include <QList>
#include <QSlider>
#include <QString>
#include <QWidget>

fwServicesRegisterMacro(::fwGui::editor::IEditor, ::uiSpline::SSplinePointsEditor, ::fwData::PointList);

namespace uiSpline
{

static const std::string S_POINTS_KEY          = "points";
static const std::string S_SELECTED_POINTS_KEY = "selectedPoints";

const ::fwCom::Signals::SignalKeyType SSplinePointsEditor::s_POINT_SELECTED_SIG       = "pointSelected";
const ::fwCom::Signals::SignalKeyType SSplinePointsEditor::s_INDEX_POINT_SELECTED_SIG = "indexPointSelected";

const ::fwCom::Slots::SlotKeyType SSplinePointsEditor::s_UPDATE_POINTLIST_SLOT = "updatePointList";
const ::fwCom::Slots::SlotKeyType SSplinePointsEditor::s_GET_INTERACTION_SLOT  = "getInteraction";

const std::string SSplinePointsEditor::s_FIELD_NAME = "PointName";

//------------------------------------------------------------------------------

SSplinePointsEditor::SSplinePointsEditor() noexcept :
    m_numberOfPoints(0),
    m_countPoint(0)
{
    m_connectObj = ::navigation::ConnectPoints::New();

    m_sigPointSelected      = newSignal<PointSelectedSignalType>(s_POINT_SELECTED_SIG);
    m_sigIndexPointSelected = newSignal<IndexPointSelectedSignalType>(s_INDEX_POINT_SELECTED_SIG);

    newSlot(s_UPDATE_POINTLIST_SLOT, &SSplinePointsEditor::updatePointList, this);
    newSlot(s_GET_INTERACTION_SLOT, &SSplinePointsEditor::getInteraction, this);
}

//------------------------------------------------------------------------------

SSplinePointsEditor::~SSplinePointsEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::starting()
{
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    m_renamePointButton = new QPushButton(QObject::tr("Rename point"));
    m_renamePointButton->setEnabled(false);

    m_removePointButton = new QPushButton(QObject::tr("Remove point"));
    m_removePointButton->setEnabled(false);

    m_removeAllPointsButton = new QPushButton(QObject::tr("Remove all point"));

    m_list = new QListWidget();
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(m_renamePointButton, 0, 0, 1, 1);
    layout->addWidget(m_removePointButton, 0, 1, 1, 1);
    layout->addWidget(m_removeAllPointsButton, 0, 2, 1, 1);
    layout->addWidget(m_list, 1, 0, 1, 4);

    QObject::connect(m_renamePointButton, SIGNAL(clicked()), this, SLOT(onClickRenamePoint()));
    QObject::connect(m_removePointButton, SIGNAL(clicked()), this, SLOT(onClickRemovePoint()));
    QObject::connect(m_removeAllPointsButton, SIGNAL(clicked()), this, SLOT(onClickRemoveAllPoints()));
    QObject::connect(
        m_list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onDoubleClickItem(QListWidgetItem*)));
    QObject::connect(
        m_list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onClickItem(QListWidgetItem*)));

    // Get the points for activity export
    ::fwData::PointList::sptr pointList = this->getInOut< ::fwData::PointList>(S_POINTS_KEY);
    SLM_ASSERT("Invalid pointList object", pointList);

    for(::fwData::Point::sptr point : pointList->getPoints())
    {
        SLM_ASSERT("Invalid point object", point);
        ::fwData::String::sptr text = ::fwData::String::dynamicCast(point->getField(s_FIELD_NAME));
        m_list->addItem(new QListWidgetItem(QString::fromStdString(text->value())) );
        m_countPoint++;
        m_numberOfPoints++;
    }

    if(m_numberOfPoints > 0)
    {
        SLM_DEBUG("Connect all spline points");
        m_connectObj->connectAllSplinePoints(pointList, this->getSptr(), "updatePointList");
        m_removeAllPointsButton->setEnabled(true);
    }

    qtContainer->setLayout(layout);
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::stopping()
{
    SLM_TRACE_FUNC();

    QObject::disconnect(m_renamePointButton, SIGNAL(clicked()), this, SLOT(onClickRenamePoint()));
    QObject::disconnect(m_removePointButton, SIGNAL(clicked()), this, SLOT(onClickRemovePoint()));
    QObject::disconnect(m_removeAllPointsButton, SIGNAL(clicked()), this, SLOT(onClickRemoveAllPoint()));
    QObject::disconnect(m_list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onClickItem(QListWidgetItem*)));
    QObject::disconnect(
        m_list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onDoubleClickItem(QListWidgetItem*)));

    m_connectObj->disconnectSplinePoints();

    this->destroy();
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::configuring()
{
    this->initialize();
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::updating()
{
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::getInteraction(::fwDataTools::PickingInfo info)
{
    if (info.m_eventId == ::fwDataTools::PickingInfo::Event::MOUSE_LEFT_UP &&
        info.m_modifierMask == ::fwDataTools::PickingInfo::CTRL)
    {
        ::fwData::PointList::sptr pointList = this->getInOut< ::fwData::PointList>(S_POINTS_KEY);

        ::fwData::Point::sptr point = ::fwData::Point::New(info.m_worldPos[0], info.m_worldPos[1], info.m_worldPos[2]);
        // Add the point clicked on a negato in the PointList
        pointList->getPoints().push_back(point);

        // Connect point
        m_connectObj->connectPointToService(point, "modified", this->getSptr(), "updatePointList");

        m_sigPointSelected->asyncEmit(point);

        m_numberOfPoints++;
        m_countPoint++;

        QString name = QString("P%1").arg(m_countPoint);
        m_list->addItem(new QListWidgetItem(name));

        m_removeAllPointsButton->setEnabled(true);

        point->setField(s_FIELD_NAME, ::fwData::String::New(name.toStdString()));
        point->setField(::fwDataTools::fieldHelper::Image::m_labelId, ::fwData::String::New(name.toStdString()));

        this->fillVisualizePointList(m_numberOfPoints - 1);

        auto sig =
            pointList->signal< ::fwData::PointList::PointAddedSignalType >(::fwData::PointList::s_POINT_ADDED_SIG);
        sig->asyncEmit(point);

        m_sigIndexPointSelected->asyncEmit(m_numberOfPoints - 1);
    }
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::onClickItem(QListWidgetItem* item)
{
    m_renamePointButton->setEnabled(true);
    m_removePointButton->setEnabled(true);

    ::fwData::PointList::sptr pointList = this->getInOut< ::fwData::PointList>(S_POINTS_KEY);
    const int index = m_list->row(item);
    ::fwData::Point::sptr point = pointList->getPoints()[index];
    this->fillVisualizePointList(index);

    m_sigPointSelected->asyncEmit(point);
    m_sigIndexPointSelected->asyncEmit(index);
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::onDoubleClickItem(QListWidgetItem* item)
{
    ::fwData::PointList::sptr pointList = this->getInOut< ::fwData::PointList>(S_POINTS_KEY);
    const int index = m_list->row(item);
    ::fwData::Point::sptr point = pointList->getPoints()[index];

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

        ::fwData::PointList::sptr pointList = this->getInOut< ::fwData::PointList>(S_POINTS_KEY);
        pointList->getPoints()[index]->setField(s_FIELD_NAME, ::fwData::String::New(text));
        pointList->getPoints()[index]->setField(::fwDataTools::fieldHelper::Image::m_labelId,
                                                ::fwData::String::New(text));

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

    ::fwData::PointList::sptr pointList = this->getInOut< ::fwData::PointList>(S_POINTS_KEY);
    ::fwData::Point::sptr point         = pointList->getPoints()[indexPoint];

    m_connectObj->disconnectPointToService(point);
    pointList->getPoints().erase(pointList->getPoints().begin() + indexPoint);

    m_list->removeItemWidget(item);
    delete(item);

    this->clearVisualizePointList();

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

    ::fwData::PointList::sptr pointList = this->getInOut< ::fwData::PointList>(S_POINTS_KEY);

    this->clearVisualizePointList();

    pointList->getPoints().clear();
    m_list->clear();

    m_renamePointButton->setEnabled(false);
    m_removePointButton->setEnabled(false);
    m_removeAllPointsButton->setEnabled(false);
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::fillVisualizePointList(int selectedPointIndex)
{
    ::fwData::PointList::sptr pointList = this->getInOut< ::fwData::PointList>(S_POINTS_KEY);

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
        else if(selectedPointIndex == 0 && m_numberOfPoints > 1)
        {
            this->addPointToVisualizePointList(pointList, selectedPointIndex + 1);
        }
    }
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::addPointToVisualizePointList(const ::fwData::PointList::sptr& pointList,
                                                       int selectedPointIndex)
{
    ::fwData::PointList::sptr visualizePointList = this->getInOut< ::fwData::PointList>(S_SELECTED_POINTS_KEY);
    SLM_ASSERT( S_SELECTED_POINTS_KEY + " doesn't exist or is not a pointList", pointList);

    ::fwData::Point::sptr pointToAdd = pointList->getPoints()[selectedPointIndex];
    visualizePointList->getPoints().push_back(pointToAdd);

    auto sig = visualizePointList->signal< ::fwData::PointList::PointAddedSignalType >(
        ::fwData::PointList::s_POINT_ADDED_SIG);
    sig->asyncEmit(pointToAdd);
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::clearVisualizePointList()
{
    ::fwData::PointList::sptr visualizePointList = this->getInOut< ::fwData::PointList>(S_SELECTED_POINTS_KEY);
    SLM_ASSERT( S_SELECTED_POINTS_KEY + " doesn't exist or is not a pointList", visualizePointList);

    if(visualizePointList && visualizePointList->getPoints().size() > 0)
    {
        // Messages to notify that points of visualizePointList are removed
        for(int i = 0; i < visualizePointList->getPoints().size(); i++)
        {
            ::fwData::Point::sptr point = visualizePointList->getPoints()[i];

            auto sig = visualizePointList->signal< ::fwData::PointList::PointRemovedSignalType >(
                ::fwData::PointList::s_POINT_REMOVED_SIG);
            sig->asyncEmit(point);
        }

        // Clears the pointList
        visualizePointList->getPoints().clear();
    }
}

//------------------------------------------------------------------------------

void SSplinePointsEditor::updatePointList()
{
    ::fwData::PointList::sptr pointList = this->getInOut< ::fwData::PointList>(S_POINTS_KEY);

    auto sig = pointList->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

} // namespace uiSpline
