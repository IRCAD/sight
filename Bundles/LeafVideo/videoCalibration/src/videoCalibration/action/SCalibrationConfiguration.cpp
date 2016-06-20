/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "videoCalibration/action/SCalibrationConfiguration.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>

#include <QDialog>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

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

}

//------------------------------------------------------------------------------

void SCalibrationConfiguration::updating() throw(::fwTools::Failed)
{
    ::fwData::Composite::csptr paramComp = this->getInput< ::fwData::Composite >("parameters");

    ::fwData::String::csptr width      = paramComp->at< ::fwData::String>("chessboardWidth");
    ::fwData::String::csptr height     = paramComp->at< ::fwData::String>("chessboardHeight");
    ::fwData::String::csptr squareSize = paramComp->at< ::fwData::String>("squareSizeChessboard");

    unsigned int chessboardWidth  = ::boost::lexical_cast<unsigned int>( width->value());
    unsigned int chessboardHeight = ::boost::lexical_cast<unsigned int>( height->value());
    float squareSizeChessboard    = ::boost::lexical_cast<float>( squareSize->value());

    m_chessboardWidthEditor->setValue( static_cast<int>(chessboardWidth));
    m_chessboardHeightEditor->setValue( static_cast<int>(chessboardHeight));
    m_squareSizeChessboardEditor->setValue(squareSizeChessboard);

    QPointer<QDialog> dialog     = new QDialog();
    QPointer<QGridLayout> layout = new QGridLayout();

    QPointer<QLabel> chessboardWidthLabel      = new QLabel(QObject::tr("Chessboard width: "));
    QPointer<QLabel> chessboardHeightLabel     = new QLabel(QObject::tr("Chessboard height: "));
    QPointer<QLabel> squareSizeChessboardLabel = new QLabel(QObject::tr("Square size Chessboard: "));
    QLabel squareSizeUnity(QObject::tr("mm"));

    layout->addWidget(chessboardWidthLabel, 0, 0);
    layout->addWidget(m_chessboardWidthEditor, 0, 1);

    layout->addWidget(chessboardHeightLabel, 1, 0);
    layout->addWidget(m_chessboardHeightEditor, 1, 1);

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

        (::fwData::String::dynamicCast(paramComp->getContainer().at("chessboardWidth")))->setValue(width->value());
        (::fwData::String::dynamicCast(paramComp->getContainer().at("chessboardHeight")))->setValue(height->value());
        (::fwData::String::dynamicCast(
             paramComp->getContainer().at("squareSizeChessboard")))->setValue(squareSize->value());

        m_sigUpdatedChessboardSize->asyncEmit(static_cast<unsigned int>(m_chessboardWidthEditor->value()),
                                              static_cast<unsigned int>(m_chessboardHeightEditor->value()),
                                              static_cast<float>(m_squareSizeChessboardEditor->value()));
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
