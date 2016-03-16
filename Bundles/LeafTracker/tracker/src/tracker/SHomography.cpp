/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "tracker/SHomography.hpp"

#include <arData/Camera.hpp>
#include <extData/FrameTL.hpp>
#include <arData/MarkerTL.hpp>
#include <extData/MatrixTL.hpp>

#include <fwData/TransformationMatrix3D.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <arlcore/MatrixR.h>

#include <boost/container/vector.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <limits>

fwServicesRegisterMacro(::fwServices::IController, ::tracker::SHomography, ::fwData::Composite);


//-----------------------------------------------------------------------------

namespace tracker
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SHomography::s_REGISTER_SLOT = "register";

//-----------------------------------------------------------------------------

SHomography::SHomography() throw () :
    m_lastTimestamp(0),
    m_patternWidth(80),
    m_isCamInitialized(false),
    m_planeSystem(NULL)
{
    m_slotRegister = ::fwCom::newSlot(&SHomography::doRegistration, this);
    ::fwCom::HasSlots::m_slots(s_REGISTER_SLOT, m_slotRegister);

    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );

    m_connections = ::fwServices::helper::SigSlotConnection::New();
}

//-----------------------------------------------------------------------------

SHomography::~SHomography() throw ()
{
}

//-----------------------------------------------------------------------------

void SHomography::configuring() throw (::fwTools::Failed)
{
    typedef ::fwRuntime::ConfigurationElementContainer::Container CfgContainer;
    ::fwRuntime::ConfigurationElement::sptr cfg = m_configuration->findConfigurationElement("config");
    SLM_ASSERT("Tag 'config' not found.", cfg);

    {
        ::fwRuntime::ConfigurationElement::sptr cfgMarkerTL = cfg->findConfigurationElement("markerTL");
        SLM_ASSERT("Tag 'markerTL' not found.", cfgMarkerTL);
        for(const CfgContainer::value_type& elt : cfgMarkerTL->getElements())
        {
            m_markerTLKeys.push_back(elt->getValue());
        }
    }

    {
        ::fwRuntime::ConfigurationElement::sptr cfgCamera = cfg->findConfigurationElement("camera");
        SLM_ASSERT("Tag 'camera' not found.", cfgCamera);
        for(const CfgContainer::value_type& elt : cfgCamera->getElements())
        {
            m_cameraKeys.push_back(elt->getValue());
        }
    }

    SLM_ASSERT("Not same number of markers and cameras", !m_markerTLKeys.empty() &&
               m_markerTLKeys.size() == m_cameraKeys.size());

    ::fwRuntime::ConfigurationElement::sptr cfgExtrinsic = cfg->findConfigurationElement("extrinsic");
    SLM_ASSERT("Tag 'extrinsic' not found.",
               (!cfgExtrinsic && m_cameraKeys.size() == 1) || (cfgExtrinsic && m_cameraKeys.size() > 1));
    if (cfgExtrinsic)
    {
        m_extrinsicKey = cfgExtrinsic->getValue();
    }

    // gets matrix timeline
    {
        ::fwRuntime::ConfigurationElement::sptr cfgMatrixTL = cfg->findConfigurationElement("matrixTL");
        SLM_ASSERT("Tag 'matrixTL' not found.", cfgMatrixTL);
        m_matrixKey = cfgMatrixTL->getValue();
    }

    // gets pattern width
    ::fwRuntime::ConfigurationElement::sptr cfgPatternWidth = cfg->findConfigurationElement("patternWidth");
    if (cfgPatternWidth)
    {
        m_patternWidth = ::boost::lexical_cast< double >(cfgPatternWidth->getValue());
    }
}

//-----------------------------------------------------------------------------

