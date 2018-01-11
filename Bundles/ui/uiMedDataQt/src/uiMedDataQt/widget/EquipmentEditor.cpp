/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMedDataQt/widget/EquipmentEditor.hpp"

#include <fwMedData/Equipment.hpp>
#include <fwMedData/Series.hpp>

#include <boost/algorithm/string.hpp>

#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>

namespace uiMedDataQt
{
namespace widget
{

//-----------------------------------------------------------------------------

EquipmentEditor::EquipmentEditor(QWidget* parent) :
    QWidget(parent)
{
    m_institutionName = new QLineEdit();

    QFormLayout* layout = new QFormLayout();

    layout->addRow(tr("Institution name"), m_institutionName);

    QGroupBox* group = new QGroupBox(tr("Equipment"));
    group->setLayout(layout);
    QVBoxLayout* topLayout = new QVBoxLayout();
    topLayout->addWidget(group);
    this->setLayout(topLayout);
}

//-----------------------------------------------------------------------------

EquipmentEditor::~EquipmentEditor()
{
}

//-----------------------------------------------------------------------------

void EquipmentEditor::setSeries(SPTR(::fwMedData::Series)series)
{
    SLM_ASSERT("Given series is null", series);
    m_series = series;

    ::fwMedData::Equipment::sptr equipment = m_series->getEquipment();
    SLM_ASSERT("Given series equipment is null", equipment);

    m_institutionName->setText(QString::fromStdString(equipment->getInstitutionName()).trimmed());

}

SPTR(::fwMedData::Equipment) EquipmentEditor::getEquipment() const
{
    ::fwMedData::Equipment::sptr equipment = ::fwMedData::Equipment::New();
    equipment->setInstitutionName(m_institutionName->text().trimmed().toStdString());

    return equipment;
}

//------------------------------------------------------------------------------

bool EquipmentEditor::isValid() const
{
    //TODO
    return true;
}

//-----------------------------------------------------------------------------

} // namespace widget
} // namespace uiMedDataQt

