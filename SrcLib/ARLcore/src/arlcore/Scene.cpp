/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/Scene.h>

#include <fstream>

#include <arlcore/SmartPointsList.h>
#include <arlcore/vnl_rotation3d_vector.h>

arlCore::Scene::Scene( PlaneSystem &universe ):
m_universe(universe),
m_tags(new Tags(universe)),
m_cameras(universe)
{}

arlCore::Scene::Scene( PlaneSystem &universe, Tags* tags ):
m_universe(universe),
m_tags(tags),
m_cameras(universe)
{}

arlCore::Scene::~Scene( void )
{
    assert(m_tags!=0);
    delete m_tags;
}

arlCore::Tag* arlCore::Scene::addTag(unsigned int nbPoints, ARLCORE_SHAPE shapeType, const Point &centre, double size)
{
    PointList pointsMonde;
    pointsMonde.shapeRandom(nbPoints, shapeType, centre, size);
    return m_tags->addTag(pointsMonde);
}

bool arlCore::Scene::plot( void ) const
{
    const bool JustVisible = false;
    const bool Overwrite = true;
    std::fstream f1,f2;
    unsigned int i;
    for( i=0 ; i<m_tags->size() ; ++i )
        m_tags->getTag(i)->getGeometry().save("c:/000000.dat", ARLCORE_POINT_SAVE_GNUPLOT, JustVisible, Overwrite);
    f1.open ("c:/000000.dat", std::fstream::out | std::fstream::app);
    Point pt0(0.0, 0.0, 0.0),pt1(400.0, 0.0, 0.0),pt2(0.0, 300.0, 0.0), pt;
    for( i=0 ; i<m_cameras.size() ; ++i )
    {
        m_cameras[i].getInvExtrinsic().trf(pt0, pt);
        f1<<"\n"<<pt.x()<<" "<<pt.y()<<" "<<pt.z();
        m_cameras[i].getInvExtrinsic().trf(pt1, pt);
        f1<<"\n"<<pt.x()<<" "<<pt.y()<<" "<<pt.z();
        m_cameras[i].getInvExtrinsic().trf(pt2, pt);
        f1<<"\n"<<pt.x()<<" "<<pt.y()<<" "<<pt.z();
    }
    f1.close();
    //TODO Afficher tous les tags avec des couleurs differentes
    f2.open (TMP_FILE, std::fstream::out);
    if(!f2.is_open()) return false;
    f2<<"set title 'Scene'\n";
    f2<<"splot \"c:/000000.dat\"\n";
    for( i=0 ; i<m_cameras.size() ; ++i )
    {
        m_cameras[i].getInvExtrinsic().trf(pt0, pt);
        f2<<"set label \""<<i+1<<"\" at "<<pt.x()<<","<<pt.y()<<","<<pt.z()<<"\n";
        f2<<"set arrow "<<i+1<<" from "<<pt.x()<<","<<pt.y()<<","<<pt.z()<<" to 0,0,0\n";
    }
    f2<<"pause -1 \"Hit return to continue\"\n";
    f2.close();
    system(GNUPLOT_EXE );
    //exec("d:/gnuplot/bin/wgnuplot", "c:/000000.dem" ); 
    return true;
}

arlCore::PlaneSystem& arlCore::Scene::getPlaneSystem( void )
{
    return m_universe;
}

const arlCore::CameraList& arlCore::Scene::getCameras( void ) const
{
    return m_cameras;
}

unsigned int arlCore::Scene::getNbCameras( void ) const
{
    return (unsigned int)m_cameras.size();
}


/*bool arlCore::Scene::nextImages( unsigned int step )
{
    setTime();
    unsigned int i;
    for( i=0 ; i<m_cameras.size() ; ++i )
        if(m_images[i]!=0) m_images[i]->fill(0);
    return dynamicBehavior(step);
}*/
/*
unsigned int arlCore::Scene::detection( unsigned int cam, arlCore::SmartPointList &spl, std::vector< const arlCore::Tag* > &tagsDetected, std::map< const arlCore::Tag*, unsigned int >& allTagsDetected )
{
    return detectionBehavior( cam, spl, tagsDetected, allTagsDetected );
}*/

