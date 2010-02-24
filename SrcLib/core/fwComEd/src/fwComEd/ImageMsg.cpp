/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwComEd/ImageMsg.hpp"

namespace fwComEd
{

std::string ImageMsg::NEW_IMAGE = "NEW_IMAGE";
std::string ImageMsg::BUFFER = "BUFFER";
std::string ImageMsg::DIMENSION = "DIMENSION";
std::string ImageMsg::SPACING = "SPACING";
std::string ImageMsg::REGION = "REGION";
std::string ImageMsg::PIXELTYPE = "PIXELTYPE";
std::string ImageMsg::TRANSFERTFUNCTION = "TRANSFERTFUNCTION";
std::string ImageMsg::WINDOWING = "WINDOWING";
std::string ImageMsg::LANDMARK = "LANDMARK";
std::string ImageMsg::DISTANCE = "DISTANCE";
std::string ImageMsg::NEW_DISTANCE = "NEW_DISTANCE";
std::string ImageMsg::DELETE_DISTANCE = "DELETE_DISTANCE";
std::string ImageMsg::SLICE_INDEX = "SLICE_INDEX";
std::string ImageMsg::ACTIVATE_SYNC_CROSS = "ACTIVATE_SYNC_CROSS";
std::string ImageMsg::INACTIVATE_SYNC_CROSS = "INACTIVATE_SYNC_CROSS";
std::string ImageMsg::CHANGE_SLICE_TYPE = "CHANGE_SLICE_TYPE";

//-----------------------------------------------------------------------------

ImageMsg::ImageMsg() throw()
{}

//-----------------------------------------------------------------------------

ImageMsg::~ImageMsg() throw()
{}

//-----------------------------------------------------------------------------

} // namespace fwComEd

