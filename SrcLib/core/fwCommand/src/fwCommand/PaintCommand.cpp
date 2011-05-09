/* ***** BEGIN LICENSE BLOCK *****
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

const boost::uint32_t  PaintCommand::getSize() const
{
    SLM_ASSERT("Index size not correspond to color size.",
            m_commandIndexContainer.size() == m_commandColorContainer.size() );
    const ::boost::uint32_t sizeOf = static_cast<boost::uint32_t>(sizeof(BufferIndex) + 2*sizeof(BufferType));

    return static_cast< ::boost::uint32_t >(sizeof(*this) + m_commandIndexContainer.size()*sizeOf );
}

//-----------------------------------------------------------------------------

void PaintCommand::setImage( ::fwData::Image::sptr  image )
{
    m_image = image;
}

//-----------------------------------------------------------------------------

void PaintCommand::prePaint( VoxelIndex x, VoxelIndex y, VoxelIndex z )
{
    ::fwData::Image::sptr image = m_image.lock();
    const std::vector< ::boost::int32_t> &size = image->getCRefSize();
    const int &sx = size[0];
    const int &sy = size[1];
    const VoxelIndex index = x + sx*y + z*sx*sy;
    prePaint(index);
}

//-----------------------------------------------------------------------------

void PaintCommand::prePaint( VoxelIndex index )
{
    ::fwData::Image::sptr image = m_image.lock();
    m_currentPrepaintIndex = index;
    SLM_ASSERT("currentPrepaintBuff must be empty. Forgot a postPaint call ?", m_currentPrepaintBuff.empty());
    BufferType *buf  = static_cast<BufferType*>( image->getBuffer() ) + index;
    std::copy(buf, buf+image->getPixelType().sizeOf(), std::back_insert_iterator<std::vector<BufferType> >(m_currentPrepaintBuff));
}

//-----------------------------------------------------------------------------

void PaintCommand::postPaint()
{
    ::fwData::Image::sptr image = m_image.lock();
    BufferType *buf  = static_cast<BufferType*>( image->getBuffer() ) + m_currentPrepaintIndex;

    unsigned int imageTypeSize = image->getPixelType().sizeOf();
    BufferIndex bufIndex = m_currentPrepaintIndex * imageTypeSize;

    for (unsigned int i = 0; i < imageTypeSize; ++i)
    {
        paint( bufIndex + i, m_currentPrepaintBuff[i], buf[i]);
    }
    m_currentPrepaintBuff.clear();
}

//-----------------------------------------------------------------------------

void PaintCommand::paint( BufferIndex index, BufferType oldValue, BufferType newValue )
{
    m_commandIndexContainer.push_back(index);
    m_commandColorContainer.push_back( std::make_pair(oldValue,newValue) );
}

//-----------------------------------------------------------------------------

void PaintCommand::apply()
{
    // start image editing
    BufferType* pixels = static_cast<BufferType*>( m_image.lock()->getBuffer() );
    SLM_ASSERT("commandIndexContainer and commandColorContainer must have same size",
            m_commandColorContainer.size() == m_commandIndexContainer.size());

    // do each voxel modification
    ColorContainer::const_iterator indexC( m_commandColorContainer.begin() );
    BOOST_FOREACH(BufferIndex index, m_commandIndexContainer)
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
    BufferType * pixels = static_cast<BufferType*>( m_image.lock()->getBuffer() );
    SLM_ASSERT("commandIndexContainer and commandColorContainer must have same size",
            m_commandColorContainer.size() == m_commandIndexContainer.size());

    // do each voxel modification
    ColorContainer::const_iterator indexC( m_commandColorContainer.begin() );
    BOOST_FOREACH(BufferIndex index, m_commandIndexContainer)
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
