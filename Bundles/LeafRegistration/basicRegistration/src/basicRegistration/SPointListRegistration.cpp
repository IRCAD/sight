/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "basicRegistration/SPointListRegistration.hpp"

#include <fwCom/Signal.hxx>
#include <fwComEd/Dictionary.hpp>

#include <fwData/Composite.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/PointList.hpp>
#include <fwData/String.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

#include <vtkPoints.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>
#include <vtkLandmarkTransform.h>


fwServicesRegisterMacro( ::fwServices::IController, ::basicRegistration::SPointListRegistration, ::fwData::Composite );


namespace basicRegistration
{

SPointListRegistration::SPointListRegistration()
{
}

// ----------------------------------------------------------------------------

SPointListRegistration::~SPointListRegistration()
{
}

// ----------------------------------------------------------------------------

void SPointListRegistration::configuring() throw ( ::fwTools::Failed )
{
    m_registeredPointsKey = m_configuration->findConfigurationElement("registeredPoints")->getAttributeValue(
        "compositeKey");
    m_referencePointsKey = m_configuration->findConfigurationElement("referencePoints")->getAttributeValue(
        "compositeKey");
    m_matrixKey = m_configuration->findConfigurationElement("matrix")->getAttributeValue("compositeKey");
}

// ----------------------------------------------------------------------------

void SPointListRegistration::starting() throw ( ::fwTools::Failed )
{
}

// ----------------------------------------------------------------------------

void SPointListRegistration::stopping() throw ( ::fwTools::Failed )
{
}

// ----------------------------------------------------------------------------

void SPointListRegistration::updating() throw ( ::fwTools::Failed )
{
    // Get fw4spl data
    ::fwData::Composite::sptr composite    = this->getObject< ::fwData::Composite >();
    ::fwData::PointList::sptr registeredPL =
        ::fwData::PointList::dynamicCast( (*composite)[ m_registeredPointsKey ] );
    ::fwData::PointList::sptr referencePL = ::fwData::PointList::dynamicCast(
        (*composite)[ m_referencePointsKey ] );
    ::fwData::TransformationMatrix3D::sptr matrix =
        ::fwData::TransformationMatrix3D::dynamicCast( (*composite)[ m_matrixKey ] );

    if( registeredPL->getPoints().size() >= 3 &&
        registeredPL->getPoints().size() == referencePL->getPoints().size() )
    {
        vtkSmartPointer<vtkLandmarkTransform> landmarkTransform = vtkSmartPointer<vtkLandmarkTransform>::New();

        vtkPoints* sourcePts = vtkPoints::New();
        vtkPoints* targetPts = vtkPoints::New();

        // Match each point in both list according to the label
        for( ::fwData::Point::sptr pointRef : referencePL->getPoints() )
        {
            const std::string& labelRef =
                pointRef->getField< ::fwData::String >(::fwComEd::Dictionary::m_labelId )->value();

            for( ::fwData::Point::sptr pointReg : registeredPL->getPoints() )
            {
                const std::string& labelReg =
                    pointReg->getField< ::fwData::String >(::fwComEd::Dictionary::m_labelId )->value();

                if(labelRef == labelReg)
                {
                    auto coord = pointRef->getRefCoord();
                    sourcePts->InsertNextPoint(coord[0], coord[1], coord[2]);

                    OSLM_ERROR("referencePL : " << pointRef->getField< ::fwData::String >(
                                   ::fwComEd::Dictionary::m_labelId )->value() );
                    OSLM_ERROR(
                        "referencePL : " << pointRef->getCoord()[0] << " " << pointRef->getCoord()[1] << " " <<
                        pointRef->getCoord()[2] );

                    coord = pointReg->getRefCoord();
                    targetPts->InsertNextPoint(coord[0], coord[1], coord[2]);
                    OSLM_ERROR("registeredPL : " << pointReg->getField< ::fwData::String >(
                                   ::fwComEd::Dictionary::m_labelId )->value() );
                    OSLM_ERROR(
                        "registeredPL : " << pointReg->getCoord()[0] << " " << pointReg->getCoord()[1] << " " <<
                        pointReg->getCoord()[2] );
                }
            }
        }

        landmarkTransform->SetSourceLandmarks(sourcePts);
        landmarkTransform->SetTargetLandmarks(targetPts);
        landmarkTransform->SetModeToRigidBody();
        landmarkTransform->Update();

        // Get the resulting transformation matrix (this matrix takes the source points to the target points)
        vtkSmartPointer<vtkMatrix4x4> m = landmarkTransform->GetMatrix();
        m->Invert();
        for(size_t l = 0; l < 4; ++l)
        {
            for(size_t c = 0; c < 4; ++c)
            {
                matrix->setCoefficient(l, c, m->GetElement(l,c));
            }
        }

        // Notify Matrix modified
        auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
            sig->asyncEmit();
        }
    }
    else
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Error",
                                                          "You must enter 2 or more points for the registration to work.",
                                                          ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

// ----------------------------------------------------------------------------

void SPointListRegistration::swapping() throw ( ::fwTools::Failed )
{
    // Classic default approach to update service when object change
    this->stopping();
    this->starting();
}

// ----------------------------------------------------------------------------

} // namespace basicRegistration


