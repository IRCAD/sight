/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiViewPoints/SViewPointsEditor.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>

#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Vector.hpp>

#include <fwGui/dialog/InputDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Get.hpp>

#include <fwTools/fwID.hpp>

#include <QCheckBox>
#include <QGridLayout>
#include <QList>
#include <QListWidget>
#include <QPushButton>
#include <QString>

fwServicesRegisterMacro(::gui::editor::IEditor, ::uiViewPoints::SViewPointsEditor, ::fwData::Vector);

namespace uiViewPoints
{

const ::fwCom::Signals::SignalKeyType SViewPointsEditor::s_DIRECT_TARGET_CHANGED_SIG = "directTargetChanged";
const std::string SViewPointsEditor::s_FIELD_NAME                                    = "ViewPointName";

//------------------------------------------------------------------------------

SViewPointsEditor::SViewPointsEditor() throw() :
    m_nbViewPoints(0)
{
    m_sigDirectTargetChanged = DirectTargetChangedSignalType::New();
    ::fwCom::HasSignals::m_signals(s_DIRECT_TARGET_CHANGED_SIG, m_sigDirectTargetChanged);
}

//------------------------------------------------------------------------------

SViewPointsEditor::~SViewPointsEditor() throw()
{
}

//------------------------------------------------------------------------------

void SViewPointsEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();

    ::fwGuiQt::container::QtContainer::sptr qtContainer =
        ::fwGuiQt::container::QtContainer::dynamicCast(this->getContainer());

    m_addPointButton = new QPushButton(QObject::tr("Add view point"));

    m_renamePointButton = new QPushButton(QObject::tr("Rename view point"));
    m_renamePointButton->setEnabled(false);

    m_removePointButton = new QPushButton(QObject::tr("Remove view point"));
    m_removePointButton->setEnabled(false);

    m_removeAllPointsButton = new QPushButton(QObject::tr("Remove all"));
    m_removeAllPointsButton->setEnabled(false);

    m_viewPointsList = new QListWidget();
    m_viewPointsList->setSelectionMode(QAbstractItemView::SingleSelection);

    QObject::connect(m_addPointButton, SIGNAL(clicked()), this, SLOT(onClickAddPoint()));
    QObject::connect(m_renamePointButton, SIGNAL(clicked()), this, SLOT(onClickRenamePoint()));
    QObject::connect(m_removePointButton, SIGNAL(clicked()), this, SLOT(onClickRemovePoint()));
    QObject::connect(m_removeAllPointsButton, SIGNAL(clicked()), this, SLOT(onClickRemoveAllPoints()));
    QObject::connect(
        m_viewPointsList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
        this, SLOT(onDoubleClickItem(QListWidgetItem*)));
    QObject::connect(
        m_viewPointsList, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(onClickItem(QListWidgetItem*)));

    QGridLayout* layout = new QGridLayout();
    layout->addWidget(m_addPointButton, 0, 0, Qt::AlignTop);
    layout->addWidget(m_renamePointButton, 0, 1, Qt::AlignTop);
    layout->addWidget(m_removePointButton, 0, 2, Qt::AlignTop);
    layout->addWidget(m_removeAllPointsButton, 0, 3, Qt::AlignTop);
    layout->addWidget(m_viewPointsList, 1, 0, 1, 4);

    qtContainer->setLayout(layout);

    // Get the already existing points for export activity
    ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector>();
    SLM_ASSERT("Invalid vector object", vector);

    for(::fwData::Object::sptr object :  vector->getContainer())
    {
        ::fwData::TransformationMatrix3D::sptr matrix = ::fwData::TransformationMatrix3D::dynamicCast(object);
        SLM_ASSERT("Invalid matrix object", matrix);

        ::fwData::String::sptr text = ::fwData::String::dynamicCast(matrix->getField(s_FIELD_NAME));
        m_viewPointsList->addItem(new QListWidgetItem(QString::fromStdString(text->value())));
    }

    m_removeAllPointsButton->setEnabled(m_viewPointsList->count() > 0);
}

//------------------------------------------------------------------------------

void SViewPointsEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    this->destroy();
}