bool arlCore::Scene::addCameras(unsigned int nbCameras, const Point& centre,
                            double sphereRadius, double distMin,
                            const std::vector<double> &intrinsic_param,
                            const std::vector<double> &intrinsic_range)
{
    unsigned int i, j;
    // tirage des points de vue des cameras
    // on tire d'abord sa position dans une sphere autour de centre
    // puis on l'eloigne du point de mire de la distance distMin
    PointList CameraPOV(3);
    double scalar, dist;
    CameraPOV.shapeRandom(nbCameras,ARLCORE_SHAPE_SPHERE, centre, sphereRadius);
    //for( j=0 ; j<nb_camera ; ++j )
        //cerr<< "CameraPOV 1= " << CameraPOV[j]->getString() << endl;
    for( i=0 ; i<nbCameras ; ++i )
    {
        dist = CameraPOV[i]->distance(centre);
        scalar = (dist+distMin)/dist;
        for( j=0 ; j<3 ; ++j )            
            CameraPOV[i]->set(j,((*CameraPOV[i])[j]-centre[j])*scalar + centre[j] );
    }
    //for( j=0 ; j<nb_camera ; ++j )
        //cerr<< "CameraPOV 2= " << CameraPOV[j]->getString() << endl;
    // tirage des parametres intrinseques
    Point tmp_intrinsic((unsigned int)intrinsic_param.size()), 
    tmp_intrinsic_var((unsigned int)intrinsic_param.size());
    std::vector<Point> tirage_intrinsic;
    for( i=0 ; i<intrinsic_param.size() ; ++i )
        tmp_intrinsic.set(i,intrinsic_param[i]);
    tmp_intrinsic_var = tmp_intrinsic;
    for( i=0 ; i<nbCameras ; ++i )
    {
        for( j=0 ; j<intrinsic_param.size() ; ++j )
            tmp_intrinsic_var.addUniformNoise(j, intrinsic_range[j]);
        tirage_intrinsic.push_back(tmp_intrinsic_var);
        tmp_intrinsic_var = tmp_intrinsic;           
        //cerr<< "tirage_intrinsic = " << tirage_intrinsic[j].getString() << endl;
    }
    // construction des cameras synthetiques
    std::vector<double> intrinsic;
    uint32HL GUID={0,0}; // FIXME
    unsigned int width=800, heigth=600; // FIXME
    for(i=0; i<nbCameras; i++)
    {
        intrinsic.clear();
        for(j=0; j<intrinsic_param.size(); j++)
            intrinsic.push_back(tirage_intrinsic[i][j]);
        m_cameras.push_back( GUID, width, heigth );
        m_cameras[i].syntheticCamera( centre , *(CameraPOV[i]), intrinsic);
        //cerr<< "CameraPOV 3= " << CameraPOV[j]->getString() << endl;
        //intrinsic.clear();
        //cerr<< "CamParam ["<<i<<"]= " << CamParam[i]->getString() << endl;
    }
    for( i=1 ; i<m_cameras.size() ; ++i )
        m_universe.setIdentity( m_cameras[i-1].getPlane(), m_cameras[i].getPlane() );
    return true;
}

