/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWIGG_MESHER_HXX__
#define __FWIGG_MESHER_HXX__

#include <fwData/Image.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataTools/Mesh.hpp>
#include <fwDataTools/helper/ImageGetter.hpp>
#include <fwDataTools/helper/Mesh.hpp>

#include <Algo/MC/marchingcube.h>

#include <Mesher/parataubin.h>
#include <Mesher/prep_mesh.h>
#include <Mesher/simplifvoxmesh.h>

#include <Topology/generic/parameters.h>
#include <Topology/map/embeddedMap2.h>

#include <memory>


namespace fwIGG
{

//-----------------------------------------------------------------------------

template < typename DATATYPE >
void getMinMax(::fwData::Image::csptr image, DATATYPE& min, DATATYPE& max)
{
    ::fwDataTools::helper::ImageGetter imageLock ( image );
    DATATYPE* buffer                      = static_cast < DATATYPE* > (imageLock.getBuffer());
    const ::fwData::Image::SizeType& size = image->getSize();
    ::fwData::Image::SizeType::value_type len = size[0]*size[1]*size[2];

    typedef std::numeric_limits<DATATYPE> ImgLimits;
    min = ImgLimits::max();
    max = (ImgLimits::is_integer || !ImgLimits::is_signed) ? ImgLimits::min() : -ImgLimits::max();

    DATATYPE* bufEnd = buffer + len;
    DATATYPE currentVoxel;

    for (DATATYPE* voxel = buffer; voxel < bufEnd; ++voxel )
    {
        currentVoxel = *voxel;

        if ( currentVoxel != 0 && currentVoxel <= min )
        {
            min = currentVoxel;
        }
        if (currentVoxel > max)
        {
            max = currentVoxel;
        }
    }
}

//-----------------------------------------------------------------------------

template < typename DATATYPE >
void Mesher::computeMeshMCS(::fwData::Image::csptr img, ::fwData::Mesh::sptr mesh, DATATYPE valueMin,
                            DATATYPE valueMax, unsigned int faces,
                            unsigned int adapt, unsigned int radius, bool percent, bool closing)
{
    ::boost::shared_ptr<PFP::MAP> map(Mesher::computeMeshMCSCGoGN(img, valueMin, valueMax, faces, adapt,
                                                                  radius, percent, closing));

    Mesher::convertCgognMesh( map, mesh, img->getOrigin() );
    ::fwDataTools::Mesh::generatePointNormals(mesh);
}

//-----------------------------------------------------------------------------

template < typename DATATYPE >
::boost::shared_ptr<CGoGN::EmbeddedMap2> Mesher::computeMeshMCSCGoGN(::fwData::Image::csptr img, DATATYPE valueMin,
                                                                     DATATYPE valueMax, unsigned int faces,
                                                                     unsigned int adapt, unsigned int radius,
                                                                     bool percent, bool closing)
{
    ::boost::shared_ptr<PFP::MAP> map(new PFP::MAP);
    int m_radius   = radius;
    double m_adapt = adapt/100;

    ::fwDataTools::helper::ImageGetter imageHelper(img);
    DATATYPE* buff  = static_cast<DATATYPE*> (imageHelper.getBuffer());
    unsigned int wx = img->getSize()[0];
    unsigned int wy = img->getSize()[1];
    unsigned int wz = img->getSize()[2];
    double sx       = img->getSpacing()[0];
    double sy       = img->getSpacing()[1];
    double sz       = img->getSpacing()[2];

    DATATYPE min;
    DATATYPE max;
    getMinMax(img, min, max);

    valueMin = valueMin < min ? min : valueMin;

    DATATYPE valLabel = DATATYPE(0);

    CGoGN::Algo::Surface::MC::Image<DATATYPE> cgognImage(buff,wx,wy,wz,sx,sy,sz);

    std::unique_ptr< CGoGN::Algo::Surface::MC::Image<DATATYPE> > cgognImageFrame( cgognImage.addFrame(m_radius+1) );

    cgognImageFrame->setVoxelSize(sx, sy, sz);
    CGoGN::Algo::Surface::MC::WindowingInterval<DATATYPE> myWindFunc;
    myWindFunc.setMinMax(valueMin, valueMax);

    CGoGN::VertexAttribute<PFP::VEC3> position = map->getAttribute<PFP::VEC3,CGoGN::VERTEX>("position");

    if (!position.isValid())
    {
        position = map->addAttribute<PFP::VEC3, CGoGN::VERTEX>("position");
    }

    CGoGN::VertexAttribute<PFP::VEC3> normal = map->getAttribute<PFP::VEC3, CGoGN::VERTEX>("normal");

    if (!normal.isValid())
    {
        normal = map->addAttribute<PFP::VEC3, CGoGN::VERTEX>("normal");
    }

    CGoGN::Algo::Surface::MC::MarchingCube<DATATYPE,
                                           CGoGN::Algo::Surface::MC::WindowingInterval,
                                           PFP> mc(cgognImageFrame.get(), map.get(), position, myWindFunc, true);
    mc.simpleMeshing();

    CGoGN::Mesher::SimplifVoxMesh<PFP> m_simpl(*map);
    CGoGN::Mesher::parallelComputeCurvature<PFP,
                                            DATATYPE>(*map,
                                                      position,
                                                      m_simpl.curvatureAttrib(),
                                                      cgognImageFrame.get(),
                                                      m_radius,
                                                      valLabel,
                                                      1.0f, 1.0f, 1.0f, 4);

    CGoGN::Mesher::parallelsmoothCurvature<PFP>(*map, m_simpl.curvatureAttrib(), 4);

    if(!closing)
    {
        mc.removeFacesOfBoundary(m_simpl.boundAttrib(),m_radius+1);
    }
    mc.recalPoints(CGoGN::Geom::Vec3f(0.0f,0.0f,0.0f));

    CGoGN::VertexAttribute<PFP::VEC3> position2 = map->getAttribute<PFP::VEC3, CGoGN::VERTEX>("position2");

    if (!position2.isValid())
    {
        position2 = map->addAttribute<PFP::VEC3, CGoGN::VERTEX>("position2");
    }

    CGoGN::Mesher::ParallelfilterTaubin<PFP>(*map, position, position2,3,4);

    unsigned int nbff = m_simpl.init(m_adapt,0.0f);
    m_simpl.setInitialLength(1.0f);

    if(percent)
    {
        unsigned int nbfinal = (nbff*faces)/100;
        m_simpl.until(nbfinal);
    }
    else
    {
        m_simpl.until(faces);
    }

    CGoGN::Mesher::ParallelfilterTaubin<PFP>(*map, position, position2, 2, 4);

    if(cgognImageFrame->getData() != nullptr)
    {
        delete cgognImageFrame->getData(); //Necessary because of a memory leak in CGoGN
    }

    return map;
}

//-----------------------------------------------------------------------------

template < typename DATATYPE >
void Mesher::computeMeshMC(::fwData::Image::csptr img, ::fwData::Mesh::sptr mesh, DATATYPE valueMin, DATATYPE valueMax)
{
    ::boost::shared_ptr<PFP::MAP> map(Mesher::computeMeshMCCGoGN(img, valueMin, valueMax));

    Mesher::convertCgognMesh( map, mesh, img->getOrigin() );
    ::fwDataTools::Mesh::generatePointNormals(mesh);
}

//-----------------------------------------------------------------------------

template < typename DATATYPE >
::boost::shared_ptr<CGoGN::EmbeddedMap2> Mesher::computeMeshMCCGoGN(::fwData::Image::csptr img, DATATYPE valueMin,
                                                                    DATATYPE valueMax)
{
    ::boost::shared_ptr<PFP::MAP> map(new PFP::MAP);
    CGoGN::VertexAttribute<PFP::VEC3> position;

    ::fwDataTools::helper::ImageGetter imageHelper(img);
    DATATYPE* buff  = static_cast<DATATYPE*> (imageHelper.getBuffer());
    unsigned int wx = img->getSize()[0];
    unsigned int wy = img->getSize()[1];
    unsigned int wz = img->getSize()[2];
    double sx       = img->getSpacing()[0];
    double sy       = img->getSpacing()[1];
    double sz       = img->getSpacing()[2];

    DATATYPE min;
    DATATYPE max;
    getMinMax(img, min, max);

    valueMin = valueMin < min ? min : valueMin;

    CGoGN::Algo::Surface::MC::Image<DATATYPE> cgognImage(buff,wx,wy,wz,sx,sy,sz);

    std::unique_ptr< CGoGN::Algo::Surface::MC::Image<DATATYPE> > cgognImageFrame( cgognImage.addFrame(1) );

    CGoGN::Algo::Surface::MC::WindowingInterval<DATATYPE> myWindFunc;
    myWindFunc.setMinMax(valueMin, valueMax);

    position = map->getAttribute<PFP::VEC3,CGoGN::VERTEX>("position");
    if (!position.isValid())
    {
        position = map->addAttribute<PFP::VEC3, CGoGN::VERTEX>("position");
    }

    CGoGN::Algo::Surface::MC::MarchingCube<DATATYPE,
                                           CGoGN::Algo::Surface::MC::WindowingInterval,
                                           PFP> mc(cgognImageFrame.get(), map.get(), position, myWindFunc, false);
    mc.simpleMeshing();

    CGoGN::Geom::Vec3f origin(img->getOrigin()[0], img->getOrigin()[1], img->getOrigin()[2]);
    mc.recalPoints(origin);

    if(cgognImageFrame->getData() != nullptr)
    {
        delete cgognImageFrame->getData(); //Necessary because of a memory leak in CGoGN
    }

    return map;
}

//-----------------------------------------------------------------------------

} // namespace fwIGG

#endif //__FWIGG_MESHER_HXX__
