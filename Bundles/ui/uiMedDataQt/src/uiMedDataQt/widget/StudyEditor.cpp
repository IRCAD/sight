/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMedDataQt/widget/StudyEditor.hpp"

#include "uiMedDataQt/constants.hpp"
#include "uiMedDataQt/InsertSeries.hpp"

#include <fwData/String.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/Study.hpp>

#include <fwMedDataTools/functions.hpp>

#include <boost/algorithm/string.hpp>

#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>

namespace uiMedDataQt
{
namespace widget
{

//-----------------------------------------------------------------------------

StudyEditor::StudyEditor(QWidget* parent) :
    QWidget(parent)
{
    m_date                   = new QLineEdit();
    m_time                   = new QLineEdit();
    m_referringPhysicianName = new QLineEdit();
    m_description            = new QLineEdit();
    m_patientAge             = new QLineEdit();

    QFormLayout* layout = new QFormLayout();

    layout->addRow(tr("Date"), m_date);
    layout->addRow(tr("Time"), m_time);
    layout->addRow(tr("Referring physician name"), m_referringPhysicianName);
    layout->addRow(tr("Description"), m_description);
    layout->addRow(tr("Patient age"), m_patientAge);

    QObject::connect(m_date, SIGNAL(textChanged(const QString&)), this, SLOT(onDateChanged(const QString&)));
    QObject::connect(m_time, SIGNAL(textChanged(const QString&)), this, SLOT(onTimeChanged(const QString&)));
    QObject::connect(m_description, SIGNAL(textChanged(const QString&)), this, SLOT(onDescChanged(const QString&)));

    QGroupBox* group = new QGroupBox(tr("Study"));
    group->setLayout(layout);
    QVBoxLayout* topLayout = new QVBoxLayout();
    topLayout->addWidget(group);
    this->setLayout(topLayout);
}

//-----------------------------------------------------------------------------

StudyEditor::~StudyEditor()
{
    QObject::disconnect(m_date, SIGNAL(textChanged(const QString&)), this, SLOT(onDateChanged(const QString&)));
    QObject::disconnect(m_time, SIGNAL(textChanged(const QString&)), this, SLOT(onTimeChanged(const QString&)));
    QObject::disconnect(m_description, SIGNAL(textChanged(const QString&)), this,
                        SLOT(onDescChanged(const QString&)));
}

//-----------------------------------------------------------------------------

bool StudyEditor::isValid() const
{
    return !m_date->text().trimmed().isEmpty()
           && m_description->text().trimmed().toStdString() != s_NEW_STUDY_TEXT
           && !m_time->text().trimmed().isEmpty();
}

//-----------------------------------------------------------------------------

SPTR(::fwMedData::Study) StudyEditor::getStudy()
{
    ::fwMedData::Study::sptr srcStudy = m_series->getStudy();
    ::fwMedData::Study::sptr study    = ::fwMedData::Study::New();
    study->setDescription(m_description->text().trimmed().toStdString());
    study->setDate(m_date->text().trimmed().toStdString());
    study->setTime(m_time->text().trimmed().toStdString());
    study->setReferringPhysicianName(m_referringPhysicianName->text().trimmed().toStdString());
    study->setInstanceUID(srcStudy->getInstanceUID());
    study->setPatientAge(srcStudy->getPatientAge());

    std::string date = srcStudy->getDate();
    ::boost::algorithm::trim(date);
    std::string time = srcStudy->getTime();
    ::boost::algorithm::trim(time);
    std::string refP = srcStudy->getReferringPhysicianName();
    ::boost::algorithm::trim(refP);
    std::string age = srcStudy->getPatientAge();
    ::boost::algorithm::trim(age);

    // Study description comparison ommitted here
    const bool same = study->getDate() == date
                      && study->getTime() == time
                      && study->getReferringPhysicianName() == refP
                      && study->getInstanceUID() == srcStudy->getInstanceUID()
                      && study->getPatientAge() == age;

    ::fwData::String::sptr fieldNewStudy = srcStudy->getField< ::fwData::String>(s_NEW_STUDY_FIELD_NAME);
    if(fieldNewStudy || !same)
    {
        study->setInstanceUID(::fwMedDataTools::generateStudyInstanceUid());
    }

    return study;
}

//-----------------------------------------------------------------------------

void StudyEditor::onDateChanged(const QString& text)
{
    if(m_date->text().trimmed().isEmpty())
    {
        m_paletteDate.setColor(QPalette::Base, QColor(Qt::red));
    }
    else
    {
        m_paletteDate.setColor(QPalette::Base, QColor(Qt::white));
    }

    m_date->setPalette(m_paletteDate);
}

//-----------------------------------------------------------------------------

void StudyEditor::onTimeChanged(const QString& text)
{
    if(m_time->text().trimmed().isEmpty())
    {
        m_paletteTime.setColor(QPalette::Base, QColor(Qt::red));
    }
    else
    {
        m_paletteTime.setColor(QPalette::Base, QColor(Qt::white));
    }

    m_time->setPalette(m_paletteTime);
}

//-----------------------------------------------------------------------------

void StudyEditor::onDescChanged(const QString& text)
{
    if(m_description->text().trimmed().toStdString() == s_NEW_STUDY_TEXT)
    {
        m_paletteTime.setColor(QPalette::Base, QColor(Qt::red));
    }
    else
    {
        m_paletteTime.setColor(QPalette::Base, QColor(Qt::white));
    }

    m_description->setPalette(m_paletteTime);
}

//-----------------------------------------------------------------------------

void StudyEditor::setSeries(SPTR(::fwMedData::Series)series)
{
    SLM_ASSERT("Given series is null", series);
    m_series = series;

    ::fwMedData::Study::sptr study = m_series->getStudy();
    SLM_ASSERT("Given series study is null", study);

    m_date->setText(QString::fromStdString(study->getDate()).trimmed());
    m_time->setText(QString::fromStdString(study->getTime()).trimmed());
    m_referringPhysicianName->setText(QString::fromStdString(study->getReferringPhysicianName()).trimmed());
    m_description->setText(QString::fromStdString(study->getDescription()).trimmed());
    m_patientAge->setText(QString::fromStdString(study->getPatientAge()).trimmed());

    // force signal (manage empty text case)
    this->onTimeChanged(m_time->text());
    this->onDateChanged(m_date->text());

}

//-----------------------------------------------------------------------------

} // namespace widget
} // namespace uiMedDataQt
