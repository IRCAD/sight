/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "registrationARL/SPoseFrom2d.hpp"

#include <arData/Camera.hpp>
#include <arData/FrameTL.hpp>
#include <arData/MarkerTL.hpp>
#include <arData/MatrixTL.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/TransformationMatrix3D.hpp>

#include <arlcore/MatrixR.h>

fwServicesRegisterMacro(::fwServices::IRegisterer, ::registrationARL::SPoseFrom2d);

//-----------------------------------------------------------------------------

namespace registrationARL
{

//-----------------------------------------------------------------------------

const ::fwServices::IService::KeyType s_MARKERTL_INPUT  = "markerTL";
const ::fwServices::IService::KeyType s_CAMERA_INPUT    = "camera";
const ::fwServices::IService::KeyType s_EXTRINSIC_INPUT = "extrinsic";
const ::fwServices::IService::KeyType s_MATRIXTL_INOUT  = "matrixTL";

//-----------------------------------------------------------------------------

SPoseFrom2d::SPoseFrom2d() noexcept :
    m_lastTimestamp(0),
    m_patternWidth(80),
    m_isInitialized(false),
    m_planeSystem(NULL)
{

}

//-----------------------------------------------------------------------------

SPoseFrom2d::~SPoseFrom2d() noexcept
{
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::configuring()
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();
    m_patternWidth                            = config.get<double>("patternWidth", 80);
    OSLM_ASSERT("patternWidth setting is set to " << m_patternWidth << " but should be > 0.", m_patternWidth > 0);
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::starting()
{
    //3D Points
    const double halfWidth = m_patternWidth * .5;

    m_3dModel.push_back(::arlCore::Point::PointFactory(-halfWidth, halfWidth, 0));
    m_3dModel.push_back(::arlCore::Point::PointFactory(halfWidth, halfWidth, 0));
    m_3dModel.push_back(::arlCore::Point::PointFactory(halfWidth, -halfWidth, 0));
    m_3dModel.push_back(::arlCore::Point::PointFactory(-halfWidth, -halfWidth, 0));
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::stopping()
{
    for(const ::arlCore::Camera* cam : m_arlCameras)
    {
        delete cam;
    }
    delete m_planeSystem;
    m_arlCameras.clear();
    m_3dModel.clear();
    m_connections.disconnect();
    m_lastTimestamp = 0;
    m_isInitialized = false;
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::updating()
{
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::computeRegistration(::fwCore::HiResClock::HiResClockType timestamp)
{
    SLM_WARN_IF("Invoking doRegistration while service is STOPPED", this->isStopped() );

    if(!m_isInitialized)
    {
        this->initialize();
    }

    if (this->isStarted())
    {
        if (timestamp > m_lastTimestamp)
        {
            ::fwCore::HiResClock::HiResClockType newerTimestamp =
                std::numeric_limits< ::fwCore::HiResClock::HiResClockType >::max();
            for(size_t i = 0; i < this->getKeyGroupSize(s_MARKERTL_INPUT); ++i)
            {
                auto markerTL = this->getInput< ::arData::MarkerTL >(s_MARKERTL_INPUT, i);
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
            auto markerTL = this->getInput< ::arData::MarkerTL >(s_MARKERTL_INPUT, 0);
            const CSPTR(::arData::MarkerTL::BufferType) buffer = markerTL->getClosestBuffer(newerTimestamp);
            const unsigned int numMarkers = buffer->getMaxElementNum();

            ::arData::MatrixTL::sptr matrixTL = this->getInOut< ::arData::MatrixTL >(s_MATRIXTL_INOUT);

            // Push matrix in timeline
            SPTR(::arData::MatrixTL::BufferType) matrixBuf;

            // For each marker
            bool matrixBufferCreated = false;
            for(unsigned int markerIndex = 0; markerIndex < numMarkers; ++markerIndex)
            {
                std::vector< ARLPointListType > markerPts;
                std::vector< const ::arlCore::Camera* > arlCameras;
                size_t indexTL = 0;

                // For each camera timeline
                for(size_t i = 0; i < this->getKeyGroupSize(s_MARKERTL_INPUT); ++i)
                {
                    auto markerTL = this->getInput< ::arData::MarkerTL >(s_MARKERTL_INPUT, i);
                    const CSPTR(::arData::MarkerTL::BufferType) buffer = markerTL->getClosestBuffer(newerTimestamp);

                    if(buffer->isPresent(markerIndex))
                    {
                        const float* registrationARLBuffer = buffer->getElement(markerIndex);

                        ARLPointListType pl;
                        for(size_t i = 0; i < 4; ++i)
                        {
                            pl.push_back(::arlCore::Point::PointFactory(registrationARLBuffer[i*2],
                                                                        registrationARLBuffer[i*2+1]));
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
                std::vector< double > optimiser_parameters(1, 1), log;

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
                ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
                sig = matrixTL->signal< ::arData::TimeLine::ObjectPushedSignalType >(
                    ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
                sig->asyncEmit(timestamp);

            }
        }
    }
}

//-----------------------------------------------------------------------------

void SPoseFrom2d::initialize()
{
    // Initialization of timelines

    ::arData::MarkerTL::csptr firstTimeline = this->getInput< ::arData::MarkerTL >(s_MARKERTL_INPUT, 0);

    const unsigned int maxElementNum = firstTimeline->getMaxElementNum();

    for(size_t i = 0; i < this->getKeyGroupSize(s_MARKERTL_INPUT); ++i)
    {
        ::arData::MarkerTL::csptr timeline = this->getInput< ::arData::MarkerTL >(s_MARKERTL_INPUT, 0);

        SLM_ASSERT("Timelines should have the same maximum number of elements",
                   maxElementNum == timeline->getMaxElementNum());
    }

    ::arData::MatrixTL::sptr matrixTL = this->getInOut< ::arData::MatrixTL >(s_MATRIXTL_INOUT);
    // initialized matrix timeline
    matrixTL->initPoolSize(maxElementNum);

    // Initialization of ARLCameras
    m_planeSystem = new ::arlCore::PlaneSystem();

    for(size_t idx = 0; idx < this->getKeyGroupSize(s_CAMERA_INPUT); ++idx)
    {
        ::arData::Camera::csptr camera = this->getInput< ::arData::Camera >(s_CAMERA_INPUT, idx);
        OSLM_FATAL_IF("Camera[" << idx << "] not found", !camera);

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
        if (idx == 1)
        {
            auto extrinsicMatrix = this->getInput< ::fwData::TransformationMatrix3D >(s_EXTRINSIC_INPUT);

            SLM_FATAL_IF("Extrinsic matrix with key '" + s_EXTRINSIC_INPUT + "' not found", !extrinsicMatrix);

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
    m_isInitialized = true;
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPoseFrom2d::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push( "markerTL", ::arData::TimeLine::s_OBJECT_PUSHED_SIG, s_COMPUTE_REGISTRATION_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

} // namespace registrationARL
