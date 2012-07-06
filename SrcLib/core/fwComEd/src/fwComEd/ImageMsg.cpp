/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "fwComEd/ImageMsg.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwComEd::ImageMsg, ::fwComEd::ImageMsg );

namespace fwComEd
{

std::string ImageMsg::NEW_IMAGE = "NEW_IMAGE";
std::string ImageMsg::BUFFER = "BUFFER";
std::string ImageMsg::MODIFIED = "MODIFIED";
std::string ImageMsg::DIMENSION = "DIMENSION";
std::string ImageMsg::SPACING = "SPACING";
std::string ImageMsg::REGION = "REGION";
std::string ImageMsg::PIXELTYPE = "PIXELTYPE";
std::string ImageMsg::LANDMARK = "LANDMARK";
std::string ImageMsg::DISTANCE = "DISTANCE";
std::string ImageMsg::NEW_DISTANCE = "NEW_DISTANCE";
std::string ImageMsg::DELETE_DISTANCE = "DELETE_DISTANCE";
std::string ImageMsg::SLICE_INDEX = "SLICE_INDEX";
std::string ImageMsg::ACTIVATE_SYNC_CROSS = "ACTIVATE_SYNC_CROSS";
std::string ImageMsg::INACTIVATE_SYNC_CROSS = "INACTIVATE_SYNC_CROSS";
std::string ImageMsg::CHANGE_SLICE_TYPE = "CHANGE_SLICE_TYPE";
std::string ImageMsg::VALUE_IS_MODIFIED = "VALUE_IS_MODIFIED";
std::string ImageMsg::TRANSPARENCY = "TRANSPARENCY";
std::string ImageMsg::VISIBILITY = "VISIBILITY";

//-----------------------------------------------------------------------------

ImageMsg::ImageMsg() throw()
{}

//-----------------------------------------------------------------------------

ImageMsg::~ImageMsg() throw()
{}

//-----------------------------------------------------------------------------

void ImageMsg::setSliceIndex(::fwData::Integer::sptr a, ::fwData::Integer::sptr f, ::fwData::Integer::sptr s, ::fwData::Object::sptr _pDataInfo )
{

    this->addEvent( ::fwComEd::ImageMsg::SLICE_INDEX, _pDataInfo );
    m_axialIndex = a;
    m_frontalIndex = f;
    m_sagittalIndex = s;
}

//-----------------------------------------------------------------------------

void ImageMsg::getSliceIndex(::fwData::Integer::sptr a, ::fwData::Integer::sptr f, ::fwData::Integer::sptr s) const
{
    SLM_ASSERT( "SLICE_INDEX Event not found in msg", this->hasEvent( ::fwComEd::ImageMsg::SLICE_INDEX ));
    a->deepCopy(m_axialIndex   );
    f->deepCopy(m_frontalIndex );
    s->deepCopy(m_sagittalIndex);
}

//-----------------------------------------------------------------------------

} // namespace fwComEd

