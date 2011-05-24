/** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/ClassRegistrar.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/ImageMsg.hpp>

#include "fwCommand/PaintCommand.hpp"


namespace fwCommand
{

//-----------------------------------------------------------------------------

REGISTER_BINDING_BYCLASSNAME( ::fwCommand::ICommand, ::fwCommand::PaintCommand, ::fwCommand::PaintCommand );

//-----------------------------------------------------------------------------
PaintCommand::PaintCommand() : ICommand(),
    m_buffer(0)
{
}

//-----------------------------------------------------------------------------

const boost::uint32_t  PaintCommand::getSize() const
{
    SLM_ASSERT("Index size not correspond to color size.",
            m_commandIndexContainer.size() == m_commandColorContainer.size() );
    const ::boost::uint32_t sizeOf = static_cast<boost::uint32_t>(sizeof(::fwData::Image::BufferIndexType) + 2*sizeof(::fwData::Image::BufferType));

    return static_cast< ::boost::uint32_t >(sizeof(*this) + m_commandIndexContainer.size()*sizeOf );
}

//-----------------------------------------------------------------------------

void PaintCommand::setImage( ::fwData::Image::sptr  image )
{
    m_image = image;
    //We assume that during all the command construction, the image do not
    //change and is not destroyed, so we can keep a reference to it buffer
    m_buffer = static_cast< ::fwData::Image::BufferType* >( image->getBuffer() );
    SLM_ASSERT("Using a paintcommand on an image without buffer is not possible", m_buffer);
}

//-----------------------------------------------------------------------------

void PaintCommand::prePaint( ::fwData::Image::VoxelIndexType x, ::fwData::Image::VoxelIndexType y, ::fwData::Image::VoxelIndexType z )
{
    ::fwData::Image::sptr image = m_image.lock();
    const std::vector< ::boost::int32_t> &size = image->getCRefSize();
    const int &sx = size[0];
    const int &sy = size[1];
    const ::fwData::Image::VoxelIndexType index = x + sx*y + z*sx*sy;
    prePaint(index);
}

//-----------------------------------------------------------------------------

void PaintCommand::prePaint( ::fwData::Image::VoxelIndexType index )
{
    ::fwData::Image::sptr image = m_image.lock();
    unsigned int imageTypeSize = image->getPixelType().sizeOf();

    m_currentPrepaintIndex = index;
    SLM_ASSERT("currentPrepaintBuff must be empty. Forgot a postPaint call ?", m_currentPrepaintBuff.empty());
    ::fwData::Image::BufferType *buf  = m_buffer + index*imageTypeSize;
    std::copy(buf, buf+imageTypeSize, std::back_insert_iterator<std::vector< ::fwData::Image::BufferType > >(m_currentPrepaintBuff));
}

//-----------------------------------------------------------------------------

void PaintCommand::postPaint()
{
    ::fwData::Image::sptr image = m_image.lock();
    ::fwData::Image::BufferType *buf  = m_buffer + m_currentPrepaintIndex;

    unsigned int imageTypeSize = image->getPixelType().sizeOf();
    ::fwData::Image::BufferIndexType bufIndex = m_currentPrepaintIndex * imageTypeSize;

    for (unsigned int i = 0; i < imageTypeSize; ++i)
    {
        paint( bufIndex + i, m_currentPrepaintBuff[i], buf[i]);
    }
    m_currentPrepaintBuff.clear();
}

//-----------------------------------------------------------------------------

void PaintCommand::paint( ::fwData::Image::BufferIndexType index, ::fwData::Image::BufferType oldValue, ::fwData::Image::BufferType newValue )
{
    m_commandIndexContainer.push_back(index);
    m_commandColorContainer.push_back( std::make_pair(oldValue,newValue) );
}

//-----------------------------------------------------------------------------

void PaintCommand::apply()
{
    // start image editing
    ::fwData::Image::BufferType* pixels = static_cast< ::fwData::Image::BufferType* >( m_image.lock()->getBuffer() );
    SLM_ASSERT("commandIndexContainer and commandColorContainer must have same size",
            m_commandColorContainer.size() == m_commandIndexContainer.size());

    // do each voxel modification
    ColorContainer::const_iterator indexC( m_commandColorContainer.begin() );
    BOOST_FOREACH(::fwData::Image::BufferIndexType index, m_commandIndexContainer)
    {
        pixels[ index ] = indexC->second;
        ++indexC;
    }
    this->notifyImageModification();
}

//-----------------------------------------------------------------------------

void PaintCommand::unapply()
{
    // start image editing
    ::fwData::Image::BufferType * pixels = static_cast< ::fwData::Image::BufferType* >( m_image.lock()->getBuffer() );
    SLM_ASSERT("commandIndexContainer and commandColorContainer must have same size",
            m_commandColorContainer.size() == m_commandIndexContainer.size());

    // do each voxel modification
    ColorContainer::const_iterator indexC( m_commandColorContainer.begin() );
    BOOST_FOREACH(::fwData::Image::BufferIndexType index, m_commandIndexContainer)
    {
        pixels[ index ] = indexC->first;
        ++indexC;
    }

    this->notifyImageModification();
}

//-----------------------------------------------------------------------------

const std::string PaintCommand::getDescription() const
{
    return std::string("Paint command");
}

//-----------------------------------------------------------------------------

void PaintCommand::notifyImageModification()
{
    if ( ! this->m_serviceNotifier.expired() )
    {
        ::fwComEd::ImageMsg::NewSptr msg;
        msg->addEvent( fwComEd::ImageMsg::BUFFER );
        ::fwServices::IEditionService::notify( this->getNotifier(), m_image.lock(), msg );
    }
}

//-----------------------------------------------------------------------------

} // namespace fwCommand