void SHomography::starting() throw (::fwTools::Failed)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);
    ::fwData::Composite::sptr comp = this->getObject< ::fwData::Composite >();

    //3D Points
    const double halfWidth = m_patternWidth * .5;

    m_3dModel.push_back(::arlCore::Point::PointFactory(-halfWidth, halfWidth, 0));
    m_3dModel.push_back(::arlCore::Point::PointFactory(halfWidth, halfWidth, 0));
    m_3dModel.push_back(::arlCore::Point::PointFactory(halfWidth, -halfWidth, 0));
    m_3dModel.push_back(::arlCore::Point::PointFactory(-halfWidth, -halfWidth, 0));

    ::arData::MarkerTL::sptr firstTimeline = comp->at< ::arData::MarkerTL >(m_markerTLKeys[0]);

    const unsigned int maxElementNum = firstTimeline->getMaxElementNum();

    for(const VectKeyType::value_type& elt : m_markerTLKeys)
    {
        ::arData::MarkerTL::sptr timeline = comp->at< ::arData::MarkerTL >(elt);
        m_connections->connect(timeline, ::extData::TimeLine::s_OBJECT_PUSHED_SIG, this->getSptr(),
                               ::tracker::SHomography::s_REGISTER_SLOT);

        SLM_ASSERT("Timelines should have the same maximum number of elements",
                   maxElementNum == timeline->getMaxElementNum());
    }

    ::extData::MatrixTL::sptr matrixTL = comp->at< ::extData::MatrixTL >(m_matrixKey);
    // initialized matrix timeline
    matrixTL->initPoolSize(maxElementNum);
}

//-----------------------------------------------------------------------------

void SHomography::stopping() throw (::fwTools::Failed)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);

    for(const ::arlCore::Camera* cam : m_arlCameras)
    {
        delete cam;
    }
    delete m_planeSystem;
    m_arlCameras.clear();
    m_3dModel.clear();
    m_connections->disconnect();
    m_lastTimestamp    = 0;
    m_isCamInitialized = false;
}

//-----------------------------------------------------------------------------

void SHomography::updating() throw (::fwTools::Failed)
{
}

//-----------------------------------------------------------------------------

void SHomography::doRegistration(::fwCore::HiResClock::HiResClockType timestamp)
{
    ::fwCore::mt::ScopedLock lock(m_mutex);

    SLM_WARN_IF("Invoking doRegistration while service is STOPPED", this->isStopped() );

    if(!m_isCamInitialized)
    {
        this->initARLCameras();
    }

    if (this->isStarted())
    {
        if (timestamp > m_lastTimestamp)
        {
            ::fwData::Composite::sptr comp = this->getObject< ::fwData::Composite >();

            ::fwCore::HiResClock::HiResClockType newerTimestamp =
                std::numeric_limits< ::fwCore::HiResClock::HiResClockType >::max();
            for(const VectKeyType::value_type& elt : m_markerTLKeys)
            {
                ::arData::MarkerTL::sptr markerTL              = comp->at< ::arData::MarkerTL >(elt);
                ::fwCore::HiResClock::HiResClockType timestamp = markerTL->getNewerTimestamp();
                if(timestamp == 0)
                {
                    OSLM_WARN("No marker found in a timeline for timestamp '"<<timestamp<<"'.");
                    return;
                }
                newerTimestamp = std::min(timestamp, newerTimestamp);
            }

            m_lastTimestamp = newerTimestamp;

            // We consider that all timeline have the same number of elements
            // This is WRONG if we have more than two cameras
            ::arData::MarkerTL::sptr markerTL = comp->at< ::arData::MarkerTL >(
                m_markerTLKeys[0]);
            const CSPTR(::arData::MarkerTL::BufferType) buffer = markerTL->getClosestBuffer(newerTimestamp);
            const unsigned int numMarkers = buffer->getMaxElementNum();

            ::extData::MatrixTL::sptr matrixTL = comp->at< ::extData::MatrixTL >(m_matrixKey);

            // Push matrix in timeline
            SPTR(::extData::MatrixTL::BufferType) matrixBuf;

            // For each marker
            bool matrixBufferCreated = false;
            for(unsigned int markerIndex = 0; markerIndex < numMarkers; ++markerIndex)
            {
                std::vector< ARLPointListType > markerPts;
                std::vector< const ::arlCore::Camera* > arlCameras;
                size_t indexTL = 0;

                // For each camera timeline
                for(const VectKeyType::value_type& elt : m_markerTLKeys)
                {
                    ::arData::MarkerTL::sptr markerTL                  = comp->at< ::arData::MarkerTL >(elt);
                    const CSPTR(::arData::MarkerTL::BufferType) buffer = markerTL->getClosestBuffer(newerTimestamp);

                    if(buffer->isPresent(markerIndex))
                    {
                        const float* trackerBuffer = buffer->getElement(markerIndex);

                        ARLPointListType pl;
                        for(size_t i = 0; i < 4; ++i)
                        {
                            pl.push_back(::arlCore::Point::PointFactory(trackerBuffer[i*2], trackerBuffer[i*2+1]));
                        }
                        markerPts.push_back(pl);
                        arlCameras.push_back(m_arlCameras[indexTL]);
                    }
                    ++indexTL;
                }

                if(markerPts.empty())
                {
                    continue;
                }

                ::arlCore::vnl_rigid_matrix vnlMatrix;
                std::vector< double > optimiser_parameters(1,1), log;

                if (!::arlCore::planarHomographyRegistration_3D_2D(*arlCameras.front(), markerPts.front(), m_3dModel,
                                                                   vnlMatrix, optimiser_parameters, log, false))
                {

                    SLM_WARN("Unable to compute planar homography registration.");
                }

                ::arlCore::multiViewPointRegistration3D2D(arlCameras, markerPts, m_3dModel, vnlMatrix,
                                                          ::arlCore::ARLCORE_PR_ISPPC, optimiser_parameters, log,
                                                          false);

                if(!matrixBufferCreated)
                {
                    matrixBuf = matrixTL->createBuffer(newerTimestamp);
                    matrixTL->pushObject(matrixBuf);
                    matrixBufferCreated = true;
                }

                float matrixValues[16];

                for (unsigned int i = 0; i < 4; ++i)
                {
                    for (unsigned int j = 0; j < 4; ++j)
                    {
                        matrixValues[4*i+j] = static_cast<float>(vnlMatrix[i][j]);
                    }
                }

                matrixBuf->setElement(matrixValues, markerIndex);
            }

            if(matrixBufferCreated)
            {
                //Notify
                ::extData::TimeLine::ObjectPushedSignalType::sptr sig;
                sig = matrixTL->signal< ::extData::TimeLine::ObjectPushedSignalType >(
                    ::extData::TimeLine::s_OBJECT_PUSHED_SIG );
                sig->asyncEmit(timestamp);

            }
        }
    }
}

