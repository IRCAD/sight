/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCalibration/action/SCalibrationConfiguration.hpp"

#include <boost/lexical_cast.hpp>

#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/Vector.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <QDialog>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

#include <string>

namespace videoCalibration
{

namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::videoCalibration::action::SCalibrationConfiguration,
                         ::fwData::Composite );

const ::fwCom::Signals::SignalKeyType SCalibrationConfiguration::s_UPDATED_CHESSBOARD_SIZE_SIG =
    "updatedChessboardSize";
//-----------------------------------------------------------------------------

SCalibrationConfiguration::SCalibrationConfiguration() throw() : m_chessboardWidthEditor(new QSpinBox()),
                                                                 m_chessboardHeightEditor(new QSpinBox()),
                                                                 m_squareSizeChessboardEditor(new QDoubleSpinBox())
{
    m_chessboardWidthEditor->setRange(0, 100);
    m_chessboardHeightEditor->setRange(0, 100);
    m_squareSizeChessboardEditor->setRange(0, 500);

    m_sigUpdatedChessboardSize = newSignal< updatedChessboardSizeSignalType >(s_UPDATED_CHESSBOARD_SIZE_SIG);
}

//------------------------------------------------------------------------------

SCalibrationConfiguration::~SCalibrationConfiguration() throw()
{
}

//------------------------------------------------------------------------------

void SCalibrationConfiguration::starting() throw(::fwTools::Failed)
{
    this->actionServiceStarting();
}

//------------------------------------------------------------------------------

void SCalibrationConfiguration::stopping() throw(::fwTools::Failed)
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SCalibrationConfiguration::configuring() throw(fwTools::Failed)
{
    this->initialize();
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    ::fwServices::IService::ConfigType configuration = this->getConfigTree();

    SLM_ASSERT("Sorry you must have one (and only one) <config/> element.",
               this->getConfigTree().get_child("service").count("config") == 1 );

    const ::fwServices::IService::ConfigType srvconfig = configuration.get_child("service");
    const ::fwServices::IService::ConfigType &config   = srvconfig.get_child("config");

    std::string paramKey = config.get<std::string>("parametersKey");
    SLM_ASSERT("Missing 'parametersKey'", !paramKey.empty());

    m_paramComp = composite->at< ::fwData::Composite >(paramKey);
    SLM_ASSERT("Missing parameters composite: " + paramKey, m_paramComp);

}

//------------------------------------------------------------------------------

void SCalibrationConfiguration::updating() throw(::fwTools::Failed)
{

    ::fwData::Object::sptr width      = m_paramComp->getContainer()["chessboardWidth"];
    ::fwData::Object::sptr height     = m_paramComp->getContainer()["chessboardHeight"];
    ::fwData::Object::sptr squareSize = m_paramComp->getContainer()["squareSizeChessboard"];

    int chessboardWidth        = std::stoi(::fwData::String::dynamicCast(width)->value());
    int chessboardHeight       = std::stoi( ::fwData::String::dynamicCast(height)->value());
    float squareSizeChessboard = std::stof(::fwData::String::dynamicCast(squareSize)->value());

    m_chessboardWidthEditor->setValue(chessboardWidth);
    m_chessboardHeightEditor->setValue(chessboardHeight);
    m_squareSizeChessboardEditor->setValue(squareSizeChessboard);

    QPointer<QDialog> dialog     = new QDialog();
    QPointer<QGridLayout> layout = new QGridLayout();

    QPointer<QLabel> chessboardHeightLabel     = new QLabel(QObject::tr("Chessboard height: "));
    QPointer<QLabel> chessboardWidthLabel      = new QLabel(QObject::tr("Chessboard width: "));
    QPointer<QLabel> squareSizeChessboardLabel = new QLabel(QObject::tr("Square size Chessboard: "));
    QLabel squareSizeUnity(QObject::tr("mm"));

    layout->addWidget(chessboardHeightLabel, 0, 0);
    layout->addWidget(m_chessboardHeightEditor, 0, 1);

    layout->addWidget(chessboardWidthLabel, 1, 0);
    layout->addWidget(m_chessboardWidthEditor, 1, 1);

    layout->addWidget(squareSizeChessboardLabel, 2, 0);
    layout->addWidget(m_squareSizeChessboardEditor, 2, 1);
    layout->addWidget(&squareSizeUnity, 2, 2);

    QPointer<QPushButton> cancelButton = new QPushButton("Cancel");
    QPointer<QPushButton> okButton     = new QPushButton("OK");
    okButton->setDefault(true);

    QPointer<QHBoxLayout> buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(okButton);
    layout->addLayout(buttonLayout, 3, 1, 3, 2);

    QObject::connect(cancelButton.data(), &QPushButton::clicked, dialog.data(), &QDialog::reject);
    QObject::connect(okButton.data(), &QPushButton::clicked, dialog.data(), &QDialog::accept);

    dialog->setLayout(layout);

    if (dialog->exec() == QDialog::Accepted)
    {
        ::fwData::String::sptr width      = ::fwData::String::New( std::to_string(m_chessboardWidthEditor->value()));
        ::fwData::String::sptr height     = ::fwData::String::New( std::to_string(m_chessboardHeightEditor->value()));
        ::fwData::String::sptr squareSize =
            ::fwData::String::New( std::to_string(m_squareSizeChessboardEditor->value()));

        (::fwData::String::dynamicCast(m_paramComp->getContainer()["chessboardWidth"]))->setValue(width->value());
        (::fwData::String::dynamicCast(m_paramComp->getContainer()["chessboardHeight"]))->setValue(height->value());
        (::fwData::String::dynamicCast(
             m_paramComp->getContainer()["squareSizeChessboard"]))->setValue(squareSize->value());

        m_sigUpdatedChessboardSize->asyncEmit(m_chessboardWidthEditor->value(), m_chessboardHeightEditor->value(),
                                              m_squareSizeChessboardEditor->value());
    }
}

//------------------------------------------------------------------------------

void SCalibrationConfiguration::swapping() throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void SCalibrationConfiguration::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace videoCalibration
