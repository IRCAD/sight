/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <stdio.h>
#include <iostream>

#include <arlcore/UnitTestArlCore.h>
//#include <arlcore/PointsList.h>
#include <arlcore/Mesh.h>

int main( int argc, char** argv )
{
    arlCore::Mesh mesh;
    mesh.load("D:/ARLBatch.exe/Mesh/test.msh");
    mesh.save("D:/ARLBatch.exe/Mesh/test.vtk");
//  const arlCore::PointList pl = mesh.getPointList();
//  pl.save("D:/ARLBatch.exe/Mesh/test.vtk", arlCore::ARLCORE_POINT_SAVE_VTK);
    
    return 0;
    /*arlCore::PointList T;
    T.load("C:/Trajectoire1.pts");
    T.save("C:/Trajectoire1.pts", arlCore::ARLCORE_POINT_SAVE_VTK);*/
    //************************************************************************
/*      unsigned int x, y, z;
    for( z=0 ; z<=20 ; z+=5 )
        for( y=0 ; y<=20 ; y+=5 )
            for( x=0 ; x<=20 ; x+=5 )
                std::cout<<"GotoReference 50\nRelativMove 6 "<<x<<" "<<y<<" "<<z<<" 0 0 0\nSleep 1000\nTakeAPose\n\n";

*/
    // UNIT TESTS
    bool b=true;
    b=arlCore::testSolveAXXB( 50000, 1e-7 )&&b;

/*  b=arlCore::testInitExtrinsicCalibration( 100, 1e-6 )&&b;
    b=arlCore::testExtrinsicCalibration( 10, 1e-6 )&&b;
    b=arlCore::testInitIntrinsicCalibration( 100, 1e-5 )&&b;
    b=arlCore::testIntrinsicCalibration( 8, 1e-3 )&&b;

    b=arlCore::test3D3DMatching( 800, 0 )&&b;
    b=arlCore::testRegistration3D3DwithoutMatching( 10000, 1e-5 )&&b; // 1e-9

    b=arlCore::testRigidTransfo( 1000000, 1e-5 )&&b; // 1e-9

    b=arlCore::testReconstructionPolyscopic( 6, 10e-10 )&&b;
    b=arlCore::testHomographyRegistration( 10000, 1e-2 )&&b; // 1e-5
    b=arlCore::testProjectiveRegistration( 10, 1e-2 )&&b; // 1.5e-5

    b=arlCore::testSphereCenterEstimation( 5000, 1e-4 )&&b;

    b=arlCore::testRegistration3D3D( 15000, 1e-10 )&&b;
    b=arlCore::testEpipolarMatching( 50, 0 )&&b;
    b=arlCore::testICPRegistration( 3, 1e-11 )&&b;

    b=arlCore::testPolynomialFieldDistortion( 1, 0.6 )&&b;*/

    if(!b)std::cerr<<"ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR ERROR\n";
//  cvWaitKey( 0 );
    if(b) return 1; else return 0;
}
