/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiCalibration/SIntrinsicEdition.hpp"

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwThread/Worker.hpp>

#include <fwCore/base.hpp>

#include <arData/Camera.hpp>

#include <fwTools/Object.hpp>

#include <fwServices/macros.hpp>

namespace uiCalibration
{
fwServicesRegisterMacro( ::fwServices::IService, ::uiCalibration::SIntrinsicEdition, ::arData::Camera);

// -------------------------------------------------------------------------

SIntrinsicEdition::SIntrinsicEdition()
{
    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
    m_dialog = new SUpdateIntrinsicDialog();


    QObject::connect(m_dialog, SIGNAL(newCalibration(std::array< double, 12 >&)),
                     this, SLOT(onNewCalibration(std::array< double, 12 >&)));

}

// -------------------------------------------------------------------------

SIntrinsicEdition::~SIntrinsicEdition()
{
    QObject::disconnect(m_dialog, SIGNAL(newCalibration(std::array< double, 12 >&)),
                        this, SLOT(onNewCalibration(std::array< double, 12 >&)));
}

// -------------------------------------------------------------------------

void SIntrinsicEdition::onNewCalibration(std::array< double, 12 >& cal)
{
    m_calibration = cal;

    this->updateCalibration();
}


// -------------------------------------------------------------------------

void SIntrinsicEdition::updateCalibration()
{
    ::arData::Camera::sptr camera = this->getObject< ::arData::Camera >();

    camera->setWidth( m_calibration[0]);
    camera->setHeight(m_calibration[1]);

    camera->setFx(m_calibration[2]);
    camera->setFy(m_calibration[3]);
    camera->setCx(m_calibration[4]);
    camera->setCy(m_calibration[5]);

    camera->setDistortionCoefficient(m_calibration[6], m_calibration[7], m_calibration[8],
                                     m_calibration[9], m_calibration[10]);

    camera->setSkew(m_calibration[11]);


    ::arData::Camera::IntrinsicCalibratedSignalType::sptr sig;
    sig = camera->signal< ::arData::Camera::IntrinsicCalibratedSignalType >(
        ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG);

    sig->asyncEmit();
}

// -------------------------------------------------------------------------

void SIntrinsicEdition::configuring() throw (fwTools::Failed)
{
    this->readCalibration();

    m_dialog->setParameters(m_calibration);

}

// -------------------------------------------------------------------------

void SIntrinsicEdition::readCalibration()
{
    ::arData::Camera::sptr camera = this->getObject< ::arData::Camera >();

    m_calibration[0] = camera->getWidth();
    m_calibration[1] = camera->getHeight();

    m_calibration[2] = camera->getFx();
    m_calibration[3] = camera->getFy();
    m_calibration[4] = camera->getCx();
    m_calibration[5] = camera->getCy();

    m_distParameters = camera->getDistortionCoefficient();

    m_calibration[6]  = m_distParameters[0];
    m_calibration[7]  = m_distParameters[1];
    m_calibration[8]  = m_distParameters[2];
    m_calibration[9]  = m_distParameters[3];
    m_calibration[10] = m_distParameters[4];
    m_calibration[11] = camera->getSkew();
}

// -------------------------------------------------------------------------

void SIntrinsicEdition::starting() throw (fwTools::Failed)
{
}

// -------------------------------------------------------------------------

void SIntrinsicEdition::stopping() throw (fwTools::Failed)
{

}

// -------------------------------------------------------------------------

void SIntrinsicEdition::swapping() throw (::fwTools::Failed)
{
    this->stopping();
    this->starting();
}

// -------------------------------------------------------------------------

void SIntrinsicEdition::updating() throw (::fwTools::Failed)
{
    this->readCalibration();
    m_dialog->setParameters(m_calibration);
    m_dialog->show();
}

// -------------------------------------------------------------------------


} // uiCalibration