//-----------------------------------------------------------------------------

void SHomography::initARLCameras()
{
    m_planeSystem                  = new ::arlCore::PlaneSystem();
    ::fwData::Composite::sptr comp = this->getObject< ::fwData::Composite >();
    unsigned int count = 0;

    for(const VectKeyType::value_type& camKey : m_cameraKeys)
    {
        ++count;
        ::arData::Camera::sptr camera = comp->at< ::arData::Camera >(camKey);
        OSLM_FATAL_IF("Camera with key '" << camKey << "' not found", !camera);

        ::arlCore::Camera* arlCamera = new ::arlCore::Camera(*m_planeSystem);

        arlCamera->init();

        arlCamera->setcx(camera->getCx());
        arlCamera->setcy(camera->getCy());
        arlCamera->setfx(camera->getFx());
        arlCamera->setfy(camera->getFy());
        ::arData::Camera::DistArrayType dist = camera->getDistortionCoefficient();
        for (unsigned int i = 0; i < 5; ++i)
        {
            arlCamera->setkc(i, dist[i]);
        }
        arlCamera->setAlphaC(camera->getSkew());

        ::arlCore::vnl_rigid_matrix matrix;

        // set extrinsic matrix only for camera 2
        if (count == 2)
        {
            ::fwData::TransformationMatrix3D::sptr extrinsicMatrix =
                comp->at< ::fwData::TransformationMatrix3D >(m_extrinsicKey);

            OSLM_FATAL_IF("Extrinsic matrix with key '" << m_extrinsicKey << "' not found", !extrinsicMatrix);

            for (unsigned int i = 0; i < 4; ++i)
            {
                for (unsigned int j = 0; j < 4; ++j)
                {
                    matrix[i][j] = extrinsicMatrix->getCoefficient(i, j);
                }
            }
        }
        arlCamera->setExtrinsic(matrix);
        m_arlCameras.push_back(arlCamera);
    }
    m_isCamInitialized = true;
}

//-----------------------------------------------------------------------------

} // namespace tracker

