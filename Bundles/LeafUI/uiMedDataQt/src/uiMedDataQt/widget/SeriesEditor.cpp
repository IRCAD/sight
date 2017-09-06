/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMedDataQt/widget/SeriesEditor.hpp"

#include "uiMedDataQt/constants.hpp"
#include "uiMedDataQt/InsertSeries.hpp"

#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>

#include <boost/algorithm/string.hpp>

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace uiMedDataQt
{
namespace widget
{

//-----------------------------------------------------------------------------

ListInput::ListInput()
{
    m_physicianInput     = new QLineEdit();
    m_addPhysicianButton = new QPushButton(tr("+"));
    m_addPhysicianButton->setEnabled(false);
    m_removePhysicianButton = new QPushButton(tr("-"));
    m_removePhysicianButton->setEnabled(false);
    m_performingPhysiciansName = new QListWidget();

    QHBoxLayout* inputLayout = new QHBoxLayout();
    inputLayout->addWidget(m_physicianInput);
    inputLayout->addWidget(m_addPhysicianButton);
    inputLayout->addWidget(m_removePhysicianButton);
    QVBoxLayout* vLayout = new QVBoxLayout();

    vLayout->addLayout(inputLayout);
    vLayout->addWidget(m_performingPhysiciansName);

    QObject::connect(m_physicianInput, SIGNAL(textChanged(const QString &)),
                     this, SLOT(onInputChanged(const QString &)));
    QObject::connect(m_addPhysicianButton, SIGNAL(clicked()),
                     this, SLOT(onAddPhysician()));
    QObject::connect(m_removePhysicianButton, SIGNAL(clicked()),
                     this, SLOT(onRemovePhysician()));
    QObject::connect(m_performingPhysiciansName, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged()));
    this->setLayout(vLayout);
}

//-----------------------------------------------------------------------------

void ListInput::onInputChanged(const QString& text)
{
    m_addPhysicianButton->setEnabled(m_physicianInput->text().count() > 0);
}

//-----------------------------------------------------------------------------

void ListInput::onAddPhysician()
{
    m_performingPhysiciansName->addItem(new QListWidgetItem(m_physicianInput->text().trimmed()));
    m_physicianInput->clear();
}

//-----------------------------------------------------------------------------

void ListInput::onRemovePhysician()
{
    QList< QListWidgetItem* > selectedItems = m_performingPhysiciansName->selectedItems();
    for(int i = 0; i < selectedItems.count(); ++i)
    {
        delete selectedItems.at(i);
    }
}

//-----------------------------------------------------------------------------

void ListInput::onSelectionChanged()
{
    m_removePhysicianButton->setEnabled(m_performingPhysiciansName->selectedItems().count() > 0);
}
//-----------------------------------------------------------------------------

SeriesEditor::SeriesEditor(QWidget* parent) :
    QWidget(parent)
{
    m_modality    = new QLineEdit();
    m_date        = new QLineEdit();
    m_time        = new QLineEdit();
    m_description = new QLineEdit();
    m_physicians  = new ListInput();

    QFormLayout* layout = new QFormLayout();

    layout->addRow(tr("Modality"), m_modality);
    layout->addRow(tr("Date"), m_date);
    layout->addRow(tr("Time"), m_time);
    layout->addRow(tr("Description"), m_description);

    layout->addRow(tr("Physicians"), m_physicians);

    QObject::connect(m_date, SIGNAL(textChanged(const QString &)), this, SLOT(onDateChanged(const QString &)));
    QObject::connect(m_time, SIGNAL(textChanged(const QString &)), this, SLOT(onTimeChanged(const QString &)));
    QObject::connect(m_description, SIGNAL(textChanged(const QString &)), this, SLOT(onDescChanged(const QString &)));

    QGroupBox* group = new QGroupBox(tr("Series"));
    group->setLayout(layout);
    QVBoxLayout* topLayout = new QVBoxLayout();
    topLayout->addWidget(group);
    this->setLayout(topLayout);
}

//-----------------------------------------------------------------------------

SeriesEditor::~SeriesEditor()
{
    QObject::disconnect(m_date, SIGNAL(textChanged(const QString &)), this, SLOT(onDateChanged(const QString &)));
    QObject::disconnect(m_time, SIGNAL(textChanged(const QString &)), this, SLOT(onTimeChanged(const QString &)));
    QObject::disconnect(m_description, SIGNAL(textChanged(const QString &)), this,
                        SLOT(onDescChanged(const QString &)));
}

//-----------------------------------------------------------------------------

void SeriesEditor::onDateChanged(const QString& text)
{
    if(m_date->text().trimmed().isEmpty())
    {
        m_palette.setColor(QPalette::Base, QColor(Qt::red));
    }
    else
    {
        m_palette.setColor(QPalette::Base, QColor(Qt::white));
    }

    m_date->setPalette(m_palette);
}

//-----------------------------------------------------------------------------

void SeriesEditor::onTimeChanged(const QString& text)
{
    if(m_time->text().trimmed().isEmpty())
    {
        m_palette.setColor(QPalette::Base, QColor(Qt::red));
    }
    else
    {
        m_palette.setColor(QPalette::Base, QColor(Qt::white));
    }

    m_time->setPalette(m_palette);
}

//-----------------------------------------------------------------------------

void SeriesEditor::onDescChanged(const QString& text)
{
    if(m_description->text().trimmed().toStdString() == s_INSERT_NEW_SERIES_TEXT)
    {
        m_palette.setColor(QPalette::Base, QColor(Qt::red));
    }
    else
    {
        m_palette.setColor(QPalette::Base, QColor(Qt::white));
    }

    m_description->setPalette(m_palette);
}

//-----------------------------------------------------------------------------

void SeriesEditor::setSeries(SPTR(::fwMedData::Series)series)
{
    SLM_ASSERT("Given series is null", series);
    m_series = series;

    m_modality->setText(QString::fromStdString(m_series->getModality()).trimmed());
    m_description->setText(QString::fromStdString(m_series->getDescription()).trimmed());
    m_date->setText(QString::fromStdString(series->getDate()).trimmed());
    m_time->setText(QString::fromStdString(series->getTime()).trimmed());

    // force signal (manage empty text case)
    this->onTimeChanged(m_time->text());
    this->onDateChanged(m_date->text());

    m_physicians->getListWidget()->clear();
    for(::fwMedData::DicomValueType value :  m_series->getPerformingPhysiciansName())
    {
        QListWidgetItem* newVal
            = new QListWidgetItem(QString::fromStdString(value).trimmed(), m_physicians->getListWidget());
    }
}

//-----------------------------------------------------------------------------

SPTR(::fwMedData::Series) SeriesEditor::getSeries()
{
    ::fwMedData::Series::sptr series = ::uiMedDataQt::InsertSeries::New();
    series->setModality(m_modality->text().trimmed().toStdString());
    series->setDate(m_date->text().trimmed().toStdString());
    series->setTime(m_time->text().trimmed().toStdString());
    series->setDescription(m_description->text().trimmed().toStdString());

    ::fwMedData::DicomValuesType newPerformingPhysiciansName;
    for(int i = 0; i < m_physicians->getListWidget()->count(); ++i)
    {
        newPerformingPhysiciansName.push_back(m_physicians->getListWidget()->item(i)->text().trimmed().toStdString());
    }

    series->setPerformingPhysiciansName(newPerformingPhysiciansName);

    return series;
}

//-----------------------------------------------------------------------------

bool SeriesEditor::isValid() const
{
    return !m_date->text().trimmed().isEmpty()
           && !m_time->text().trimmed().isEmpty()
           && m_description->text().trimmed().toStdString() != s_INSERT_NEW_SERIES_TEXT;
}

//-----------------------------------------------------------------------------

} // namespace widget
} // namespace uiMedDataQt

