/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
    SLM_ASSERT("Index size not correspond to color size.", m_commandIndexContainer.size() == m_commandColorContainer.size() );
    const ::boost::uint32_t sizeOf = static_cast<boost::uint32_t>(sizeof(VoxelIndex) + 2*sizeof(VoxelType));

    return static_cast< ::boost::uint32_t >(sizeof(*this) + m_commandIndexContainer.size()*sizeOf );
}

//-----------------------------------------------------------------------------

void PaintCommand::setImage( ::fwData::Image::sptr  image )
{
    m_image = image;
}

//-----------------------------------------------------------------------------

void PaintCommand::paint( VoxelIndex index, VoxelType oldValue, VoxelType newValue )
{
    m_commandIndexContainer.push_back(index);
    m_commandColorContainer.push_back( std::make_pair(oldValue,newValue) );
}

//-----------------------------------------------------------------------------

void PaintCommand::apply()
{
    // start image editing
    VoxelType * pixels = static_cast<VoxelType*>( m_image.lock()->getBuffer() );

    // do each voxel modification
    IndexContainer::const_iterator indexI       ( m_commandIndexContainer.begin() );
    ColorContainer::const_iterator indexC       ( m_commandColorContainer.begin() );
    IndexContainer::const_iterator indexIEnd    ( m_commandIndexContainer.end() );

    while ( indexI != indexIEnd )
    {
        const VoxelIndex    index   = *indexI;

        pixels[ index ] = indexC->second;

        ++indexI;
        ++indexC;
    }


    this->notifyImageModification();
}

//-----------------------------------------------------------------------------

void PaintCommand::unapply()
{
    // start image editing
    VoxelType * pixels = static_cast<VoxelType*>( m_image.lock()->getBuffer() );

    // do each voxel modification
    IndexContainer::const_iterator indexI       ( m_commandIndexContainer.begin() );
    ColorContainer::const_iterator indexC       ( m_commandColorContainer.begin() );
    IndexContainer::const_iterator indexIEnd    ( m_commandIndexContainer.end() );

    while ( indexI != indexIEnd )
    {
        const VoxelIndex    index   = *indexI;

        pixels[ index ] = indexC->first;

        ++indexI;
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
