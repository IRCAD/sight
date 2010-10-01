/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
//
//#include <fwTools/fwID.hpp>
//
//#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
//#include <fwServices/op/Add.hpp>
//

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>

//#include <fwData/Acquisition.hpp>

#include "ctrlSelection/MedicalImageSrv.hpp"

namespace ctrlSelection
{


//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::fwServices::IController, ::ctrlSelection::MedicalImageSrv, ::fwData::Image ) ;

//-----------------------------------------------------------------------------

MedicalImageSrv::MedicalImageSrv() throw()
{}

//-----------------------------------------------------------------------------

MedicalImageSrv::~MedicalImageSrv() throw()
{}

//-----------------------------------------------------------------------------

void MedicalImageSrv::updating( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void MedicalImageSrv::convertImage()
{
    ::fwData::Image::sptr pImg = this->getObject< ::fwData::Image >();
    std::pair<bool, bool> MinMaxTFAreModified = ::fwComEd::fieldHelper::MedicalImageHelpers::checkMinMaxTF( pImg );
    bool landMarksAreModified = ::fwComEd::fieldHelper::MedicalImageHelpers::checkLandmarks( pImg );
    bool sliceIndexAreModified = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageSliceIndex( pImg );

}

//-----------------------------------------------------------------------------

void MedicalImageSrv::starting()  throw ( ::fwTools::Failed )
{
    this->convertImage();
}

//-----------------------------------------------------------------------------

void MedicalImageSrv::stopping()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void MedicalImageSrv::swapping()  throw ( ::fwTools::Failed )
{
    this->convertImage();
}

//-----------------------------------------------------------------------------

void MedicalImageSrv::configuring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void MedicalImageSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void MedicalImageSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void MedicalImageSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // ctrlSelection
