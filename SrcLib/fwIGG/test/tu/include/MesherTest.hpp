/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWIGG_UT_MESHERTEST_HPP__
#define __FWIGG_UT_MESHERTEST_HPP__

#include <boost/filesystem.hpp>

#include <cppunit/extensions/HelperMacros.h>

#include <fwCore/HiResTimer.hpp>

#include <fwData/Image.hpp>

#include <Mesher/simplifvoxmesh.h>

namespace fwIGG
{
namespace ut
{

struct PFP : public CGoGN::PFP_STANDARD
{
    typedef CGoGN::EmbeddedMap2 MAP;
};


class MesherTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( MesherTest );
CPPUNIT_TEST( memoryTest );
CPPUNIT_TEST( sameMeshTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void memoryTest();
    void sameMeshTest();

};

} //namespace ut
} //namespace fwIGG

#endif // __FWIGG_UT_MESHERTEST_HPP__