//------------------------------------------------------------------------------

void SViewPointsEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    const std::vector<ConfigurationType> transformConfig = m_configuration->find("transform");
    if(!transformConfig.empty())
    {
        SLM_ASSERT("UID attribute is missing", transformConfig.at(0)->hasAttribute("uid"));
        m_transformUID = transformConfig.at(0)->getAttributeValue("uid");
    }
}

//------------------------------------------------------------------------------

void SViewPointsEditor::updating() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SViewPointsEditor::onClickAddPoint()
{
    m_nbViewPoints++;
    QString name = QString("ViewPoint %1").arg(m_nbViewPoints);
    m_viewPointsList->addItem(new QListWidgetItem(name));
    const std::string name_stdstring = name.toStdString();

    if(::fwTools::fwID::exist(m_transformUID))
    {
        // Get the current matrix
        ::fwData::TransformationMatrix3D::sptr Currentmatrix =
            ::fwData::TransformationMatrix3D::dynamicCast(::fwTools::fwID::getObject(m_transformUID));

        // Copy the current matrix
        ::fwData::TransformationMatrix3D::sptr smatrix =
            ::fwData::TransformationMatrix3D::sptr(::fwData::TransformationMatrix3D::New());
        smatrix->deepCopy(Currentmatrix);
        smatrix->setField(s_FIELD_NAME, ::fwData::String::New(name_stdstring));
        ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector>();
        vector->getContainer().push_back(smatrix);
    }

    m_removeAllPointsButton->setEnabled(m_viewPointsList->count() > 0);
}

//------------------------------------------------------------------------------

void SViewPointsEditor::onClickRenamePoint()
{
    ::fwGui::dialog::InputDialog dialogWindows;
    dialogWindows.setTitle("Rename view point");
    dialogWindows.setMessage("Enter new name");

    QListWidgetItem* item = m_viewPointsList->selectedItems().takeFirst();
    std::string text      = dialogWindows.getInput();
    QString str           = QString::fromUtf8(text.c_str());

    if(text.size() != 0)
    {
        item->setText(str);
        ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector>();
        const int index = m_viewPointsList->row(item);
        vector->getContainer()[index]->setField(s_FIELD_NAME, ::fwData::String::New(text));
    }

    m_renamePointButton->setEnabled(false);
    m_removePointButton->setEnabled(false);
}

//------------------------------------------------------------------------------

void SViewPointsEditor::onClickRemovePoint()
{
    QListWidgetItem* item = m_viewPointsList->selectedItems().takeFirst();

    ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector>();
    vector->getContainer().erase( vector->getContainer().begin() + m_viewPointsList->row(item));

    m_viewPointsList->removeItemWidget(item);
    delete (item);

    m_renamePointButton->setEnabled(false);
    m_removePointButton->setEnabled(false);
}

//------------------------------------------------------------------------------

void SViewPointsEditor::onClickRemoveAllPoints()
{
    ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector>();
    vector->getContainer().clear();

    m_viewPointsList->clear();

    m_nbViewPoints = 0;
    m_renamePointButton->setEnabled(false);
    m_removePointButton->setEnabled(false);
    m_removeAllPointsButton->setEnabled(false);
}

//------------------------------------------------------------------------------

void SViewPointsEditor::onClickItem(QListWidgetItem* item)
{
    m_renamePointButton->setEnabled(true);
    m_removePointButton->setEnabled(true);
}

//------------------------------------------------------------------------------

void SViewPointsEditor::onDoubleClickItem(QListWidgetItem* item)
{
    // See if a viewpoint is double clicked and get the associated matrix
    std::string name = (item->text()).toStdString();
    ::fwData::Vector::sptr vector = this->getObject< ::fwData::Vector>();
    int pointIndex = m_viewPointsList->row(item);

    // Send the matrix with the signal m_sigDirectTargetChanged
    ::fwData::TransformationMatrix3D::sptr trf;
    trf = ::fwData::TransformationMatrix3D::dynamicCast(vector->getContainer()[pointIndex]);
    m_sigDirectTargetChanged->asyncEmit(trf);
}

} // namespace uiViewPoints

