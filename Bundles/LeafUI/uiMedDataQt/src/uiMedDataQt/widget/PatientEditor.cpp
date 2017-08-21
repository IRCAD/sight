/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiMedDataQt/widget/PatientEditor.hpp"

#include "uiMedDataQt/constants.hpp"

#include <fwData/String.hpp>

#include <fwMedData/Patient.hpp>
#include <fwMedData/Series.hpp>

#include <fwMedDataTools/functions.hpp>

#include <boost/algorithm/string.hpp>

#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPalette>

namespace uiMedDataQt
{
namespace widget
{

//-----------------------------------------------------------------------------

PatientEditor::PatientEditor(QWidget* parent) :
    QWidget(parent)
{
    m_txtName = new QLineEdit();

    m_txtBirthdate = new QLineEdit();

    m_cbSex = new QComboBox();
    m_cbSex->addItem(QString::fromStdString("Male"));
    m_cbSex->addItem(QString::fromStdString("Female"));
    m_cbSex->addItem(QString::fromStdString("Unknown"));

    QFormLayout* layout = new QFormLayout();
    layout->addRow(tr("&Name"), m_txtName);
    layout->addRow(tr("&Birthdate"), m_txtBirthdate);
    layout->addRow(tr("&Sex"), m_cbSex);

    QObject::connect(m_txtName, SIGNAL(textChanged(const QString &)), this, SLOT(onNameChanged(const QString &)));
    QObject::connect(m_txtBirthdate, SIGNAL(textChanged(const QString &)), this, SLOT(onBirthChanged(const QString &)));

    QGroupBox* group = new QGroupBox(tr("Patient"));
    group->setLayout(layout);
    QVBoxLayout* topLayout = new QVBoxLayout();
    topLayout->addWidget(group);
    this->setLayout(topLayout);
}

//-----------------------------------------------------------------------------

PatientEditor::~PatientEditor()
{
    QObject::disconnect(m_txtName, SIGNAL(textChanged(QString)), this, SLOT(onNameChanged));
    QObject::disconnect(m_txtBirthdate, SIGNAL(textChanged(QString)), this, SLOT(onBirthdateChanged));
}

//-----------------------------------------------------------------------------

bool PatientEditor::isValid() const
{
    return !m_txtName->text().trimmed().isEmpty()
           && m_txtName->text().trimmed().toStdString() != s_NEW_PATIENT_TEXT
           && !m_txtBirthdate->text().trimmed().isEmpty();
}

//-----------------------------------------------------------------------------

void PatientEditor::onNameChanged(const QString& text)
{
    if(m_txtName->text().trimmed().isEmpty() || m_txtName->text().trimmed().toStdString() == s_NEW_PATIENT_TEXT)
    {
        m_paletteName.setColor(QPalette::Base, QColor(Qt::red));
    }
    else
    {
        m_paletteName.setColor(QPalette::Base, QColor(Qt::white));
    }

    m_txtName->setPalette(m_paletteName);
}

//-----------------------------------------------------------------------------

void PatientEditor::onBirthChanged(const QString& text)
{
    if(m_txtBirthdate->text().trimmed().isEmpty())
    {
        m_paletteBirthdate.setColor(QPalette::Base, QColor(Qt::red));
    }
    else
    {
        m_paletteBirthdate.setColor(QPalette::Base, QColor(Qt::white));
    }

    m_txtBirthdate->setPalette(m_paletteBirthdate);
}

//-----------------------------------------------------------------------------

SPTR(::fwMedData::Patient) PatientEditor::getPatient() const
{
    ::fwMedData::Patient::sptr srcPatient = m_series->getPatient();
    ::fwMedData::Patient::sptr patient    = ::fwMedData::Patient::New();
    patient->setName(m_txtName->text().trimmed().toStdString());
    patient->setBirthdate(m_txtBirthdate->text().trimmed().toStdString());
    patient->setPatientId(srcPatient->getPatientId());

    const std::string sex = m_cbSex->currentText().toStdString();
    if(sex == "Male")
    {
        patient->setSex("M");
    }
    else if(sex == "Female")
    {
        patient->setSex("F");
    }
    else
    {
        patient->setSex("O");
    }

    // Tell to not compare patient sex if value is not managed
    std::string srcPatientSex = srcPatient->getSex();
    ::boost::algorithm::trim(srcPatientSex);
    const bool compareSex = (srcPatientSex == "M" || srcPatientSex == "F" || srcPatientSex == "O");

    std::string name = patient->getName();
    ::boost::algorithm::trim(name);
    std::string birth = patient->getBirthdate();
    ::boost::algorithm::trim(birth);
    std::string sexStr = patient->getSex();
    ::boost::algorithm::trim(sexStr);

    const bool same = srcPatient->getName() == name
                      && srcPatient->getBirthdate() == birth
                      && (compareSex ? (srcPatientSex == sexStr) : true)
                      && srcPatient->getPatientId() == patient->getPatientId();

    ::fwData::String::sptr fieldPatient = srcPatient->getField< ::fwData::String >(s_NEW_PATIENT_FIELD_NAME);
    if(fieldPatient || !same)
    {
        patient->setPatientId(::fwMedDataTools::generatePatientId());
    }

    return patient;
}

//-----------------------------------------------------------------------------

void PatientEditor::setSeries(SPTR(::fwMedData::Series)series)
{
    SLM_ASSERT("Given series is null", series);
    m_series = series;

    ::fwMedData::Patient::sptr patient = m_series->getPatient();
    SLM_ASSERT("Given series patient is null", patient);

    m_txtName->setText(QString::fromStdString(patient->getName()).trimmed());
    m_txtBirthdate->setText(QString::fromStdString(patient->getBirthdate()).trimmed());

    // force signal (manage empty text case)
    this->onNameChanged(m_txtName->text());
    this->onBirthChanged(m_txtBirthdate->text());

    std::string sex = patient->getSex();
    ::boost::algorithm::trim(sex);

    if(sex == "M")
    {
        m_cbSex->setCurrentIndex(0);
    }
    else if(sex == "F")
    {
        m_cbSex->setCurrentIndex(1);
    }
    else
    {
        m_cbSex->setCurrentIndex(2);
        SLM_WARN_IF("Unknown patient sex for value '" + patient->getSex() + "'", patient->getSex() != "O");
    }
}

//-----------------------------------------------------------------------------

} // namespace widget
} // namespace uiMedDataQt

