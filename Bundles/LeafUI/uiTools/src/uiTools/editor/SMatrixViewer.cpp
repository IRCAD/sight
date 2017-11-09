/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiTools/editor/SMatrixViewer.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/TransformationMatrix3D.hpp>

#include <fwGuiQt/container/QtContainer.hpp>

#include <fwServices/macros.hpp>

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QString>
#include <QVBoxLayout>

namespace uiTools
{

namespace editor
{

static const ::fwServices::IService::KeyType s_MATRIX_INPUT = "matrix";

fwServicesRegisterMacro( ::gui::editor::IEditor, ::uiTools::editor::SMatrixViewer);

// ------------------------------------------------------------------------------

SMatrixViewer::SMatrixViewer() noexcept :
    m_title("matrix")
{
}

// ------------------------------------------------------------------------------

SMatrixViewer::~SMatrixViewer() noexcept
{

}

// ------------------------------------------------------------------------------

void SMatrixViewer::configuring()
{
    fwGui::IGuiContainerSrv::initialize();

    ::fwRuntime::ConfigurationElement::sptr configCfg = m_configuration->findConfigurationElement("title");
    if(configCfg)
    {
        m_title = configCfg->getValue();
    }
}

// ------------------------------------------------------------------------------

void SMatrixViewer::starting()
{
    fwGui::IGuiContainerSrv::create();
    fwGuiQt::container::QtContainer::sptr qtContainer = fwGuiQt::container::QtContainer::dynamicCast(getContainer());

    QBoxLayout* mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_description = new QLabel(QString::fromStdString(m_title));

    mainLayout->addWidget(m_description);

    QGridLayout* gridLayout = new QGridLayout();

    for (unsigned int i = 0; i < 4; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
        {
            QLabel* label = new QLabel("");
            m_matrixLabels.push_back(label);
            gridLayout->addWidget(label, i, j);
        }
    }
    mainLayout->addLayout(gridLayout);

    qtContainer->setLayout(mainLayout);

    this->updateFromMatrix();
}

// ------------------------------------------------------------------------------

void SMatrixViewer::stopping()
{

    this->destroy();
}

// ------------------------------------------------------------------------------

void SMatrixViewer::updating()
{
    this->updateFromMatrix();
}

// ------------------------------------------------------------------------------

void SMatrixViewer::updateFromMatrix()
{
    auto matrix = this->getInput< ::fwData::TransformationMatrix3D >(s_MATRIX_INPUT);
    for (unsigned int i = 0; i < 4; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
        {
            m_matrixLabels[i*4 + j]->setText(QString("%1").arg(matrix->getCoefficient(i, j)));
        }
    }
}

// ------------------------------------------------------------------------------

void SMatrixViewer::clearLabels()
{
    for (unsigned int i = 0; i < 4; ++i)
    {
        for (unsigned int j = 0; j < 4; ++j)
        {
            m_matrixLabels[i*4 + j]->setText(QString(""));
        }
    }
}

// ------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SMatrixViewer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_MATRIX_INPUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

// ------------------------------------------------------------------------------

}// namespace editor

} // namespace uiTools