bool arlCore::Scene::addCameras(unsigned int nbCameras, const Point& centre,
                            double sphereRadius, double distMin,
                            const std::vector<double> &intrinsic_param,
                            const std::vector<double> &intrinsic_range,
                            double angleMin)
{
    unsigned int i, j;
    // tirage des points de vue des cameras
    // on tire d'abord sa position dans une sphere autour de centre
    // puis on l'eloigne du point de mire de la distance distMin
    PointList CameraPOV(3), CameraPOV_before(3);
    double scalar, dist;
    CameraPOV_before.shapeRandom(nbCameras,ARLCORE_SHAPE_SOLIDANGLE, centre, sphereRadius, angleMin);
    //for(j=0; j<nb_camera; j++)
        //cerr<< "CameraPOV 1= " << CameraPOV[j]->getString() << endl;
        
    // choose a random direction for the solid angle
    vnl_vector_fixed<double,3> t_null(0,0,0);
    vnl_rotation3d_vector random_vector;
    random_vector.uniform_random();// a random rotation is created
    vnl_rigid_matrix mat( (vnl_rotation3d_matrix) random_vector, t_null);//this rigid matrix with the random rotation is applied to CameraPOV_before
    mat.trf(CameraPOV_before, CameraPOV);
    
    for( i=0 ; i<nbCameras ; ++i )
    {
        dist = CameraPOV[i]->distance(centre);
        scalar = (dist+distMin)/dist;
        for( j=0 ; j<3 ; ++j )            
            CameraPOV[i]->set(j,((*CameraPOV[i])[j]-centre[j])*scalar + centre[j] );
    }
    //for(j=0; j<nb_camera; j++)
        //cerr<< "CameraPOV 2= " << CameraPOV[j]->getString() << endl;
    // tirage des parametres intrinseques
    Point tmp_intrinsic((unsigned int)intrinsic_param.size()), 
    tmp_intrinsic_var((unsigned int)intrinsic_param.size());
    std::vector<Point> tirage_intrinsic;
    for( i=0 ; i<intrinsic_param.size() ; ++i )
        tmp_intrinsic.set(i, intrinsic_param[i]);
    tmp_intrinsic_var = tmp_intrinsic;
    for( i=0 ; i<nbCameras ; ++i )
    {
        for( j=0 ; j<intrinsic_param.size() ; ++j )
            tmp_intrinsic_var.addUniformNoise(j, intrinsic_range[j]);
        tirage_intrinsic.push_back(tmp_intrinsic_var);
        tmp_intrinsic_var = tmp_intrinsic;           
        //cerr<< "tirage_intrinsic = " << tirage_intrinsic[j].getString() << endl;
    }
    // construction des cameras synthetiques
    uint32HL GUID={0,0}; // FIXME
    unsigned int width=800, heigth=600; // FIXME
    std::vector<double> intrinsic;
    for( i=0 ; i<nbCameras ; ++i )
    {
        intrinsic.clear();
        for( j=0 ; j<intrinsic_param.size() ; ++j )
            intrinsic.push_back(tirage_intrinsic[i][j]);
//      m_cameras.push_back( Camera (m_universe) );
        m_cameras.push_back( GUID, width, heigth );
        m_cameras.back().syntheticCamera( centre , *(CameraPOV[i]), intrinsic);
        //cerr<< "CameraPOV 3= " << CameraPOV[j]->getString() << endl;
        //intrinsic.clear();
        //cerr<< "CamParam ["<<i<<"]= " << CamParam[i]->getString() << endl;
    }
    for( i=0 ; i<m_cameras.size()-1 ; ++i )
        for( j=i+1 ; j<m_cameras.size() ; ++j )
            m_universe.setIdentity(m_cameras[i].getPlane(),m_cameras[j].getPlane());
    return true;
}

arlCore::Tags& arlCore::Scene::getTags( void )
{
    return *m_tags;
}

bool arlCore::Scene::dynamicBehavior( unsigned int step )
{
    return false;
}

unsigned int arlCore::Scene::detectionBehavior( unsigned int cam, SmartPointList &, std::vector< const Tag* > &tags, std::map< const Tag*, unsigned int >& allTags )
{
    return 0;
}

unsigned int arlCore::Scene::detection( unsigned int camNo, unsigned int tagNo, SmartPointList &spl, double gaussianNoise )
{
    if(m_tags->size()<tagNo || camNo>m_cameras.size() || camNo==0 ) return 0;
    unsigned int i;
    long int date=0, time=0;
    Point pt(2,Point::ARLCORE_POINT_TYPE_ARTK,date,time);
    Point pt3D(3);
    pt.setStatus(Point::ARLCORE_POINT_STATUS_DETECTED);
    const double ColorMin=50.0;
    const double ColorStep=(255.0-ColorMin)/(double)(m_tags->size()+2);
    if(m_tags->getTag(tagNo)==0) return 0;
    assert(m_tags->getTag(tagNo)!=0);
    PointList &tagPoints=m_tags->getTag(tagNo)->getGeometry();
    vnl_rigid_matrix T;
    const bool Transfo = m_universe.getTrf( m_tags->getTag(tagNo)->getPlane(), m_cameras[camNo-1].getPlane(), T );
    //T.invert();
    for( i=0 ; i<tagPoints.size() ; ++i )
    {
        T.trf(*tagPoints[i], pt3D);
        m_cameras[camNo-1].project3DPoint( pt3D, pt);
        const Colour color(0,ColorMin+ColorStep*(tagNo+1),0);
        pt.setColour(color);
        if(gaussianNoise>0) pt.addGaussianNoise(gaussianNoise);
        spl.push_back(pt, camNo, (void*)m_tags->getTag(tagNo));
    }
    return tagPoints.size();
}
