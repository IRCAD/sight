/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/UnitTestArlCore.h>

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <iostream>
//#include <conio.h> // kbhit & getch

#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_matrix_inverse.h>

#include <arlcore/Misc.h>
#include <arlcore/Camera.h>
#include <arlcore/Calibration.h>
#include <arlcore/FieldCorrection.h>
#include <arlcore/Point.h>
#include <arlcore/PointsList.h>
#include <arlcore/SmartPointsList.h>
#include <arlcore/PlaneSystem.h>
#include <arlcore/MatrixR.h>
#include <arlcore/Reconst3D.h>
#include <arlcore/Optimization.h>
#include <arlcore/UncertaintyPropag.h>
#include <arlcore/vnl_rigid_vector.h>
#include <arlcore/vnl_rotation3d_matrix.h>
#include <arlcore/vnl_rotation3d_vector.h>
#include <arlcore/Scenes.h>
#include <arlcore/Mesh.h>
#include <arlcore/ICP.h>

static long int m_date;
static long int m_time;

void printHeader( const std::string &name, unsigned int nbIterations, double tolerance )
{
    const long int Seed = 1171888051;
    std::cerr<<"\n************\n";
    std::cerr<<"* Unit Test : "<<name<<"\n";
    std::cerr<<"************\n";
    std::cerr<<"* Iterations  = "<<nbIterations<<"\n";
    std::cerr<<"* Tolerance   = "<<tolerance<<"\n";
    if(!arlRandom::Random::initRandom("Random-100000000.txt",500000)==0)
        std::cerr<<"* Random seed = Random-100000000.txt,500000\n";
    else std::cerr<<"* Random seed = "<<arlRandom::Random::initRandom(Seed)<<"\n";
    std::cerr<<"*\n";
    m_date = arlTime::getNowDate();
    m_time = arlTime::getNowTime();
}

bool printErrors( const std::string &name, const vnl_vector<double>& error, long int robustnessOK, long int robustnessTotal, double tolerance, bool Verbose=false)
{
    const unsigned int precision=4;
    const double mean=error.mean();
    double stdDev = 0;
    unsigned int i;
    for( i=0 ; i<error.size() ; ++i )
        stdDev += (error[i]-mean)*(error[i]-mean);
    if(error.size()>0) stdDev = sqrt(stdDev/error.size());
    //Calcul de l'�cart type plus elegant mais couteux en memoire
    //vnl_vector<double> meanVect(error.size());
    //meanVect.fill(mean);
    //stdDev = (error-meanVect).rms();
    bool test=true;
    if(tolerance>=0) test=error.max_value()<=tolerance;
    std::cerr<<std::setprecision(precision);
    if(Verbose)
        for( i=0; i<error.size() ; ++i )
            std::cerr<<"* "<<name<<"["<<i<<"]="<<error[i]<<"\n";
    std::cerr<<"* "<<name<<" : ";
    if(tolerance>=0)
        if(test) std::cerr<<"OK OK";
        else std::cerr<<"ERROR";
    std::cerr<<"\n*  Mean/Std = {"<<mean<<" , "<<stdDev<<"}\n";
    std::cerr<<"*  Min/Max  = ["<<error.min_value()<<" , "<<error.max_value()<<"]\n";
    double p=0;
    if(robustnessTotal!=0) p=(double)robustnessOK*100.0/(double)robustnessTotal;
    std::cerr<<"*  Robustness = "<<robustnessOK<<" / "<<robustnessTotal<<" = "<<p<<"%\n";
    return test;
}

bool printErrors( const std::string &name, const vnl_vector_fixed<double,5>& stat, long int robustnessOK, long int robustnessTotal, double tolerance, bool Verbose=false)
{
    double min, max, mean, stdDev, RMS;
    const double n = arlCore::computeStat( stat, min, max, mean, stdDev, RMS );
    const unsigned int Precision=4;
    const bool Test = max<tolerance;
    std::cerr<<std::setprecision(Precision);
    std::cerr<<"* "<<name<<" : ";
    if(Test) std::cerr<<"OK OK\n";
    else std::cerr<<"ERROR\n";
    std::cerr<<"*  Mean/Std/RMS = {"<<mean<<" , "<<stdDev<<" , "<<RMS<<"}\n";
    std::cerr<<"*  Min/Max  = ["<<min<<" , "<<max<<"]\n";
    double p=0;
    if(robustnessTotal!=0) p=(double)robustnessOK*100.0/(double)robustnessTotal;
    std::cerr<<"*  Robustness = "<<robustnessOK<<" / "<<robustnessTotal<<" = "<<p<<"%\n";
    return Test;
}


bool printFooter( unsigned int nbIterations, bool test )
{
    double a = (double)m_date;
    double b = (double)m_time;
    m_date = arlTime::getNowDate();
    m_time = arlTime::getNowTime();
    double sec=0; //m_date-a;
    if(b>m_time)
        sec+=1.0-(b+(double)m_time)/1000000.0;
    else
        sec+=((double)m_time-b)/((double)CLK_TCK*10);
    std::cerr<<"*\n";
    std::cerr<<"*  Total Time = "<<sec<<" seconds\n";
    std::cerr<<"*  Time/Iter. = "<<sec/nbIterations<<" seconds\n";
    if(test) std::cerr<<"** OK OK OK OK OK OK OK OK";
    else     std::cerr<<"** ERROR ERROR ERROR ERROR";
    std::cerr<<"\n************\n";
    return test;
}

// testRegistration3D3D
bool arlCore::testRegistration3D3D( long int nbIterations, double tolerance )
{
    const bool Verbose = false;
    const double VolumeSize = 500.0;
    const double GaussianNoise3D = 0.0;
    printHeader("Registration 3D/3D",nbIterations, tolerance);
    vnl_vector<double> errorRotation(nbIterations);
    vnl_vector<double> errorTranslation(nbIterations);
    long int robustnessOK=0, robustnessTotal=0;
    long int compteur;
    for( compteur=0 ; compteur<nbIterations ; ++compteur )
    {
        arlCore::PlaneSystem universe;
        arlCore::SceneUnitTest scene(universe);
        const arlCore::PointList &regPoints = scene.getTags().getTag(0)->getGeometry();
        arlCore::PointList modelPoints(3);
        arlCore::vnl_rigid_matrix T1, T2;
        T1.uniform_random(VolumeSize);
        T1.trf(regPoints, modelPoints);
//      unsigned int i;
//      for( i=0 ; i<modelPoints.size() ; ++i )
//          modelPoints[i]->addGaussianNoise(GaussianNoise3D);
        if(T2.register3D3D(regPoints, modelPoints, true))
        {
            ++robustnessOK;
            T1.compare(T2, errorTranslation[compteur], errorRotation[compteur]);
            //T2.RMS3D3D( modelPoints, regPoints, regPoints.size(),
            //          erreur_recalage3D3D, errorVector[compteur], EC_3D3D);
        }
        ++robustnessTotal;
    }//boucle compteur
    bool test = printErrors("Erreur sur la translation",errorTranslation,robustnessOK,robustnessTotal,tolerance);
    test = printErrors("Erreur sur la rotation",errorRotation,robustnessOK,robustnessTotal,tolerance) && test;
    return printFooter(nbIterations, test);
}

// testICPRegistration
bool arlCore::testICPRegistration( long int nbIterations, double tolerance )
{
    const bool Verbose = false;
    const double VolumeSize = 10.0;
    const double GaussianNoise3D = 0.0;
    printHeader("ICP Registration",nbIterations, tolerance);
    vnl_vector_fixed<double,5> errorRotation(0.0);
    vnl_vector_fixed<double,5> errorTranslation(0.0);
    long int robustnessOK=0, robustnessTotal=0;
    long int compteur;
    arlCore::vnl_rigid_matrix T1, T2, Identity;
    unsigned int i;
    for( compteur=0 ; compteur<nbIterations ; ++compteur )
    {
        const double Width=50, Length=50, Degree=2.0, ModelNoise=0.0;
        arlCore::PointList summits;
        arlCore::Mesh meshModel(1.0);
        double altitude = 10.0, modelXStep=0.2, modelZStep=0.2;
        double cloudXStep = 0.1, cloudZStep = 2.0;
        const double FringeWidth = cloudZStep/4.0;
        unsigned int nbSummits = 8;
        const arlCore::PointList &model = meshModel.getPointList();
        if(compteur==0) meshModel.load("G:/Mouse/ToSort/19052008-sd-ext.raw", GaussianNoise3D);
        if(compteur>0 || meshModel.getPointList().size()<1 )
        {
            for( i=0 ; i<nbSummits ; ++i )
            {
                const double X = arlRandom::Random::uniformDoubleRnd( 0, Width );
                const double Z = arlRandom::Random::uniformDoubleRnd( 0, Length );
                double Y = arlRandom::Random::uniformDoubleRnd( altitude/2, altitude );
                if(i<4) Y = arlRandom::Random::uniformDoubleRnd( 0, altitude/2 );
                summits.push_back(arlCore::Point(X, Y, Z));
            }
            meshModel.generateY( Width, Length, modelXStep, modelZStep, Degree, ModelNoise, summits );
        }
        std::stringstream modelName;
        modelName<<"../Model"<<compteur<<".raw";
        meshModel.save(modelName.str());
        if(compteur==0) meshModel.resampling(.5);
        //if(Verbose) std::cout<<compteur<<"\n";
        // Set Random transformation
        const double Phi = arlRandom::Random::uniformDoubleRnd( -15*arlCore::DegToRad, 15*arlCore::DegToRad );
        const double Theta = arlRandom::Random::uniformDoubleRnd( -15*arlCore::DegToRad, 15*arlCore::DegToRad );
        const double Psy = arlRandom::Random::uniformDoubleRnd( -15*arlCore::DegToRad, 15*arlCore::DegToRad );
        vnl_rotation3d_matrix rot( Phi, Theta, Psy );
        T1.setRotation(rot);
        T1.put(0,3,arlRandom::Random::uniformDoubleRnd( -VolumeSize/2, VolumeSize/2 ));
        T1.put(1,3,arlRandom::Random::uniformDoubleRnd( -VolumeSize/2, VolumeSize/2 ));
        T1.put(2,3,arlRandom::Random::uniformDoubleRnd( -VolumeSize/2, VolumeSize/2 ));
        //T1.uniform_random(VolumeSize);
        //T1.trf(model, cloud);
        if(Verbose) std::cout<<T1.getString();
        arlCore::Mesh meshCloud(-1.0);
        arlCore::PointList fringes(3);
        const arlCore::PointList *cloud = 0;
        if(compteur==0)
        {
            arlCore::Point gravity, boundingBox1, boundingBox2;
            double minRadius, maxRadius, std;
            model.properties( gravity, boundingBox1, boundingBox2, minRadius, maxRadius, std );
            const double Ratio = (FringeWidth/2.0)/cloudZStep;
            for( i=0 ; i<model.size() ; ++i )
                if(model[i])
                {
                    //if(Verbose) std::cout<<"Ratio = "<<Ratio<<"\n";
                    arlCore::Point point(*model[i]);
                    const double Z = point.z();
                    int fringeNo = int(Z/cloudZStep);
                    //if(Verbose) std::cout<<"FringeNo = "<<fringeNo<<"\n";
                    point.setScalar(-fringeNo);
                    double d = fabs((Z/cloudZStep)-(double)fringeNo);
                    //if(Verbose) std::cout<<"d = "<<d<<"\n";
                    if(d<=Ratio) fringes.push_back(point);
                }
            cloud = &fringes;
        }else
        {
            meshCloud.generateY( Width, Length, cloudXStep, cloudZStep, Degree, GaussianNoise3D, summits );
            cloud = &meshCloud.getPointList();
        }
        std::stringstream cloudName, cloudName2;
        cloudName<<"../Cloud"<<compteur<<".raw";
        cloudName2<<"../Cloud"<<compteur<<".pts";
        if(compteur==0) fringes.save(cloudName2.str());
        else meshCloud.save(cloudName.str());
        if(Verbose) std::cout<<"Cloud ("<<cloud->size()<<") = "<<cloudName.str()<<"\n";
        if(Verbose) std::cout<<"Model ("<<model.size()<<") = "<<modelName.str()<<"\n";
        T2=T1;
        std::stringstream trfName;
        trfName<<"../init"<<compteur<<".trf";
        T2.save(trfName.str());
        //T2.invert();
        const double RmsMax=-1;
        const bool JustVisible = false;
        const unsigned int NbIterationsMax = 500;
        arlCore::ICP icp( model, *cloud, JustVisible);
        icp.setMaxIterations(NbIterationsMax);
        icp.initSolution(T2);
        //if(T2.registerICP( model, *cloud, firstRMS, lastRMS, iterations, JustVisible, RmsMax, NbIterationsMax ))
        if(icp.solve())
        {
            double errorT, errorR;
            ++robustnessOK;
            Identity.compare(icp.getSolution(), errorT, errorR);
            addValue(errorTranslation, errorT);
            addValue(errorRotation, errorR);
            if(Verbose)
            {
                std::cout<<"*** ICP Initialization\n";
                std::cout<<"RMS = "<<icp.getStartError()<<" -> "<<icp.getEndError()<<"\n";
                std::cout<<"Iterations = "<<icp.getNbIterations()<<"\n";
                std::cout<<"Error Translation = "<<errorT<<"\n";
                std::cout<<"Error Rotation = "<<errorR<<"\n";
                std::cout<<icp.getSolution().getString()<<"\n";
            }
            icp.optimiseLS();
            Identity.compare(icp.getSolution(), errorT, errorR);
            if(Verbose)
            {
                std::cout<<"*** ICP LM optimisation\n";
                std::cout<<"RMS = "<<icp.getStartError()<<" -> "<<icp.getEndError()<<"\n";
                std::cout<<"Iterations = "<<icp.getNbIterations()<<"\n";
                std::cout<<"Error Translation = "<<errorT<<"\n";
                std::cout<<"Error Rotation = "<<errorR<<"\n";
                std::cout<<icp.getSolution().getString()<<"\n";
            }
        }
        ++robustnessTotal;
    }//boucle compteur
    std::string name="Translation error";
    bool test = printErrors(name, errorTranslation, robustnessOK, robustnessTotal, tolerance);
    name="Rotation error";
    test = printErrors(name, errorRotation, robustnessOK, robustnessTotal, tolerance) && test;
    return printFooter(nbIterations, test);
}

// testRegistration3D3DUncertainty
bool arlCore::testRegistration3D3DUncertainty( long int nbIterations, double tolerance )
{
    const bool Verbose = false;
    const double VolumeSize = 500.0;
    const double rangeGaussianNoise3D = 0.0;
    printHeader("Registration Uncertainty 3D/3D",nbIterations, tolerance);
    vnl_vector<double> errorRotation(nbIterations);
    vnl_vector<double> errorTranslation(nbIterations);
    long int robustnessOK=0, robustnessTotal=0;
    long int compteur;
    std::vector<double> log, opt_param;
    for( compteur=0 ; compteur<nbIterations ; ++compteur )
    {
        arlCore::PlaneSystem universe;
        arlCore::SceneUnitTest scene(universe);
        const arlCore::PointList &worldPoints = scene.getTags().getTag(0)->getGeometry();
        arlCore::PointList modelPoints(3);
        arlCore::vnl_rigid_matrix T1, T2;
        T1.uniform_random(VolumeSize);
        T1.trf(worldPoints, modelPoints);

        double rangeGaussianNoise3D_x = arlRandom::Random::gaussianRnd(VolumeSize/100.0);
        double rangeGaussianNoise3D_y = arlRandom::Random::gaussianRnd(VolumeSize/100.0);
        double rangeGaussianNoise3D_z = arlRandom::Random::gaussianRnd(VolumeSize/100.0);
        if(rangeGaussianNoise3D_x < 1e-3){rangeGaussianNoise3D_x = 0.1;}
        if(rangeGaussianNoise3D_y < 1e-3){rangeGaussianNoise3D_y = 0.1;}
        if(rangeGaussianNoise3D_z < 1e-3){rangeGaussianNoise3D_z = 0.1;}
//      for(unsigned int i=0 ; i<modelPoints.size() ; ++i )
//      {
//          modelPoints[i]->set(0, modelPoints[i]->x() + arlRandom::Random::gaussianRnd(rangeGaussianNoise3D_x));
//          modelPoints[i]->set(1, modelPoints[i]->y() + arlRandom::Random::gaussianRnd(rangeGaussianNoise3D_y));
//          modelPoints[i]->set(2, modelPoints[i]->z() + arlRandom::Random::gaussianRnd(rangeGaussianNoise3D_z));
//      }
        // we fill the covariance matrix to check that the optimization process takes it into account
        // we do not apply any noise on the data !! This is only done for uncertainty prediction
        for( unsigned int i=0 ; i<modelPoints.size() ; ++i )
        {
            arlCore::vnl_covariance_matrix &cov_mat = modelPoints[i]->getCovMatrix();
            cov_mat.fill(0.0);
            cov_mat.put(0,0,rangeGaussianNoise3D_x*rangeGaussianNoise3D_x);
            cov_mat.put(1,1,rangeGaussianNoise3D_y*rangeGaussianNoise3D_y);
            cov_mat.put(2,2,rangeGaussianNoise3D_z*rangeGaussianNoise3D_z);
            //std::cerr<<"mat cov = " << cov_mat <<std::endl;
        }

        if(T2.register3D3DUncertainty(worldPoints, modelPoints, arlCore::ARLCORE_REGISTER3D3D_GC, opt_param, log/*, worldPoints.size()*/) )
        {
            ++robustnessOK;
            T1.compare(T2, errorTranslation[compteur], errorRotation[compteur]);
            //T2.RMS3D3D( modelPoints, worldPoints, worldPoints.size(),
            //          erreur_recalage3D3D, errorVector[compteur], EC_3D3D);
        }
        ++robustnessTotal;
    }//boucle compteur
    bool test = printErrors("Erreur sur la translation",errorTranslation,robustnessOK,robustnessTotal,tolerance);
    test = printErrors("Erreur sur la rotation",errorRotation,robustnessOK,robustnessTotal,tolerance) && test;
    return printFooter(nbIterations, test);
}

// testProjectiveRegistration
bool arlCore::testProjectiveRegistration( long int nbIterations, double tolerance )
{
    const bool Verbose = false;
    const double TransfVolumeSize = 500.0;
    const double GaussianNoise2D = 0.0;
    const unsigned int nbTypes = arlCore::ARLCORE_PR_NBTYPES;
    printHeader("Projective registration",nbIterations, tolerance);
    std::vector< vnl_vector_fixed<double,5> > errorRotation(nbTypes);
    std::vector< vnl_vector_fixed<double,5> > errorTranslation(nbTypes);
    std::vector<long int> robustnessOK(nbTypes), robustnessTotal(nbTypes);
    double errorT, errorR;
    long int compteur;
    unsigned int enum_3D2D, i,j;
    for( compteur=0 ; compteur<nbIterations ; ++compteur)
    {
        //std::cerr<<"Iteration "<<compteur<<std::endl;
        arlCore::PlaneSystem universe;
        arlCore::SceneUnitTest scene(universe);
        const arlCore::CameraList &cameras = scene.getCameras();
        const arlCore::PointList &worldPoints = scene.getTags().getTag(0)->getGeometry();
        std::vector< std::vector<arlCore::Point::csptr> >reprojection2D(cameras.size());
        arlCore::SmartPointList splMonde;
        //std::cerr<<"Camera Number "<<cameras.size()<<std::endl;
        //std::cerr<<"Point Number "<<worldPoints.size()<<std::endl;
        for( j=0 ; j<cameras.size() ; ++j )
        {
            scene.detection( j+1, 0, splMonde, GaussianNoise2D );
            splMonde.getPoints(reprojection2D[j], j+1, (void*) scene.getTags().getTag(0));
        }
        arlCore::vnl_rigid_matrix T1,T2;
        T1.uniform_random(TransfVolumeSize);
        arlCore::PointList modelPoints(3);
        T1.trf(worldPoints, modelPoints);
        for(enum_3D2D=arlCore::ARLCORE_PR_UNKNOWN+1,i=0; enum_3D2D <arlCore::ARLCORE_PR_NBTYPES; ++enum_3D2D,++i)
        {
            if( enum_3D2D!=ARLCORE_PR_ISPPC_CG && enum_3D2D!=ARLCORE_PR_OSPPC_CG && enum_3D2D!=ARLCORE_PR_EPPC_CG )
            {
                //std::cerr<<"Method :"<<ARLCORE_PROJECTIVE_REGISTRATION_NAMES[enum_3D2D]<<std::endl;
                if(compteur==0)
                {
                    errorTranslation[i].fill(0.0);
                    errorRotation[i].fill(0.0);
                    robustnessOK[i]=0;
                    robustnessTotal[i]=0;
                }
                T2.setIdentity();
                std::vector<double> log, param_optimisation;
                if(arlCore::multiViewPointRegistration3D2D( cameras.getList(), reprojection2D, modelPoints,
                    T2, (arlCore::ARLCORE_PROJECTIVE_REGISTRATION)enum_3D2D, param_optimisation, log, 0))
                {
                    T1.compareInverse(T2, errorT, errorR);
                    addValue(errorTranslation[i], errorT);
                    addValue(errorRotation[i], errorR);
                    ++robustnessOK[i];
                }
                ++robustnessTotal[i];
            }
        }
    }//boucle compteur
    bool test=true;
    for(enum_3D2D=arlCore::ARLCORE_PR_UNKNOWN+1,i=0; enum_3D2D <arlCore::ARLCORE_PR_NBTYPES; ++enum_3D2D,++i)
    {
        if( enum_3D2D!=ARLCORE_PR_ISPPC_CG && enum_3D2D!=ARLCORE_PR_OSPPC_CG && enum_3D2D!=ARLCORE_PR_EPPC_CG )
        {
            std::string name="Translation error ("+arlCore::ARLCORE_PROJECTIVE_REGISTRATION_NAMES[enum_3D2D]+")";
            test = printErrors(name,errorTranslation[i],robustnessOK[i],robustnessTotal[i],tolerance) && test;
            name="Rotation error ("+arlCore::ARLCORE_PROJECTIVE_REGISTRATION_NAMES[enum_3D2D]+")";
            test = printErrors(name,errorRotation[i],robustnessOK[i],robustnessTotal[i],tolerance) && test;
        }
    }
    return printFooter(nbIterations, test);
}

// testHomographyRegistration
bool arlCore::testHomographyRegistration( long int nbIterations, double tolerance )
{
    const bool Verbose = false;
    const double VolumeSize = 500.0;
    const double GaussianNoise2D = 0.0;
    const double GaussianNoise3D = 0.0;
    printHeader("Homography registration",nbIterations, tolerance);
    vnl_vector_fixed<double,5> errorRotation(0.0);
    vnl_vector_fixed<double,5> errorTranslation(0.0);
    long int robustnessOK=0, robustnessTotal=0;
    double errorT, errorR;
    unsigned int i;
    long int  compteur;
    for(compteur=0;compteur<nbIterations;++compteur)
    {
        //std::cerr<<"Compteur ="<<compteur<<std::endl;
        arlCore::PlaneSystem universe;
        arlCore::SceneUnitTest scene(universe);
        const arlCore::Point &centre=scene.getCentre();
        const arlCore::CameraList &cameras = scene.getCameras();
        arlCore::Tag* tag_monde_plan = scene.getTags().getTag(1);
        arlCore::PointList &point_monde_plan = tag_monde_plan->getGeometry();
        arlCore::vnl_rotation3d_vector random_rot_vec;
        arlCore::Point translation(0,0,0),tmp_trf;
        random_rot_vec.uniform_random();
        translation.shapeRandom(ARLCORE_SHAPE_CUBE, VolumeSize);
        arlCore::vnl_rotation3d_matrix random_rot_mat(random_rot_vec);
        arlCore::vnl_rigid_matrix T1, T2;
        T1.setRotation(random_rot_mat);
        for( i=0 ; i<3 ; ++i )
             T1.put(i,3, translation[i] );
        arlCore::PointList point_plan_a_recaler(3);
            //on change la transfo de maniere a positionner les points dans le champ de vue des cameras
        T1.put(0,3, centre.x() );T1.put(1,3, centre.y() );T1.put(2,3, centre.z() );
        for(i=0; i<point_monde_plan.size(); ++i)
        {//on met dans point_plan_a_recaler la position des points plans 3D dans le repere monde avec z=0
            point_plan_a_recaler.push_back( *(point_monde_plan[i]) );
            T1.trf(*(point_monde_plan[i]));
        }
        T1.invert();
        for( i=0 ; i<cameras.size() ; ++i)
        {
            std::vector<arlCore::Point::csptr> reprojection2D;
            arlCore::SmartPointList splMonde;
            scene.detection( i+1, 1, splMonde, GaussianNoise2D );
            splMonde.getPoints(reprojection2D, i+1, (void*) tag_monde_plan);
            std::vector<double> log, param_optimisation;
            T2.setIdentity();
            if( arlCore::planarHomographyRegistration_3D_2D(cameras[i], reprojection2D, point_plan_a_recaler.getList(),
                T2, param_optimisation, log, true) )
            {
                T1.compareInverse(T2, errorT, errorR);
                addValue(errorTranslation, errorT);
                addValue(errorRotation, errorR);
                ++robustnessOK;
            }
            ++robustnessTotal;
        }
    }//boucle compteur
    bool test = printErrors("Erreur sur la translation",errorTranslation,robustnessOK,robustnessTotal,tolerance);
    test = printErrors("Erreur sur la rotation",errorRotation,robustnessOK,robustnessTotal,tolerance) && test;
    return printFooter(nbIterations, test);
}

// testReconstructionPolyscopic
bool arlCore::testReconstructionPolyscopic( long int nbIterations, double tolerance)
{
    printHeader("Polyscopic reconstruction",nbIterations, tolerance);
    const bool Verbose = false;
    const double GaussianNoise2D=0.0;
    std::vector<double> log;
    std::vector<long int> robustnessOK, robustnessTotal;
    arlCore::Point point_reconstruit;
    long int compteur;
    unsigned int i,j,k,enum_recons,plane=0;
    std::vector< vnl_vector_fixed<double,5> > errorReproj;
    for( compteur=0 ; compteur<nbIterations ; ++compteur)
    {
        //std::cerr << "Iteration " << compteur <<std::endl;
        arlCore::PlaneSystem universe;
        arlCore::SceneUnitTest scene(universe);
        const unsigned int NbCameras = (unsigned int)scene.getCameras().size();
        const unsigned int NbPoints = (unsigned int)scene.getTags().getTag(0)->getGeometry().size();
        const std::vector<arlCore::Camera> &cameras = scene.getCameras().getList();
        // creation de points 3D autour du point mire
        // worldPoints = point dans un volume pour tester le recalage 3D/3D et ISPPC OSPPC et EPPC
        // FONCTION(point_mire, nombre de points max, parametre de tirage, points tires, points tires bruites, param bruit)
        const arlCore::Tag *tag_monde = scene.getTags().getTag(0);
        const arlCore::Tag *tag_monde_plan = scene.getTags().getTag(1);
        const arlCore::PointList &worldPoints = tag_monde->getGeometry();
        std::vector< std::vector<arlCore::Point::csptr> >reprojection2D(NbCameras);
        arlCore::SmartPointList splMonde;
        for( j=0 ; j<NbCameras ; ++j)
        {
            scene.detection( j+1, 0, splMonde, GaussianNoise2D );
            splMonde.getPoints(reprojection2D[j], j+1, (void*) tag_monde);
        }
        for(enum_recons=arlCore::ARLCORE_R3D_UNKNOWN +1,k=0; enum_recons<arlCore::ARLCORE_R3D_NBTYPE; ++enum_recons,++k)
        {
            if(Verbose) std::cerr << "Changement de methode\n";
            if( compteur==0 )
            {
                vnl_vector_fixed<double,5> tmp(0.0);
                errorReproj.push_back(tmp);
                robustnessOK.push_back(0);
                robustnessTotal.push_back(0);
            }
            for( i=0 ; i<NbPoints ; ++i )
            {
                std::vector<arlCore::Point::csptr> pointList2D;
                    for( j=0 ; j<NbCameras ; ++j )
                        pointList2D.push_back(reprojection2D[j][i]);
                    if(arlCore::reconst3D( pointList2D, cameras, point_reconstruit, (arlCore::ARLCORE_RECONSTRUCTION3D) enum_recons, plane, log))
                    {
                        const double Error = point_reconstruit.distance( *(worldPoints[i]) );
                        addValue(errorReproj[k], Error);
                        if(Verbose) std::cerr << "Point reconstruit = "<< point_reconstruit[0] <<" "<< point_reconstruit[1] <<" "<<point_reconstruit[2] <<"\n";
                        ++robustnessOK[k];
                    }
                    ++robustnessTotal[k];
            }
        }
    }//boucle compteur
    bool test = true;
    for(enum_recons=arlCore::ARLCORE_R3D_UNKNOWN+1,i=0; enum_recons<arlCore::ARLCORE_R3D_NBTYPE; ++enum_recons,++i)
    {
        std::string name="Reconstruction error ("+arlCore::ARLCORE_RECONSTRUCTION3D_NAMES[enum_recons]+")";
        test = printErrors(name,errorReproj[i],robustnessOK[i],robustnessTotal[i],tolerance) && test;
    }
    return printFooter(nbIterations, test);
}

// testReconstructionPolyscopicUncertainty
bool arlCore::testReconstructionPolyscopicUncertainty( long int nbIterations, double tolerance)
{
    printHeader("Polyscopic reconstruction uncertainty",nbIterations, tolerance);
    const bool Verbose = false;
    const double rangeGaussianNoise2D_x=2.0, rangeGaussianNoise2D_y=2.0;
    const double rangeGaussianNoise2D=2.0;
    std::vector<double> log;
    std::vector< std::vector<double> > index_mu(6);//(nbIterations);
    std::vector< vnl_vector<double> > errorVector;
    std::vector<long int> robustnessOK, robustnessTotal;
    arlCore::Point point_reconstruit;
    long int compteur, nbpt=0;
    unsigned int i,j,k,enum_recons,plane=0;
    for( compteur=0 ; compteur<nbIterations ; ++compteur)
    {
        arlCore::PlaneSystem universe;
        arlCore::SceneUnitTest scene(universe,200,1);
        const unsigned int nbCameras = (unsigned int)scene.getCameras().size();
        const unsigned int nbPoints = scene.getTags().getTag(0)->getGeometry().size();
        const std::vector<arlCore::Camera> &cameras = scene.getCameras().getList();
        // creation de points 3D autour du point mire
        // worldPoints = point dans un volume pour tester le recalage 3D/3D et ISPPC OSPPC et EPPC
        // FONCTION(point_mire, nombre de points max, parametre de tirage, points tires, points tires bruites, param bruit)
        const arlCore::Tag *tag_monde = scene.getTags().getTag(0);
        const arlCore::Tag *tag_monde_plan = scene.getTags().getTag(1);
        const arlCore::PointList &worldPoints = tag_monde->getGeometry();
        std::vector< std::vector<arlCore::Point::csptr> >reprojection2D(nbCameras);
        std::vector< std::vector<arlCore::Point::csptr> >reprojection2D_nonoise(nbCameras);
        arlCore::SmartPointList  splMonde;
        for( j=0 ; j<nbCameras ; ++j)
        {
            scene.detection( j+1, 0, splMonde, 0.0 );
            splMonde.getPoints(reprojection2D_nonoise[j], j+1, (void*) tag_monde);
            for( i=0 ; i<nbPoints ; ++i )
            {
                arlCore::Point* tmp;
                tmp = new arlCore::Point(2);
                reprojection2D[j].push_back(tmp);
            }
        }
        double GaussianNoise2D_x = arlRandom::Random::gaussianRnd(rangeGaussianNoise2D_x);
        double GaussianNoise2D_y = arlRandom::Random::gaussianRnd(rangeGaussianNoise2D_y);
        double GaussianNoise2D = 0.1;//arlRandom::Random::gaussianRnd(rangeGaussianNoise2D);
//      std::cerr << "GaussianNoise2D_x["<<compteur<<"] =" << GaussianNoise2D_x<<std::endl;
//      std::cerr << "GaussianNoise2D_y["<<compteur<<"] =" << GaussianNoise2D_y<<std::endl;
        std::cerr << "GaussianNoise2D["<<compteur<<"] =" << GaussianNoise2D<<std::endl;
//      for( j=0 ; j<nbCameras ; ++j)
//          for( i=0 ; i<nbPoints ; ++i )
//          {
//              reprojection2D[j][i]->set(0, reprojection2D[j][i]->x() + arlRandom::Random::gaussianRnd(GaussianNoise2D_x));
//              reprojection2D[j][i]->set(1, reprojection2D[j][i]->y() + arlRandom::Random::gaussianRnd(GaussianNoise2D_y));
//              arlCore::vnl_covariance_matrix &cov_mat = reprojection2D[j][i]->getCovMatrix();
//              cov_mat.put(0,0,GaussianNoise2D_x*GaussianNoise2D_x);
//              cov_mat.put(1,0,0.0);
//              cov_mat.put(0,1,0.0);
//              cov_mat.put(1,1,GaussianNoise2D_y*GaussianNoise2D_y);
//          }

//      for(enum_recons=arlCore::ARLCORE_R3D_UNKNOWN +1,k=0; enum_recons<arlCore::ARLCORE_R3D_NBTYPE; ++enum_recons,++k)
        for(enum_recons=arlCore::ARLCORE_R3D_TWO_LINES_APPROX ,k=0; enum_recons<=arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION; ++enum_recons,++k)
//      for(enum_recons=arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION ,k=0; enum_recons<=arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY; ++enum_recons,++k)
        {
            if( compteur==0 )
            {
                vnl_vector<double> tmp(nbIterations);
                errorVector.push_back(tmp);
                robustnessOK.push_back(0);
                robustnessTotal.push_back(0);
            }

            if(enum_recons == arlCore::ARLCORE_R3D_TWO_LINES_APPROX || enum_recons == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION)
{
            if(enum_recons == arlCore::ARLCORE_R3D_TWO_LINES_APPROX)
            {
                for( j=0 ; j<nbCameras ; ++j)
                {
                    for( i=0 ; i<nbPoints ; ++i )
                    {
                        //*reprojection2D[j][i] = *reprojection2D_nonoise[j][i];
                        //std::cerr<<"reprojection2D_nonoise ="<<reprojection2D_nonoise[j][i]->x()<<" "<<reprojection2D_nonoise[j][i]->y()<<"\n";
                        ((Point::sptr)reprojection2D[j][i])->set(0, reprojection2D_nonoise[j][i]->x() + arlRandom::Random::gaussianRnd(GaussianNoise2D));
                        ((Point::sptr)reprojection2D[j][i])->set(1, reprojection2D_nonoise[j][i]->y() + arlRandom::Random::gaussianRnd(GaussianNoise2D));
                        //std::cerr<<"reprojection2D="<<reprojection2D[j][i]->x()<<" "<<reprojection2D[j][i]->y()<<"\n";
                        arlCore::vnl_covariance_matrix &cov_mat = ((Point::sptr)reprojection2D[j][i])->getCovMatrix();
                        cov_mat.put(0,0,GaussianNoise2D*GaussianNoise2D);
                        cov_mat.put(1,0,0.0);
                        cov_mat.put(0,1,0.0);
                        cov_mat.put(1,1,GaussianNoise2D*GaussianNoise2D);
                        //std::cerr<<"cov_mat="<<cov_mat<<"\n";
                    }
                }
            }
//          if(enum_recons == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION)
//          {
//              for( j=0 ; j<nbCameras ; ++j)
//              {
//                  for( i=0 ; i<nbPoints ; ++i )
//                  {
//                      //*reprojection2D[j][i] = *reprojection2D_nonoise[j][i];
//                      //std::cerr<<"reprojection2D_nonoise ="<<reprojection2D_nonoise[j][i]->x()<<" "<<reprojection2D_nonoise[j][i]->y()<<"\n";
//                      reprojection2D[j][i]->set(0, reprojection2D_nonoise[j][i]->x() + arlRandom::Random::gaussianRnd(GaussianNoise2D));
//                      reprojection2D[j][i]->set(1, reprojection2D_nonoise[j][i]->y() + arlRandom::Random::gaussianRnd(GaussianNoise2D));
//                      //std::cerr<<"reprojection2D="<<reprojection2D[j][i]->x()<<" "<<reprojection2D[j][i]->y()<<"\n";
//                      arlCore::vnl_covariance_matrix &cov_mat = reprojection2D[j][i]->getCovMatrix();
//                      cov_mat.put(0,0,GaussianNoise2D*GaussianNoise2D);
//                      cov_mat.put(1,0,0.0);
//                      cov_mat.put(0,1,0.0);
//                      cov_mat.put(1,1,GaussianNoise2D*GaussianNoise2D);
//                      //std::cerr<<"cov_mat="<<cov_mat<<"\n";
//                  }
//              }
//          }
            if(enum_recons == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY)
            {
                for( j=0 ; j<nbCameras ; ++j)
                {
                    for( i=0 ; i<nbPoints ; ++i )
                    {
                        //*reprojection2D[j][i] = *reprojection2D_nonoise[j][i];
                        //std::cerr<<"reprojection2D_nonoise ="<<reprojection2D_nonoise[j][i]->x()<<" "<<reprojection2D_nonoise[j][i]->y()<<"\n";
                        ((Point::sptr)reprojection2D[j][i])->set(0, reprojection2D_nonoise[j][i]->x() + arlRandom::Random::gaussianRnd(GaussianNoise2D_x));
                        ((Point::sptr)reprojection2D[j][i])->set(1, reprojection2D_nonoise[j][i]->y() + arlRandom::Random::gaussianRnd(GaussianNoise2D_y));
                        //std::cerr<<"reprojection2D="<<reprojection2D[j][i]->x()<<" "<<reprojection2D[j][i]->y()<<"\n";
                        arlCore::vnl_covariance_matrix &cov_mat = ((Point::sptr)reprojection2D[j][i])->getCovMatrix();
                        cov_mat.put(0,0,GaussianNoise2D_x*GaussianNoise2D_x);
                        cov_mat.put(1,0,0.0);
                        cov_mat.put(0,1,0.0);
                        cov_mat.put(1,1,GaussianNoise2D_y*GaussianNoise2D_y);
                        //std::cerr<<"cov_mat="<<cov_mat<<"\n";
                    }
                }

            }
            double reconstructionError = 0.0;
            for( i=0 ; i<nbPoints ; ++i )
            {
                std::vector<arlCore::Point::csptr> pointList2D;
                    for( j=0 ; j<nbCameras ; ++j )
                        pointList2D.push_back(reprojection2D[j][i]);
                    if(arlCore::reconst3D( pointList2D, cameras, point_reconstruit, (arlCore::ARLCORE_RECONSTRUCTION3D) enum_recons, plane, log))
                    {
//                      if(i==0)
                        {
                            vnl_matrix<double> soustraction(3,1);
                            vnl_vector<double> tt(3);
                            tt[0] = worldPoints[i]->x();tt[1] = worldPoints[i]->y();tt[2] = worldPoints[i]->z();
                            soustraction.set_column(0, point_reconstruit.getCoordinates() - tt);
                            //std::cerr << "soustraction =\n"<<soustraction<<std::endl;
                            //std::cerr << "cov matrice =\n"<<point_reconstruit.getCovMatrix()<<std::endl;
                            //std::cerr << "trace cov matrice =\n"<<sqrt( point_reconstruit.getCovMatrix()[0][0] +point_reconstruit.getCovMatrix()[1][1]+point_reconstruit.getCovMatrix()[2][2])<<std::endl;
//                          index_mu[compteur] = (soustraction.transpose() * vnl_matrix_inverse<double> (point_reconstruit.getCovMatrix()) * soustraction)(0,0);
                            index_mu[k].push_back( (soustraction.transpose() * vnl_matrix_inverse<double> (point_reconstruit.getCovMatrix()) * soustraction)(0,0) );
                            std::cerr << "index_mu["<<k<<"]["<<compteur<<"] =" << index_mu[k][compteur]<<std::endl;
//                          std::cerr << "index_mu["<<compteur<<"] =" << index_mu[compteur]<<std::endl;
                        }
                        reconstructionError += point_reconstruit.distance( *(worldPoints[i]) );
                        if(Verbose) std::cerr << "Point reconstruit = "<< point_reconstruit[0] <<" "<< point_reconstruit[1] <<" "<<point_reconstruit[2] <<"\n";
                        ++robustnessOK[k];
                        ++nbpt;
                    }
                    ++robustnessTotal[k];
            }
            errorVector[k][compteur] = reconstructionError/(double)nbPoints;
}
        }
    }//boucle compteur
    for(enum_recons=arlCore::ARLCORE_R3D_TWO_LINES_APPROX ,k=0; enum_recons<=arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION; ++enum_recons,++k)
    {
        if(enum_recons == arlCore::ARLCORE_R3D_TWO_LINES_APPROX || enum_recons == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION)
        {
            std::cerr << "index_mu["<<k<<"] size =" << (unsigned int)index_mu[k].size()<<std::endl;
            //for( i=0 ; i<index_mu[k].size() ; ++i )
            //  std::cerr << "index_mu ["<<k<<"] =" << index_mu[k][i]<<std::endl;

            if(enum_recons == arlCore::ARLCORE_R3D_TWO_LINES_APPROX)
            {
                arlCore::WriteTableau ( "PredictionSigma3D_ARLCORE_R3D_TWO_LINES_APPROX", index_mu[k], nbIterations);
                arlCore::KSValidation3D("PredictionSigma3D_ARLCORE_R3D_TWO_LINES_APPROX");
            }

            if(enum_recons == arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION)
            {
                arlCore::WriteTableau ( "PredictionSigma3D_ARLCORE_R3D_REPROJECTION_OPTIMIZATION", index_mu[k], nbIterations);
                arlCore::KSValidation3D("PredictionSigma3D_ARLCORE_R3D_REPROJECTION_OPTIMIZATION");
            }

        }
    }

    bool test = true;
    for(enum_recons=arlCore::ARLCORE_R3D_UNKNOWN+1,i=0; enum_recons<arlCore::ARLCORE_R3D_NBTYPE; ++enum_recons,++i)
    {
        std::string name="Reconstruction error ("+arlCore::ARLCORE_RECONSTRUCTION3D_NAMES[enum_recons]+")";
        test = printErrors(name,errorVector[i],robustnessOK[i],robustnessTotal[i],tolerance) && test;
    }
    return printFooter(nbIterations, test);
}

// testEpipolarMatching
bool arlCore::testEpipolarMatching( long int nbIterations, double tolerance )
{
    const bool Verbose = false;
    const double VolumeSize = 2000.0;
    const double GaussianNoise2D = 0.0;
    const double GaussianNoise3D = 0.0;
    printHeader("Epipolar matching",nbIterations, tolerance);
    arlCore::Point reprojection2D(2);
    const arlCore::Point *p;
    unsigned int i,j,k;
    vnl_vector<double> corrects(nbIterations,0.0);
    vnl_vector<double> nonCorrects(nbIterations,0.0);
    vnl_vector<double> nonApparies(nbIterations,0.0);
    long int robustnessOK=0, robustnessTotal=0;
    long int compteur;
    for( compteur=0 ; compteur<nbIterations ; ++compteur )
    {
        const unsigned int NbPointsModel = (unsigned int)arlRandom::Random::uniformDoubleRnd( 10, 20 );
        //TODO : Gerer outliers 2D
        arlCore::PlaneSystem universe;
        arlCore::SceneUnitTest scene(universe);
        arlCore::SmartPointList spl;
        arlCore::PointList worldPoints;
        worldPoints.shapeRandom(NbPointsModel, arlCore::ARLCORE_SHAPE_SPHERE,arlCore::Point(0.0,0.0,0.0), VolumeSize); //arlCore::ARLCORE_SHAPE_PLAINSQUARE
        const std::vector<arlCore::Camera> &cameras = scene.getCameras().getList();
        if(cameras.size()>=2)
        {   // Il doit y a avoir au moins 2 cameras dans la scene
            const unsigned int NbCameras = 2;
            ++robustnessTotal;
            std::vector< std::vector<const arlCore::Point*> > realMatching(worldPoints.size());
            std::vector< std::vector<arlCore::Point::csptr> > matching;
            // Creation du jeu d'essai
            for( i=0 ; i<worldPoints.size() ; ++i )
                for( j=0 ; j<NbCameras ; ++j )
                {
                    cameras[j].project3DPoint(*(worldPoints[i]), reprojection2D);
                    reprojection2D.addGaussianNoise(GaussianNoise2D);
                    p = spl.push_back(reprojection2D, j+1);
                    realMatching[i].push_back(p);
                }
            arlCore::epipolarMatching( cameras, spl, matching, GaussianNoise2D );
            if(matching.size()>0)
            {   // Est-ce que les appariements obtenus sont corrects ?
                ++robustnessOK;
                for( i=0 ; i<matching.size() ; ++i )
                {
                    bool found = false;
                    for( j=0 ; j<realMatching.size() && !found ; ++j )
                    {
                        bool b=true;
                        for( k=0 ; k<NbCameras ; ++k )
                            b=b && matching[i][k]==realMatching[j][k];
                        found = found || b;
                    }
                    if(!found) ++nonCorrects[compteur]; else ++corrects[compteur];
                }
            }
            nonApparies[compteur]=realMatching.size()-corrects[compteur];
        }
    }//boucle compteur
    vnl_vector<double> total(nbIterations);
    total=corrects+nonCorrects;
    bool test = printErrors("Nombre d'appariements incorrects",nonCorrects,robustnessOK,robustnessTotal,tolerance);
    printErrors("Proportion vrais appariements/total",element_quotient(corrects,total),robustnessOK,robustnessTotal,-1);
    printErrors("Proportion faux appariements/total",element_quotient(nonCorrects,total),robustnessOK,robustnessTotal,-1);
    printErrors("Proportion non apparies/total",element_quotient(nonApparies,total),robustnessOK,robustnessTotal,-1);
    return printFooter(nbIterations, test);
}

// testInitIntrinsicCalibration
bool arlCore::testInitIntrinsicCalibration( long int nbIterations, double tolerance )
{
    const bool Verbose = false;
    const double GaussianNoise2D = 0.0;
    const double GaussianNoise3D = 0.0;
    const unsigned int nb_Cameras_MAX = 1, nb_Poses_MAX=30;
    printHeader("Intrinsic calibration initialization",nbIterations, tolerance);
    vnl_vector_fixed<double,5> errorRotation(0.0);
    vnl_vector_fixed<double,5> errorTranslation(0.0);
    vnl_vector_fixed<double,5> errorIntrinsic(0.0);
    long int robustnessOK=0, robustnessTotal=0;
    long int compteur;
    for( compteur=0 ; compteur<nbIterations ; ++compteur)
    {
        arlCore::PlaneSystem universe;
        std::vector<double> k_range(4);
        k_range[0]=k_range[1]=k_range[2]=k_range[3]=0;
        SceneUnitTestInitIntrinsicCalib scene(universe, k_range, nb_Cameras_MAX, nb_Poses_MAX);
        const unsigned int nbPoses = scene.getNbPoses();
        unsigned int i,j;
        const std::vector< arlCore::Camera > &liste_camera = scene.getCameras().getList();
        const arlCore::Camera &cam = liste_camera[0];
        std::vector< arlCore::PointList > listeWorldPoints(nbPoses);
        //listeWorldPoints[k] contient la geometrie 3D de Tag[k]
        for( i=0 ; i<nbPoses ; ++i )
        {
            const arlCore::PointList &worldPoints = scene.getTags().getTag(i)->getGeometry();
            listeWorldPoints[i] = worldPoints;
        }
        arlCore::vnl_rotation3d_matrix rot_mat;
        std::vector< arlCore::vnl_rigid_matrix > liste_extrinsic_alea(nbPoses);
        for( i=0 ; i<nbPoses ; ++i )
        {   //creation de petites transformations appliquees au damier 3D
            arlCore::vnl_rigid_matrix tmp;
            tmp.uniform_random( 30 );
            liste_extrinsic_alea[i] = tmp;
        }//liste_extrinsic_alea[i] contient 1 transfo aleatoire representant un mouvement du Tag[i]
        std::vector< arlCore::PointList > reprojection2D(nbPoses);
        for( i=0 ; i<nbPoses ; ++i )//pour chaque position du damier on calcule les reprojections 2D
            {   //on applique liste_extrinsic_alea[i] au Tag[i]
                arlCore::PointList modelPoints(listeWorldPoints[i].size() );
                liste_extrinsic_alea[i].trf(listeWorldPoints[i],modelPoints);
                cam.project3DPoint(modelPoints, reprojection2D[i]);
            }

        vnl_matrix_fixed<double,3,3> mat_arrangee;
        for( i=0 ; i<nbPoses ; ++i )// avec notre facon de creer le monde H[i] = IntrinsicMatrix * ExtrinsicMatrixModifie * liste_extrinsic_alea[i]
        {//ExtrinsicMatrixModifie est la matrice 3x3 [r1 r2 t] ou r1 r2 et t sont les 1er, 2nd et 4ieme vecteur colonne de ExtrinsicMatrix
            for( j=0 ; j<3 ; ++j)
                {
                    mat_arrangee(j,0) = (cam.getExtrinsic()* liste_extrinsic_alea[i])(j,0);
                    mat_arrangee(j,1) = (cam.getExtrinsic()* liste_extrinsic_alea[i])(j,1);
                    mat_arrangee(j,2) = (cam.getExtrinsic()* liste_extrinsic_alea[i])(j,3);
                }
                //std::cerr<<"matrice arrangee = "<<mat_arrangee<<std::endl;
                //std::cerr<<"H[i] solution = "<<cam.getIntrinsicMatrix() * mat_arrangee/(cam.getIntrinsicMatrix() * mat_arrangee)(2,2)<<std::endl;
        }
        arlCore::Camera camera_init(universe);
        std::vector< arlCore::vnl_rigid_matrix > extrinsic_init(nbPoses);
        /////////////////////////INITIALISATION
        std::vector<double> log, optimiser_parameters;
        if(arlCore::initIntrinsicCalibration(listeWorldPoints, reprojection2D, camera_init, extrinsic_init, optimiser_parameters, log))
        {   //Calcul des erreurs
            double errorI, errorR=0 ,errorT=0;
            errorI = fabs(camera_init.getfx()-cam.getfx())+fabs(camera_init.getfy()-cam.getfy())
                            + fabs(camera_init.getcx()-cam.getcx())+fabs(camera_init.getcy()-cam.getcy());
            for( i=0 ; i<nbPoses ; ++i )
            {   //verification du calcul des matrices extrinseques
                arlCore::vnl_rigid_vector realT(cam.getExtrinsic()*liste_extrinsic_alea[i]), estimateT(extrinsic_init[i]);
                //std::cerr<<"extrinsic_init["<<i<<"]" << std::endl << extrinsic_init[i] <<std::endl;
                //std::cerr<<"verite extrinsic["<<i<<"]"<< std::endl<<cam.getExtrinsic()*liste_extrinsic_alea[i]<<std::endl;
                errorR += fabs(realT[0]- estimateT[0])+fabs(realT[1]- estimateT[1])+fabs(realT[2]- estimateT[2]);
                errorT += fabs(realT[3]- estimateT[3])+fabs(realT[4]- estimateT[4])+fabs(realT[5]- estimateT[5]);
            }
            addValue(errorTranslation, errorT/3/nbPoses);
            addValue(errorRotation, errorR/3/nbPoses);
            addValue(errorIntrinsic, errorI);
            ++robustnessOK;
        }
        ++robustnessTotal;
    }//boucle compteur
    bool test = printErrors("Erreurs sur la translation extrinseque",errorTranslation,robustnessOK,robustnessTotal,tolerance);
    test = printErrors("Erreur sur la rotation extrinseque",errorRotation,robustnessOK,robustnessTotal,tolerance) && test;
    test = printErrors("Erreur sur les parametres intrinseques",errorIntrinsic,robustnessOK,robustnessTotal,tolerance) && test;
    return printFooter(nbIterations, test);
}

// testIntrinsicCalibration
bool arlCore::testIntrinsicCalibration( long int nbIterations, double tolerance )
{
    const bool Verbose = false;
    const double GaussianNoise2D = 0.0;
    const double GaussianNoise3D = 0.0;
    const unsigned int nbParameters = 8, nb_Cameras_MAX=1, nb_Poses_MAX=30;
    printHeader("Intrinsic calibration",nbIterations, tolerance);

    vnl_vector_fixed<double,5> errorRotation(0.0);
    vnl_vector_fixed<double,5> errorTranslation(0.0);
    vnl_vector_fixed<double,5> errorIntrinsic(0.0);
    long int robustnessOK=0, robustnessTotal=0;
    long int compteur;
    for( compteur=0 ; compteur<nbIterations ; ++compteur)
    {
        //std::cerr<<"Compteur ="<<compteur<<std::endl;
        arlCore::PlaneSystem universe;
        std::vector<double> k_range(4);
        k_range[0]=0.01;k_range[1]=0.01;k_range[2]=0.0001;k_range[3]=0.0001;
//      k_range[0]=0.0;k_range[1]=0.0;k_range[2]=0.0;k_range[3]=0.0;

        SceneUnitTestInitIntrinsicCalib scene(universe, k_range, nb_Cameras_MAX, nb_Poses_MAX);
        const unsigned int nbPoses = scene.getNbPoses();
        unsigned int i,k;

        const arlCore::Camera &cam = scene.getCameras()[0];
        std::vector< arlCore::PointList > listeWorldPoints(nbPoses);//listeWorldPoints[k] contient la geometrie 3D de Tag[k]
        for( k=0 ; k<nbPoses ; ++k )
        {
            const arlCore::PointList &worldPoints = scene.getTags().getTag(k)->getGeometry();
            listeWorldPoints[k] = worldPoints;
        }
        arlCore::Point point2D(2);
        arlCore::vnl_rotation3d_matrix rot_mat;
        std::vector< arlCore::vnl_rigid_matrix > liste_extrinsic_alea(nbPoses);
        for( k=0 ; k<nbPoses ; ++k )
        {   //creation de petites transformations appliquees au damier 3D
            arlCore::vnl_rigid_matrix tmp;
            tmp.uniform_random( 30 );
            liste_extrinsic_alea[k] = tmp;
        }//liste_extrinsic_alea[k] contient 1 transfo aleatoire repr�sentant un mouvement du Tag[k]
        std::vector< arlCore::PointList > reprojection2D(nbPoses);
        std::vector< arlCore::PointList > reprojection2D_bruitee(nbPoses);
        for( k=0 ; k<nbPoses ; ++k )//pour chaque position du damier on calcule les reprojections 2D
        {   //on applique liste_extrinsic_alea[k] au Tag[k]
            arlCore::PointList modelPoints(listeWorldPoints[k].size() );
            liste_extrinsic_alea[k].trf(listeWorldPoints[k],modelPoints);
            cam.project3DPoint(modelPoints, reprojection2D[k]);
            cam.project3DPoint(modelPoints, reprojection2D_bruitee[k]);
        }
        for( k=0 ; k<nbPoses ; ++k )//on rajoute du bruit 2D sur la variable reprojection2D_bruitee
            for( i=0 ; i<scene.getChessNbPoints(k) ; ++i)//cela servira a initialiser imparfaitement pour refineIntrinsicCalibration
            {
                reprojection2D_bruitee[k][i]->addGaussianNoise(GaussianNoise2D);
                //std::cerr<<"reprojection2D_bruitee["<<k<<"]["<<i<<"]="<<reprojection2D_bruitee[k][i]->getString() <<std::endl;
                //std::cerr<<"reprojection2D["<<k<<"]["<<i<<"]="<<reprojection2D[k][i]->getString() <<std::endl;
            }
        vnl_matrix_fixed<double,3,3> mat_arrangee;
        for( k=0 ; k<nbPoses ; ++k )// avec notre facon de creer le monde H[k] = IntrinsicMatrix * ExtrinsicMatrixModifie * liste_extrinsic_alea[k]
        {//ExtrinsicMatrixModifie est la matrice 3x3 [r1 r2 t] ou r1 r2 et t sont les 1er, 2nd et 4ieme vecteur colonne de ExtrinsicMatrix
            for( i=0 ; i<3 ; ++i)
                {
                    mat_arrangee(i,0) = (cam.getExtrinsic()* liste_extrinsic_alea[k])(i,0);
                    mat_arrangee(i,1) = (cam.getExtrinsic()* liste_extrinsic_alea[k])(i,1);
                    mat_arrangee(i,2) = (cam.getExtrinsic()* liste_extrinsic_alea[k])(i,3);
                }
//              std::cerr<<"matrice arrangee = "<<mat_arrangee<<std::endl;
//              std::cerr<<"H[i] solution = "<<cam.getIntrinsicMatrix() * mat_arrangee/(cam.getIntrinsicMatrix() * mat_arrangee)(2,2)<<std::endl;
        }
        arlCore::Camera camera_init(universe);
        std::vector< arlCore::vnl_rigid_matrix > extrinsic_init(nbPoses);
        /////////////////////////INITIALISATION & RAFFINEMENT
        std::vector<double> log, optimiser_parameters;
        optimiser_parameters.push_back(0);//Optimisation method choice 0 =LM, 1=POWELL
        optimiser_parameters.push_back(0);//Verbose choice
//      optimiser_parameters.push_back(1e-10);//set f_tolerance

        if(arlCore::initIntrinsicCalibration(listeWorldPoints, reprojection2D_bruitee, camera_init, extrinsic_init, optimiser_parameters, log))
            if(arlCore::refineIntrinsicCalibration(listeWorldPoints, reprojection2D, nbParameters, camera_init, extrinsic_init, optimiser_parameters, log))
            {   // Calcul des erreurs
                double errorI, errorR=0 ,errorT=0;
                errorI = fabs(camera_init.getfx()-cam.getfx())+fabs(camera_init.getfy()-cam.getfy())
                        + fabs(camera_init.getcx()-cam.getcx())+fabs(camera_init.getcy()-cam.getcy())
                        + fabs(camera_init.getkc(0)-cam.getkc(0))+fabs(camera_init.getkc(1)-cam.getkc(1))
                        + fabs(camera_init.getkc(2)-cam.getkc(2))+fabs(camera_init.getkc(3)-cam.getkc(3));
//              std::cerr<<"error intrinseque ="<<errorI<<std::endl;
//              std::cerr<<"Solution   fx fy cx cy k1 k2 k3 k4 = "<<cam.getfx()<<" "<<cam.getfy()<<" "<<cam.getcx()<<" "<<cam.getcy()<<" "<<cam.getkc(0)<<" "<<cam.getkc(1)<<" "<<cam.getkc(2)<<" "<<cam.getkc(3)<<" "<<std::endl;
//              std::cerr<<"Estimation fx fy cx cy k1 k2 k3 k4 = "<<camera_init.getfx()<<" "<<camera_init.getfy()<<" "<<camera_init.getcx()<<" "<<camera_init.getcy()<<" "<<camera_init.getkc(0)<<" "<<camera_init.getkc(1)<<" "<<camera_init.getkc(2)<<" "<<camera_init.getkc(3)<<" "<<std::endl;
                for( k=0 ; k<nbPoses ; ++k )
                {   //verification du calcul des matrices extrinseques
                    arlCore::vnl_rigid_vector realT(cam.getExtrinsic()*liste_extrinsic_alea[k]), estimateT(extrinsic_init[k]);
//                  std::cerr<<"extrinsic_init["<<k<<"]" << std::endl << extrinsic_init[k] <<std::endl;
//                  std::cerr<<"verite extrinsic["<<k<<"]"<< std::endl<<cam.getExtrinsic()*liste_extrinsic_alea[k]<<std::endl;
                    errorR += fabs(realT[0]- estimateT[0])+fabs(realT[1]- estimateT[1])+fabs(realT[2]- estimateT[2]);
                    errorT += fabs(realT[3]- estimateT[3])+fabs(realT[4]- estimateT[4])+fabs(realT[5]- estimateT[5]);
                }
                addValue(errorTranslation, errorT/3/nbPoses);
                addValue(errorRotation, errorR/3/nbPoses);
                addValue(errorIntrinsic, errorI);
                ++robustnessOK;
            }
            ++robustnessTotal;
    }//boucle compteur
    bool test = printErrors("Erreurs sur la translation extrinseque",errorTranslation,robustnessOK,robustnessTotal,tolerance);
    test = printErrors("Erreur sur la rotation extrinseque",errorRotation,robustnessOK,robustnessTotal,tolerance) && test;
    test = printErrors("Erreur sur les parametres intrinseques",errorIntrinsic,robustnessOK,robustnessTotal,tolerance) && test;
    return printFooter(nbIterations, test);
}

// testInitExtrinsicCalibration
bool arlCore::testInitExtrinsicCalibration( long int nbIterations, double tolerance )
{
// TODO : description du test unitaire
// Ce test unitaire cree une scene contenant plusieurs cameras avec plusieurs poses
// les cameras sont tires dans une sphere ayant pour centre un point proche de l'origine.
// toutes les cameras sont orientes vers ce point (tire dans une sphere de rayon 100 autour de l'origine)
// Les poses sont des objets plans avec Z=0 tires  autour de l'origine
// pour simuler des poses, on applique a ces objets des transformation rigides aleatoires
// avec un faible alea sur les translations (les trsf sont contenues dans liste_extrinsic_alea)
// On calcule ensuite la reprojection des objets rigidement transformes dans les cameras
// On lance alors les algorithmes de calibrage extrinseques et on les compare a la verite terrain.
// Le calcul de la verite terrain se fait grace a la connaissance de la position de chaque camera
// par rapport au repere monde et grace a la connaissance des trsf aleatoires appliques aux objets rigides de calibrage
// On rappelle qu'on cherche a determiner :
// 1) les poses extrinseques Te_k par rapport a la premiere camera tel que
// sum ||P1(Te_k*Mk_i) - mk_i ||^2 soit minimal. P1 correspond a la fonction de projection
// de la premiere camera avec des parametres extrinseque a l'identit� !!
// Mk_i sont les pts 3D du modele dans son repere pour la pose k
// mk_i sont les points 2D reprojetes visibles ET invisibles (ils sont alors NULL) pour la pose k
//
// 2) les transformations Ts_j entre les reperes de chaque camera par rapport a la premiere, tel que
// sum ||Pj(Ts_j*Te_0*M0_i) - m0_j_i ||^2 soit minimal. Pj correspond a la fonction de projection
// de la j ieme camera avec des parametres extrinseque a l'identit� aussi.
// M0_i sont les pts 3D du modele dans son repere pour la pose 0 de la camera j
// m0_j_i sont les pts 3D du modele dans son repere pour la pose 0 de la camera j
//
// Mk_i sont les points des modeles 3D originaux avant applications des trsf aleatoires
// les points reprojetes correspondant dans chaque camera sont les:
// mk_j_i = T_ext_cam_j * T_alea_k * Mk_i
// En l'occurence la verite terrain des Te_k vaut  T_ext_cam_0 * T_alea_k (on rappelle que
// les parametres intrinseques de chaque camera sont consideres a l'identite lors de l'optimisation du critere)
//
// la verite terrain des Ts_j peut se calculer a partir de la pose 0 uniquement (en supposant que
// l'objet pose 0 est visible par toutes les cameras en meme temps) alors:
// mk_j_i = T_ext_cam_j * T_alea_k * Mk_i = Ts_j * Te_0 = Ts_j * T_ext_cam_0 * T_alea_k * Mk_i
// donc    T_ext_cam_j = Ts_j * T_ext_cam_0        donc     Ts_j  = T_ext_cam_j * T_ext_cam_0^(-1)
    const bool verbose = false;
    const double gaussianNoise2D = 0.0;
    const unsigned int nbParameters = 8, nb_Cameras_MAX = 10, nb_Poses_MAX=10;
    printHeader("Extrinsic initialization calibration",nbIterations, tolerance);

    vnl_vector_fixed<double,5> errorRotation(0.0);
    vnl_vector_fixed<double,5> errorTranslation(0.0);
    vnl_vector_fixed<double,5> errorIntrinsic(0.0);
    long int robustnessOK=0, robustnessTotal=0;
    long int compteur;
    for( compteur=0 ; compteur<nbIterations ; ++compteur)
    {
        arlCore::PlaneSystem universe;
        std::vector<double> k_range(4);
        k_range[0]=0.5;k_range[1]=1.0;k_range[2]=0.01;k_range[3]=0.01;

        SceneUnitTestInitIntrinsicCalib scene(universe, k_range, nb_Cameras_MAX, nb_Poses_MAX);
        const unsigned int nbPoses = scene.getNbPoses();
        const unsigned int nbCameras = (unsigned int)scene.getCameras().size();
        unsigned int i,j,k;
        const std::vector< arlCore::Camera > &liste_camera = scene.getCameras().getList();
        std::vector< arlCore::PointList > listeWorldPoints(nbPoses);//listeWorldPoints[k] contient la geometrie 3D de Tag[k]
        for( k=0 ; k<nbPoses ; ++k )
        {
            const arlCore::PointList &worldPoints = scene.getTags().getTag(k)->getGeometry();
            listeWorldPoints[k] = worldPoints;
        }
        arlCore::Point point2D(2);
        arlCore::vnl_rotation3d_matrix rot_mat;
        std::vector< arlCore::vnl_rigid_matrix > liste_extrinsic_alea(nbPoses);
        for( k=0 ; k<nbPoses ; ++k )
        {   //creation de petites transformations appliquees au damier 3D
            arlCore::vnl_rigid_matrix tmp;
            tmp.uniform_random( 30 );
            liste_extrinsic_alea[k] = tmp;
//          std::cerr<<"liste_extrinsic_alea["<<k<<"]="<<liste_extrinsic_alea[k].getString()<<std::endl;
        }//liste_extrinsic_alea[k] contient 1 transfo aleatoire reprzsentant un mouvement du Tag[k]
        std::vector< std::vector< std::vector< arlCore::Point* > > > reprojection2D(nbPoses);
        for( k=0 ; k<nbPoses ; ++k)
        {
            reprojection2D[k].resize(nbCameras);
            arlCore::PointList modelPoints(listeWorldPoints[k].size() );
            liste_extrinsic_alea[k].trf(listeWorldPoints[k],modelPoints);
            for( j=0 ; j<nbCameras ; ++j )//pour chaque position du damier on calcule les reprojections 2D
            {   //on applique liste_extrinsic_alea[k] au Tag[k]
                reprojection2D[k][j].resize(listeWorldPoints[k].size());
//              std::cerr<<"listeWorldPoints[k].size()"<<listeWorldPoints[k].size()<<std::endl;
//              std::cerr<<"reprojection2D["<<k<<"]["<<j<<"].size()"<<reprojection2D[k][j].size()<<std::endl;
                for( i=0 ; i< listeWorldPoints[k].size(); ++i )
                {
                    reprojection2D[k][j][i] = new arlCore::Point(2);
                    //std::cerr<<"*(modelPoints[i])"<<modelPoints[i]->getString()<<std::endl;
                    liste_camera[j].project3DPoint(*(modelPoints[i]), *(reprojection2D[k][j][i]));
                    //std::cerr<<"*(reprojection2D["<<k<<"]["<<j<<"]["<<i<<"])"<<reprojection2D[k][j][i]->getString()<<std::endl;
                }
            }
        }
        for( k=0 ; k<nbPoses ; ++k )//on rajoute du bruit 2D sur la variable reprojection2D_bruitee
            for( j=0 ; j<nbCameras ; ++j )
                for( i=0 ; i<scene.getChessNbPoints(k) ; ++i)//cela servira a initialiser imparfaitement pour refineIntrinsicCalibration
                {
                    reprojection2D[k][j][i]->addGaussianNoise(gaussianNoise2D);
                    //std::cerr<<"reprojection2D_bruitee["<<k<<"]["<<i<<"]="<<reprojection2D_bruitee[k][i]->getString() <<std::endl;
                    //std::cerr<<"reprojection2D["<<k<<"]["<<i<<"]="<<reprojection2D[k][i]->getString() <<std::endl;
                }
        std::vector< arlCore::Camera > liste_camera_identity;
        arlCore::vnl_rigid_matrix identity;
        identity.set_identity();
        for( j=0 ; j<nbCameras ; ++j )
        {
            liste_camera_identity.push_back(arlCore::Camera (liste_camera[j]));
            liste_camera_identity[j].setExtrinsic(identity);
        }
        std::vector< arlCore::vnl_rigid_matrix> init_rigid_trsf((nbCameras-1)+nbPoses);
        std::vector<double> log_init, optimiser_parameters;
        //INITIALISATION DU CALIBRAGE
        arlCore::initExtrinsicCalibration(listeWorldPoints, reprojection2D, liste_camera_identity, init_rigid_trsf, optimiser_parameters, log_init );
        std::vector<arlCore::vnl_rigid_matrix> solution((nbCameras-1)+nbPoses);
        for( k=0 ; k<nbPoses ; ++k )
        {
            arlCore::vnl_rigid_matrix var;
            var.mult(liste_camera[0].getExtrinsic(), liste_extrinsic_alea[k]);
            solution[k] = var;
        }
        for( i=1 ; i<nbCameras ; ++i )
        {
            arlCore::vnl_rigid_matrix var;
            var.mult(liste_camera[i].getExtrinsic(), liste_camera[0].getInvExtrinsic());
            solution[nbPoses+i-1] = var;
        }
        for( i=0 ; i<(nbCameras-1)+nbPoses ; ++i )
        {
            double error_t, error_r;
            init_rigid_trsf[i].compare(solution[i], error_t, error_r);
//          std::cerr<<"erreur t = "<< error_t << "   erreur r = "<< error_r<<std::endl;
        }
        double errorR=0 ,errorT=0;
        for( i=0 ; i<(nbCameras-1)+nbPoses ; ++i )
        {   //verification du calcul des matrices extrinseques
            arlCore::vnl_rigid_vector realT(solution[i]), estimateT(init_rigid_trsf[i]);
            //std::cerr<<"extrinsic_init["<<i<<"]" << std::endl << extrinsic_init[i] <<std::endl;
            //std::cerr<<"verite extrinsic["<<i<<"]"<< std::endl<<cam.getExtrinsic()*liste_extrinsic_alea[i]<<std::endl;
            errorR += fabs(realT[0]- estimateT[0])+fabs(realT[1]- estimateT[1])+fabs(realT[2]- estimateT[2]);
            errorT += fabs(realT[3]- estimateT[3])+fabs(realT[4]- estimateT[4])+fabs(realT[5]- estimateT[5]);
        }
        addValue(errorTranslation, errorT/3/nbPoses);
        addValue(errorRotation, errorR/3/nbPoses);
        ++robustnessOK;
        ++robustnessTotal;
    }//boucle compteur
    bool test = printErrors("Erreurs sur les translations extrinseques",errorTranslation,robustnessOK,robustnessTotal,tolerance);
    test = printErrors("Erreur sur les rotations extrinseques",errorRotation,robustnessOK,robustnessTotal,tolerance) && test;
    return printFooter(nbIterations, test);
}

// testExtrinsicCalibration
bool arlCore::testExtrinsicCalibration( long int nbIterations, double tolerance )
{
// TODO description du test unitaire
// A SAVOIR : visiblement l'algorithme de calibrage extrinseque a du mal a s'arreter
// lorsqu'on cree plus de 8 cameras. Il faudra peut etre ajouter une condition temporelle
// pour forcer l'algorithme a s'arreter lorsque les erreurs stagnent
    const bool Verbose = false;
    const double gaussianNoise2D = 0.0;
    const double gaussianNoise3D = 0.0;
    const unsigned int nbParameters = 8, nb_Cameras_MAX=8, nb_Poses_MAX=10;
    printHeader("Extrinsic calibration",nbIterations, tolerance);

    vnl_vector_fixed<double,5> errorRotation(0.0);
    vnl_vector_fixed<double,5> errorTranslation(0.0);
    vnl_vector_fixed<double,5> errorIntrinsic(0.0);
    long int robustnessOK=0, robustnessTotal=0;
    long int compteur;
    for( compteur=0 ; compteur<nbIterations ; ++compteur)
    {
        arlCore::PlaneSystem universe;
        std::vector<double> k_range(4);
        k_range[0]=0.5;k_range[1]=1.0;k_range[2]=0.01;k_range[3]=0.01;

        SceneUnitTestInitIntrinsicCalib scene(universe, k_range, nb_Cameras_MAX, nb_Poses_MAX);
        const unsigned int nbPoses = scene.getNbPoses();
        const unsigned int nbCameras = (unsigned int)scene.getCameras().size();
        unsigned int i,j,k;
        const std::vector< arlCore::Camera > &liste_camera = scene.getCameras().getList();

        std::vector< arlCore::PointList > listeWorldPoints(nbPoses);//listeWorldPoints[k] contient la geometrie 3D de Tag[k]
        for( i=0 ; i<nbPoses ; ++i )
        {
            const arlCore::PointList &worldPoints = scene.getTags().getTag(i)->getGeometry();
            listeWorldPoints[i] = worldPoints;
        }
        arlCore::Point point2D(2);
        arlCore::vnl_rotation3d_matrix rot_mat;
        std::vector< arlCore::vnl_rigid_matrix > liste_extrinsic_alea(nbPoses);

        for( i=0 ; i<nbPoses ; ++i )
        {   //creation de petites transformations appliquees au damier 3D
            arlCore::vnl_rigid_matrix tmp;
            tmp.uniform_random( 30 );
            liste_extrinsic_alea[i] = tmp;
//          std::cerr<<"liste_extrinsic_alea["<<i<<"]="<<liste_extrinsic_alea[i].getString()<<std::endl;
        }//liste_extrinsic_alea[i] contient 1 transfo aleatoire repr�sentant un mouvement du Tag[k]
        std::vector< std::vector< std::vector< arlCore::Point* > > > reprojection2D(nbPoses);
        std::vector< std::vector< std::vector< arlCore::Point* > > > reprojection2D_bruitee(nbPoses);
        for( k=0 ; k<nbPoses ; ++k)
        {
            reprojection2D[k].resize(nbCameras);
            reprojection2D_bruitee[k].resize(nbCameras);
            arlCore::PointList modelPoints(listeWorldPoints[k].size() );
            liste_extrinsic_alea[k].trf(listeWorldPoints[k],modelPoints);
            for( j=0 ; j<nbCameras ; ++j )//pour chaque position du damier on calcule les reprojections 2D
            {   //on applique liste_extrinsic_alea[k] au Tag[k]
                reprojection2D[k][j].resize(listeWorldPoints[k].size());
//              std::cerr<<"listeWorldPoints[k].size()"<<listeWorldPoints[k].size()<<std::endl;
//              std::cerr<<"reprojection2D["<<k<<"]["<<j<<"].size()"<<reprojection2D[k][j].size()<<std::endl;

                reprojection2D_bruitee[k][j].resize(listeWorldPoints[k].size());
                for( i=0 ; i< listeWorldPoints[k].size(); ++i )
                {
                    reprojection2D[k][j][i] = new arlCore::Point(2);
                    reprojection2D_bruitee[k][j][i] = new arlCore::Point(2);
                    //std::cerr<<"*(modelPoints[i])"<<modelPoints[i]->getString()<<std::endl;
                    liste_camera[j].project3DPoint(*(modelPoints[i]), *(reprojection2D[k][j][i]));
                    liste_camera[j].project3DPoint(*(modelPoints[i]), *(reprojection2D_bruitee[k][j][i]));
                    //std::cerr<<"*(reprojection2D["<<k<<"]["<<j<<"]["<<i<<"])"<<reprojection2D[k][j][i]->getString()<<std::endl;
                }
            }
        }
        for( k=0 ; k<nbPoses ; ++k )//on rajoute du bruit 2D sur la variable reprojection2D_bruitee
            for( j=0 ; j<nbCameras ; ++j )
                for( i=0 ; i<scene.getChessNbPoints(k) ; ++i)//cela servira a initialiser imparfaitement pour refineIntrinsicCalibration
                {
                    reprojection2D_bruitee[k][j][i]->addGaussianNoise(gaussianNoise2D);
                    //std::cerr<<"reprojection2D_bruitee["<<k<<"]["<<i<<"]="<<reprojection2D_bruitee[k][i]->getString() <<std::endl;
                    //std::cerr<<"reprojection2D["<<k<<"]["<<i<<"]="<<reprojection2D[k][i]->getString() <<std::endl;
                }


        std::vector< const arlCore::Camera* > liste_camera_identity;
        arlCore::vnl_rigid_matrix identity;
        identity.set_identity();
        for( j=0 ; j<nbCameras ; ++j )
        {
            arlCore::Camera *var=new arlCore::Camera (liste_camera[j]);
            var->setExtrinsic(identity);
            liste_camera_identity.push_back(var);
        }

        std::vector< arlCore::vnl_rigid_matrix> init_rigid_trsf((nbCameras-1)+nbPoses);
        std::vector<double> log_init, optimiser_parameters;
        const double Method = 0; // 0=LEVENBERG-MARQUARDT 1=POWELL
        optimiser_parameters.push_back(Method);
        //INITIALISATION DU CALIBRAGE
        arlCore::initExtrinsicCalibration(listeWorldPoints, reprojection2D_bruitee, liste_camera_identity, init_rigid_trsf, optimiser_parameters, log_init );
        arlCore::refineExtrinsicCalibration(listeWorldPoints, reprojection2D, liste_camera_identity, init_rigid_trsf, optimiser_parameters, log_init );
        std::vector<arlCore::vnl_rigid_matrix> solution((nbCameras-1)+nbPoses);
        for( i=0 ; i<nbPoses ; ++i )
        {
            arlCore::vnl_rigid_matrix var;
            var.mult(liste_camera[0].getExtrinsic(), liste_extrinsic_alea[i]);
            solution[i] = var;
        }
        for( i=1 ; i<nbCameras ; ++i )
        {
            arlCore::vnl_rigid_matrix var;
            var.mult(liste_camera[i].getExtrinsic(), liste_camera[0].getInvExtrinsic());
            solution[nbPoses+i-1] = var;
        }
        for( i=0 ; i<(nbCameras-1)+nbPoses ; ++i )
        {
            double error_t, error_r;
            init_rigid_trsf[i].compare(solution[i], error_t, error_r);
//          std::cerr<<"erreur t = "<< error_t << "   erreur r = "<< error_r<<std::endl;
        }
        double errorR=0 ,errorT=0;
        for( i=0 ; i<(nbCameras-1)+nbPoses ; ++i )
        {   //verification du calcul des matrices extrinseques
            arlCore::vnl_rigid_vector realT(solution[i]), estimateT(init_rigid_trsf[i]);
            //std::cerr<<"extrinsic_init["<<i<<"]" << std::endl << extrinsic_init[i] <<std::endl;
            //std::cerr<<"verite extrinsic["<<i<<"]"<< std::endl<<cam.getExtrinsic()*liste_extrinsic_alea[i]<<std::endl;
            errorR += fabs(realT[0]- estimateT[0])+fabs(realT[1]- estimateT[1])+fabs(realT[2]- estimateT[2]);
            errorT += fabs(realT[3]- estimateT[3])+fabs(realT[4]- estimateT[4])+fabs(realT[5]- estimateT[5]);
        }
        addValue(errorTranslation, errorT/3/nbPoses);
        addValue(errorRotation, errorR/3/nbPoses);
        ++robustnessOK;
    ++robustnessTotal;
//boucle compteur
    }

    bool test = printErrors("Erreurs sur les translations extrinseques",errorTranslation,robustnessOK,robustnessTotal,tolerance);
    test = printErrors("Erreur sur les rotations extrinseques",errorRotation,robustnessOK,robustnessTotal,tolerance) && test;
    return printFooter(nbIterations, test);

    return 0;
}

// testRigidTransfo
bool arlCore::testRigidTransfo( unsigned int nbIterations, double tolerance )
{
    printHeader("Transformations rigides",nbIterations, tolerance);
    const bool Verbose = false;
    const double cubicSize = 1000;
    unsigned int i;
    vnl_vector_fixed<double,5> error1(0.0), error2(0.0);
    double errorT, errorR;
    vnl_rigid_matrix m1;
    for( i=0 ; i<nbIterations ; ++i )
    {
        vnl_rigid_vector v1(m1);
        vnl_rigid_matrix m2(v1);
        vnl_rigid_vector v2(m2);
        vnl_rigid_matrix m3(v2);
        m1.compare(m3, errorT, errorR);
        addValue(error1, errorT);
        addValue(error2, errorR);
        bool error = (errorT>tolerance || errorR>tolerance);
        if(error && Verbose) std::cerr<<m1.as_matrix()<<"\n";
/*      if(error && Verbose) std::cerr<<v1.as_vector()<<"\n\n";
        if(error && Verbose) std::cerr<<m2.as_matrix()<<"\n";
        if(error && Verbose) std::cerr<<v2.as_vector()<<"\n\n";*/
        if(error && Verbose) std::cerr<<m3.as_matrix()<<"\n";
        if(error && Verbose) std::cerr<<"----------------------------\n";
        m1.uniform_random(cubicSize);
    }
    bool test = true;
    test = printErrors("Translation error",error1,0,0,tolerance,false) && test;
    test = printErrors("Rotation error",error2,0,0,tolerance,false) && test;
    return printFooter(nbIterations, test);
}

// testRegistration3D3DwithoutMatching
bool arlCore::testRegistration3D3DwithoutMatching( long int nbIterations, double tolerance )
{
    const bool Verbose = false;
    // Constantes de recalage
    const double GaussianNoise3D = 0.0;
    const double decimage = 1.0;
    // Constantes de cr�ation du phantom
    const double TransfVolumeSize = 500.0;
    const double VolumeSize = 400.0;
    const unsigned int NbpointsModel = 10;
    const unsigned int NbOutLiers = 2;
    const double ProbaSousEchantillon = 0.8;
    unsigned int i;
    Point point3D(3);
    printHeader("Registration 3D/3D without matching",nbIterations, tolerance);
    vnl_vector<double> errorRotation(nbIterations,0.0);
    vnl_vector<double> errorTranslation(nbIterations,0.0);
    long int robustnessOK=0, robustnessTotal=0;
    long int compteur;
    for( compteur=0 ; compteur<nbIterations ; ++compteur )
    {
        arlCore::PointList modelPoints(3), worldPoints(3), outLiers(3);
        modelPoints.shapeRandom(NbpointsModel, ARLCORE_SHAPE_SPHERE,arlCore::Point(0.0,0.0,0.0), VolumeSize); //ARLCORE_SHAPE_PLAINSQUARE
        arlCore::vnl_rigid_matrix T1, T2;
        T1.uniform_random(TransfVolumeSize);
        for( i=0 ; i<modelPoints.size() ; ++i)
            if(arlRandom::Random::uniformDoubleRnd( 0.0, 1.0 )<ProbaSousEchantillon)
            {
                T1.trf(*(modelPoints[i]), point3D);
                worldPoints.push_back(point3D);
            }
        for( i=0 ; i<worldPoints.size() ; ++i )
            worldPoints[i]->addGaussianNoise(GaussianNoise3D);
        outLiers.shapeRandom(NbOutLiers, ARLCORE_SHAPE_SPHERE, arlCore::Point(0.0,0.0,0.0), VolumeSize);
        worldPoints.push_back(outLiers);
        if(T2.register3D3DwithoutMatching(modelPoints, worldPoints, true, GaussianNoise3D, decimage))
        {
            ++robustnessOK;
            T1.compare(T2, errorTranslation[compteur], errorRotation[compteur]);
        }
        ++robustnessTotal;
    }//boucle compteur
    bool test = printErrors("Erreur sur la translation",errorTranslation,robustnessOK,robustnessTotal,tolerance);
    test = printErrors("Erreur sur la rotation",errorRotation,robustnessOK,robustnessTotal,tolerance) && test;
    return printFooter(nbIterations, test);
}

// test3D3DMatching
bool arlCore::test3D3DMatching( long int nbIterations, double tolerance )
{
    const bool Verbose = false;
    // Constantes de matching
    const double GaussianNoise3D = 0.0;
    const double Decimage = 0.8;
    // Constantes de creation du phantom
    const double TransfVolumeSize = 500.0;
    const double VolumeSize = 400.0;
    const unsigned int NbpointsModel = 15;
    const unsigned int NbOutLiers = 5;
    const double ProbaSousEchantillon = 0.8;

    unsigned int i,j;
    Point point3D(3);
    printHeader("3D/3D matching",nbIterations, tolerance);
    vnl_vector<double> corrects(nbIterations,0.0);
    vnl_vector<double> nonCorrects(nbIterations,0.0);
    vnl_vector<double> nonApparies(nbIterations,0.0);
    long int robustnessOK=0, robustnessTotal=0;
    long int compteur;
    for( compteur=0 ; compteur<nbIterations ; ++compteur )
    {
        std::vector< const arlCore::Point* > realMatchingA, realMatchingB;
        arlCore::PointList modelPoints(3), worldPoints(3), outLiers(3);
        modelPoints.shapeRandom(NbpointsModel, ARLCORE_SHAPE_SPHERE,arlCore::Point(0.0,0.0,0.0), VolumeSize); //ARLCORE_SHAPE_PLAINSQUARE
        arlCore::vnl_rigid_matrix T1;
        T1.uniform_random(TransfVolumeSize);
        for( i=0 ; i<modelPoints.size() ; ++i)
            if(arlRandom::Random::uniformDoubleRnd( 0.0, 1.0 )<ProbaSousEchantillon)
            {
                T1.trf(*(modelPoints[i]), point3D);
                realMatchingA.push_back(modelPoints[i]);
                worldPoints.push_back(point3D);
                realMatchingB.push_back(worldPoints.back());
            }
        for( i=0 ; i<worldPoints.size() ; ++i )
            worldPoints[i]->addGaussianNoise(GaussianNoise3D);
        outLiers.shapeRandom(NbOutLiers, ARLCORE_SHAPE_SPHERE, arlCore::Point(0.0,0.0,0.0), VolumeSize);
        worldPoints.push_back(outLiers);
        std::vector< const arlCore::Point* > Va, Vb;
        assert(realMatchingA.size()==realMatchingB.size());
        assert(Va.size()==Vb.size());
        if(arlCore::matching3D3D(modelPoints, worldPoints, GaussianNoise3D, Decimage, Va, Vb)>0)
        {   // Est-ce que les appariements obtenus sont corrects ?
            ++robustnessOK;
            for( i=0 ; i<Va.size() ; ++i )
            {
                bool found = false;
                for( j=0 ; j<realMatchingA.size() && !found ; ++j )
                    found = found || (realMatchingA[j]==Va[i] && realMatchingB[j]==Vb[i]);
                if(!found) ++nonCorrects[compteur]; else ++corrects[compteur];
            }
        }
        nonApparies[compteur]=realMatchingA.size()-corrects[compteur];
        ++robustnessTotal;
    }//boucle compteur
    vnl_vector<double> total(nbIterations);
    total=corrects+nonCorrects;

    bool test = printErrors("Nombre d'appariements incorrects",nonCorrects,robustnessOK,robustnessTotal,tolerance);
    printErrors("Proportion vrais appariements/total",element_quotient(corrects,total),robustnessOK,robustnessTotal,-1);
    printErrors("Proportion faux appariements/total",element_quotient(nonCorrects,total),robustnessOK,robustnessTotal,-1);
    printErrors("Proportion non apparies/total",element_quotient(nonApparies,total),robustnessOK,robustnessTotal,-1);
    return printFooter(nbIterations, test);
}

    // The 9 first parameters are used for the initialization of SceneCriterionComparison:
    // number of point (parameters[0])
    // number of camera  (parameters[1])
    // angle between the camera  (parameters[2])
    // size of the shape in which the 3D points are randomly chosen  (parameters[3])
    // shape of the random 3D points  (parameters[4])
    // number of control point  (parameters[5])
    // distance between the control point center and the registration point   (parameters[6])
    // size of the shape in which the control points are randomly chosen  (parameters[7])
    // shape of the random control points   (parameters[8])
    //
    // Then the other parameters of the experiments (variable or static) are found at the end of the vector
    // noise on 3D points (parameters[9][10][11])
    // noise on 2D points (parameters[12][13])
    // information on the initilisation rigid transformation (0=close to the solution; 1=identity; 2=randomly chosen)(parameters[14])
bool arlCore::testRegistrationCriteriaComparison( long int nbIterations, std::vector<double> parameters, std::vector<double> staticStatus,
                                                std::vector< vnl_vector<double>* > erreur_recalage_methode, std::vector< vnl_vector<double>* > duree_recalage_methode,
                                                 std::vector< vnl_vector<double>* > robustesse_recalage_methode )
{
    const bool Verbose = false;
    const unsigned int nbMethode = 12;
    erreur_recalage_methode.resize(nbMethode);
    duree_recalage_methode.resize(nbMethode);
    robustesse_recalage_methode.resize(nbMethode);
    const double TransfVolumeSize = 500.0;
    const double rangeGaussianNoise3D_X = parameters[9], rangeGaussianNoise3D_Y = parameters[10], rangeGaussianNoise3D_Z = parameters[11];
    const double rangeGaussianNoise2D_X = parameters[12], rangeGaussianNoise2D_Y = parameters[13];
    double GaussianNoise3D_X, GaussianNoise3D_Y, GaussianNoise3D_Z, GaussianNoise2D_X, GaussianNoise2D_Y, RSB_3D, RSB_2D;
    const unsigned int nbTypes = arlCore::ARLCORE_PR_NBTYPES;
    //printHeader("Projective registration",nbIterations, tolerance);
//  std::vector< vnl_vector<double> > erreur_recalage_methode(nbMethode);
//  std::vector< vnl_vector<double> > duree_recalage_methode(nbMethode);
    std::vector< vnl_vector_fixed<double,5> > errorRotation(nbTypes);
    std::vector< vnl_vector_fixed<double,5> > errorTranslation(nbTypes);
    std::vector<long int> robustnessOK(nbTypes), robustnessTotal(nbTypes);
    long int compteur;
    arlCore::Object registrationDuration;
    unsigned int enum_3D2D, enum_3D3D, i, plane_reconstruction;
    long int j;
    std::vector<double> log_reconstruction, registrationError;
    for( i=0 ; i<nbMethode ; ++i )
    {
        erreur_recalage_methode[i] = new vnl_vector<double>(nbIterations);
        duree_recalage_methode[i] = new vnl_vector<double>(nbIterations);
        robustesse_recalage_methode[i] = new vnl_vector<double>(nbIterations);
    }
    for( compteur=0 ; compteur<nbIterations ; ++compteur)
    {
        //std::cerr<<"ITERATION No "<<compteur<<std::endl;
        //Scene creation
        arlCore::PlaneSystem universe;
        std::vector<double> noiseValues(5);
        //do{
        //Noise computation
        if(staticStatus[9] == 0 && staticStatus[10] == 0 && staticStatus[11] == 0 ){
        GaussianNoise3D_X = fabs(arlRandom::Random::gaussianRnd(rangeGaussianNoise3D_X));
        GaussianNoise3D_Y = fabs(arlRandom::Random::gaussianRnd(rangeGaussianNoise3D_Y));
        GaussianNoise3D_Z = fabs(arlRandom::Random::gaussianRnd(rangeGaussianNoise3D_Z));}
        else{
        GaussianNoise3D_X = rangeGaussianNoise3D_X;
        GaussianNoise3D_Y = rangeGaussianNoise3D_Y;
        GaussianNoise3D_Z = rangeGaussianNoise3D_Z;}
        if(Verbose) std::cerr<<"GaussianNoise3D_X ="<<GaussianNoise3D_X<<std::endl<<"GaussianNoise3D_Y ="<<GaussianNoise3D_Y<<std::endl<<"GaussianNoise3D_Z ="<<GaussianNoise3D_Z<<std::endl;
        if(staticStatus[12] == 0 && staticStatus[13] == 0){
        GaussianNoise2D_X = fabs(arlRandom::Random::gaussianRnd(rangeGaussianNoise2D_X));
        GaussianNoise2D_Y = fabs(arlRandom::Random::gaussianRnd(rangeGaussianNoise2D_Y));}
        else{
        GaussianNoise2D_X = rangeGaussianNoise2D_X;
        GaussianNoise2D_Y = rangeGaussianNoise2D_Y;}
        if(Verbose) std::cerr<<"GaussianNoise2D_X ="<<GaussianNoise2D_X<<std::endl<<"GaussianNoise2D_Y ="<<GaussianNoise2D_Y<<std::endl;
        noiseValues[0] = GaussianNoise3D_X; noiseValues[1] = GaussianNoise3D_Y; noiseValues[2] = GaussianNoise3D_Z;
        noiseValues[3] = GaussianNoise2D_X; noiseValues[4] = GaussianNoise2D_Y;
        //arlCore::SceneCriterionComparison *scene_test=0;
        //if(scene_test != 0)
        //  delete scene_test;
        //scene_test = new arlCore::SceneCriterionComparison(universe, parameters, staticStatus, noiseValues, RSB_3D, RSB_2D);
        //}while(RSB_3D < 35 || RSB_2D < 35);exit(0);
        arlCore::SceneCriterionComparison scene(universe, parameters, staticStatus, noiseValues, RSB_3D, RSB_2D);
        const unsigned int nbCameras = (unsigned int)scene.getCameras().size();
        const unsigned int nbModelPoints = scene.getTags().getTag(0)->getGeometry().size();
        const unsigned int nbControlPoints = scene.getTags().getTag(1)->getGeometry().size();
        const std::vector<arlCore::Camera> &cameras = scene.getCameras().getList();
        const arlCore::PointList &regPoints     = scene.getTags().getTag(0)->getGeometry();
        const arlCore::PointList &controlPoints = scene.getTags().getTag(1)->getGeometry();
        std::vector< std::vector<arlCore::Point::csptr> >reprojection2D(cameras.size());
        arlCore::SmartPointList  splMonde;
        for( i=0 ; i<cameras.size() ; ++i )
        {
            scene.detection( i+1, 0, splMonde, 0.0 );
            splMonde.getPoints(reprojection2D[i], i+1, (void*) scene.getTags().getTag(0));
        }
        //Step 0 : creation of the seeked transformation T2 (= T1^-1 )
        // creation of the initialisation
        // application of T1 to modelPoints and controlModelPoints
        arlCore::vnl_rigid_matrix T1,T2;
        T1.uniform_random(TransfVolumeSize);
        arlCore::PointList modelPoints(3), controlModelPoints(3);
        T1.trf(regPoints, modelPoints);
        T1.trf(controlPoints, controlModelPoints);
        T2.invert(T1);

        std::vector< vnl_rigid_matrix > T_list; //list that contains the solution matrix
        vnl_rigid_matrix trf_initialisation;
        if(parameters[14] == 0)
        {
            trf_initialisation = T2;
            trf_initialisation.setTranslation( trf_initialisation(0,3)+arlRandom::Random::uniformDoubleRnd(0,3), trf_initialisation(1,3)+arlRandom::Random::uniformDoubleRnd(0,3),trf_initialisation(2,3)+arlRandom::Random::uniformDoubleRnd(0,3));
        }
        if(parameters[14] == 1)
            trf_initialisation.set_identity();
        if(parameters[14] == 2)
            trf_initialisation.uniform_random(TransfVolumeSize);

        //Step 1 : creation of the noisy 3D points
        arlCore::PointList modelPointsNoisy(3);
        for( i=0 ; i<modelPoints.size() ; ++i )
        {
            arlCore::Point tmp(3);
            tmp.set(0, modelPoints[i]->x() + arlRandom::Random::gaussianRnd(GaussianNoise3D_X) );
            tmp.set(1, modelPoints[i]->y() + arlRandom::Random::gaussianRnd(GaussianNoise3D_Y) );
            tmp.set(2, modelPoints[i]->z() + arlRandom::Random::gaussianRnd(GaussianNoise3D_Z) );
            modelPointsNoisy.push_back(tmp);
            if(Verbose) std::cerr<<"modelPointsNoisy["<<i<<"]"<<modelPointsNoisy[i]->getCoordinates()<<std::endl;
            arlCore::vnl_covariance_matrix &cov_mat = modelPointsNoisy[i]->getCovMatrix();
            cov_mat.fill(0.0);
            cov_mat.put(0,0,GaussianNoise3D_X*GaussianNoise3D_X);
            cov_mat.put(1,1,GaussianNoise3D_Y*GaussianNoise3D_Y);
            cov_mat.put(2,2,GaussianNoise3D_Z*GaussianNoise3D_Z);
            if(Verbose) std::cerr<<"cov mat 3D["<<i<<"] ="<< modelPointsNoisy[i]->getCovMatrix()<<std::endl;
        }
        //Step 2 : creation of the noisy 2D points
        std::vector< std::vector<arlCore::Point::csptr> >reprojection2DNoisy(cameras.size());
        for( j=0 ; j<(long int)nbCameras ; ++j)
        {
            //reprojection2DNoisy[j].setDimension(2);
            for( i=0 ; i<nbModelPoints ; ++i )
            {
                arlCore::Point *tmp;
                tmp = new arlCore::Point;
                reprojection2DNoisy[j].push_back(tmp);
                ((Point::sptr)reprojection2DNoisy[j][i])->init(2);
                ((Point::sptr)reprojection2DNoisy[j][i])->set(0, reprojection2D[j][i]->x() + arlRandom::Random::gaussianRnd(GaussianNoise2D_X));
                ((Point::sptr)reprojection2DNoisy[j][i])->set(1, reprojection2D[j][i]->y() + arlRandom::Random::gaussianRnd(GaussianNoise2D_Y));
                if(Verbose) std::cerr<<"reprojection2D["<<j<<"]["<<i<<"]"<<reprojection2D[j][i]->getCoordinates()<<std::endl;
                if(Verbose) std::cerr<<"reprojection2DNoisy["<<j<<"]["<<i<<"]"<<reprojection2DNoisy[j][i]->getCoordinates()<<std::endl;
                arlCore::vnl_covariance_matrix &cov_mat = ((Point::sptr)reprojection2DNoisy[j][i])->getCovMatrix();
                cov_mat.fill(0.0);
                cov_mat.put(0,0,GaussianNoise2D_X*GaussianNoise2D_X);
                cov_mat.put(1,1,GaussianNoise2D_Y*GaussianNoise2D_Y);
                if(Verbose) std::cerr<<"cov mat 2D["<<i<<"] ="<< reprojection2DNoisy[j][i]->getCovMatrix()<<std::endl;
            }
        }
        //Step 3 : reconstruction of the 2D points for 3D/3D registration
        arlCore::PointList reconstructedPoints_Lines(3), reconstructedPoints_Opt(3);
        for( i=0 ; i<regPoints.size() ; ++i )
        {
            arlCore::Point tmp;
            reconstructedPoints_Lines.push_back(tmp);
            reconstructedPoints_Opt.push_back(tmp);
            std::vector<Point::csptr> tmp_2D_point_list;
            for( j=0 ; j<(long int)cameras.size() ; ++j )
                tmp_2D_point_list.push_back(reprojection2DNoisy[j][i]);

            registrationDuration.startLap();
            reconst3D( tmp_2D_point_list, cameras, *(reconstructedPoints_Lines[i]), arlCore::ARLCORE_R3D_MULTI_LINES_APPROX, plane_reconstruction, log_reconstruction);
            (*duree_recalage_methode[0])[compteur] = registrationDuration.getLap();
            if(Verbose) std::cerr<<"reconstructedPoints_Lines["<<i<<"]"<<reconstructedPoints_Lines[i]->getCoordinates()<<std::endl;
            registrationDuration.startLap();
            reconst3D( tmp_2D_point_list, cameras, *(reconstructedPoints_Opt[i])  , arlCore::ARLCORE_R3D_REPROJECTION_OPTIMIZATION_UNCERTAINTY, plane_reconstruction, log_reconstruction, -1, true);
            (*duree_recalage_methode[1])[compteur] = registrationDuration.getLap();
            (*duree_recalage_methode[2])[compteur] = registrationDuration.getLap();
            (*duree_recalage_methode[3])[compteur] = registrationDuration.getLap();
            if(Verbose) std::cerr<<"reconstructedPoints_Opt["<<i<<"]"<<reconstructedPoints_Opt[i]->getCoordinates()<<std::endl;
        }

        //Step 4 : computation of the 3D/3D registration
        T_list.push_back(trf_initialisation);
        registrationDuration.startLap();
        T_list[0].register3D3D(modelPointsNoisy, reconstructedPoints_Lines, true);
        (*duree_recalage_methode[0])[compteur] += registrationDuration.getLap();// += because we take into account the time to reconstruct the points
        if(T_list[0] == trf_initialisation)//check the robustness : has it converged or not ?
                (*robustesse_recalage_methode[0])[compteur] = 1;

        if(Verbose) std::cerr<<"T1 ="<<T1<<std::endl;
        //std::cerr<<"T solution ="<<T2<<std::endl;
        if(Verbose) std::cerr<<"T_list[0] ="<<T_list[0]<<std::endl;
        T_list.push_back(trf_initialisation);
        registrationDuration.startLap();
        T_list[1].register3D3D(modelPointsNoisy, reconstructedPoints_Opt, true);
        (*duree_recalage_methode[1])[compteur] += registrationDuration.getLap();// += because we take into account the time to reconstruct the points
        if(T_list[1] == trf_initialisation)//check the robustness : has it converged or not ?
                (*robustesse_recalage_methode[1])[compteur] = 1;
        if(Verbose) std::cerr<<"Recalage 3D3D : methode directe"<<std::endl;
        //std::cerr<<"T_list[1] ="<<T_list[1]<<std::endl;}

        for(enum_3D3D = arlCore::ARLCORE_REGISTER3D3D_UNKNOWN+1,i=0; enum_3D3D <arlCore::ARLCORE_REGISTER3D3D_GC; ++enum_3D3D,++i)
        {
            std::vector< double > optimise_param, log_3D3D;
            std::string name="Recalage 3D3D : methode "+arlCore::ARLCORE_REGISTER3D3D_NAMES[i+1];
            if(Verbose) std::cerr<<name<<std::endl;
            T_list.push_back(trf_initialisation);
            registrationDuration.startLap();
            T_list[2+i].register3D3DUncertainty(modelPointsNoisy, reconstructedPoints_Opt, (arlCore::ARLCORE_REGISTER3D3D)enum_3D3D, optimise_param, log_3D3D/*, modelPoints.size()*/ );
            (*duree_recalage_methode[2+i])[compteur] += registrationDuration.getLap();// += because we take into account the time to reconstruct the points
            if(Verbose)
            {
                for(j=0;j<(long int)log_3D3D.size();++j)
                    std::cerr<<"log["<<j<<"] ="<<log_3D3D[j]<<std::endl;
                std::cerr<<"T_list[2+i] ="<<T_list[2+i]<<std::endl;
            }
            if(T_list[2+i] == trf_initialisation)//check the robustness : has it converged or not ?
                (*robustesse_recalage_methode[2+i])[compteur] = 1;
        }
        //Step 5 : computation of the 3D/2D registration
        int offset = (int)T_list.size();
        if(Verbose) std::cerr<<"T_list.size() ="<<(unsigned int)T_list.size()<<std::endl;
        for(enum_3D2D=arlCore::ARLCORE_PR_UNKNOWN+1,i=0; enum_3D2D <arlCore::ARLCORE_PR_ISPPC_CG; ++enum_3D2D,++i)
        //for(enum_3D2D=arlCore::ARLCORE_PR_UNKNOWN+1,i=0; enum_3D2D <arlCore::ARLCORE_PR_EPPC; ++enum_3D2D,++i)
        {
            std::string name="Recalage 3D2D : methode "+arlCore::ARLCORE_PROJECTIVE_REGISTRATION_NAMES[i+1];
            if(Verbose) std::cerr<<name<<std::endl;
            T_list.push_back(trf_initialisation);
            //std::cerr<<"T_list[offset+i] ="<<T_list[offset+i]<<std::endl;
            std::vector<double> log, param_optimisation;
            registrationDuration.startLap();
            arlCore::multiViewPointRegistration3D2D( cameras, reprojection2DNoisy, modelPointsNoisy,
                T_list[offset+i], (arlCore::ARLCORE_PROJECTIVE_REGISTRATION)enum_3D2D, param_optimisation, log, 1);
            (*duree_recalage_methode[offset+i])[compteur] = registrationDuration.getLap();
//          {
//              T1.compareInverse(T2, errorT, errorR);
//              addValue(errorTranslation[i], errorT);
//              addValue(errorRotation[i], errorR);
//              ++robustnessOK[i];
//          }
            if(Verbose)
            {
                for(j=0;j<(long int)log.size();++j)
                    std::cerr<<"log["<<j<<"] ="<<log[j]<<std::endl;
                std::cerr<<"T_list["<<offset+i<<"] ="<<T_list[offset+i]<<std::endl;
            }
            if(T_list[2+i] == trf_initialisation)//check the robustness : has it converged or not ?
                (*robustesse_recalage_methode[2+i])[compteur] = 1;
//          ++robustnessTotal[i];
        }
        //Step 6 : computation of registration error of each method using the control points
        if(Verbose) std::cerr<< "CALCUL DES ERREURS "<<std::endl;
        for(i=0;i<T_list.size();++i)
        {
            double error = 0;
            arlCore::PointList temp_points(3);
            arlCore::Point toto(3);
            for( j=0 ; j<(long int)controlModelPoints.size() ; ++j )
                temp_points.push_back(toto);
            T_list[i].trf(controlModelPoints,temp_points);
            for( j=0 ; j<(long int)controlModelPoints.size() ; ++j )
                error += temp_points[j]->distance(*controlPoints[j]);
            (*erreur_recalage_methode[i])[compteur] = error;
            //std::cerr<< "Erreur de recalage methode ["<<i<<"]="<<registrationError[i] <<std::endl;
        }
    }//boucle compteur

    for(enum_3D3D = arlCore::ARLCORE_REGISTER3D3D_UNKNOWN+1,i=0; enum_3D3D <arlCore::ARLCORE_REGISTER3D3D_GC; ++i)
    {
        if(i==0){
        std::string name="Recalage 3D3D : methode directe avec reconstruction droite";
        std::cerr<<name<<std::endl;}
        else {if(i==1){
        std::string name="Recalage 3D3D : methode directe avec reconstruction optimale";
        std::cerr<<name<<std::endl;}
        else{
        std::string name="Recalage 3D3D : methode "+arlCore::ARLCORE_REGISTER3D3D_NAMES[i-1];
        std::cerr<<name<<std::endl;++enum_3D3D;}}

        vnl_vector<double> sub(nbIterations);
        for( j=0 ; j<nbIterations ; ++j )
        {
            if(Verbose){std::cerr<< "Erreur de recalage methode ["<<i<<"]="<<(*erreur_recalage_methode[i])[j] <<std::endl;}
            sub[j] = (*erreur_recalage_methode[i])[j] - erreur_recalage_methode[i]->mean();
        }
        std::cerr<< "Erreur MOYENNE de recalage methode ["<<i<<"]="<<erreur_recalage_methode[i]->mean() <<std::endl;
        std::cerr<< "Erreur MAX de recalage methode ["<<i<<"]="<<erreur_recalage_methode[i]->max_value() <<std::endl;
        std::cerr<< "Ecart-type de recalage methode ["<<i<<"]="<<sub.rms() <<std::endl;
        std::cerr<<std::setprecision(10)<< "Duree MOYENNE de recalage methode ["<<i<<"]="<<duree_recalage_methode[i]->mean() <<std::endl<<std::endl;
        std::cerr<< "Robustesse methode ["<<i<<"]="<<robustesse_recalage_methode[i]->sum() <<std::endl<<std::endl;
    }
    int offset2 = i;
    std::cerr<<i<<std::endl;
    //for(enum_3D2D=arlCore::ARLCORE_PR_UNKNOWN+1,i=0; enum_3D2D <arlCore::ARLCORE_PR_EPPC; ++enum_3D2D,++i)
    for(enum_3D2D=arlCore::ARLCORE_PR_UNKNOWN+1,i=0; enum_3D2D <arlCore::ARLCORE_PR_ISPPC_CG; ++enum_3D2D,++i)
    {
        std::string name="Recalage 3D2D : methode "+arlCore::ARLCORE_PROJECTIVE_REGISTRATION_NAMES[i+1];
        std::cerr<<name<<std::endl;
        vnl_vector<double> sub(nbIterations);
        for( j=0 ; j<nbIterations ; ++j)
        {
            if(Verbose){std::cerr<< "Erreur de recalage methode ["<<i<<"]="<<(*erreur_recalage_methode[i+offset2])[j] <<std::endl;}
            sub[j] = (*erreur_recalage_methode[i+offset2])[j] - erreur_recalage_methode[i+offset2]->mean();
        }
        std::cerr<< "Erreur MOYENNE de recalage methode ["<<i<<"]="<<erreur_recalage_methode[i+offset2]->mean() <<std::endl;
        std::cerr<< "Erreur MAX de recalage methode ["<<i<<"]="<<erreur_recalage_methode[i+offset2]->max_value() <<std::endl;
        std::cerr<< "Ecart-type de recalage methode ["<<i<<"]="<<sub.rms() <<std::endl;
        std::cerr<<std::setprecision(10)<< "Duree MOYENNE de recalage methode ["<<i<<"]="<<duree_recalage_methode[i+offset2]->mean() <<std::endl<<std::endl;
        std::cerr<< "Robustesse methode ["<<i<<"]="<<robustesse_recalage_methode[i+offset2]->sum() <<std::endl<<std::endl;
    }
    bool test=true;
//  for(enum_3D2D=arlCore::ARLCORE_PR_ISPPC,i=0; enum_3D2D <arlCore::ARLCORE_PR_NBTYPES; ++enum_3D2D,++i)
//  {
//      std::string name="Translation error ("+arlCore::ARLCORE_PROJECTIVE_REGISTRATION_NAMES[enum_3D2D]+")";
//      //test = printErrors(name,errorTranslation[i],robustnessOK[i],robustnessTotal[i],tolerance) && test;
//      name="Rotation error ("+arlCore::ARLCORE_PROJECTIVE_REGISTRATION_NAMES[enum_3D2D]+")";
//      //test = printErrors(name,errorRotation[i],robustnessOK[i],robustnessTotal[i],tolerance) && test;
//  }
    return printFooter(nbIterations, test);
}

// testSphereCenterEstimation
bool arlCore::testSphereCenterEstimation( long int nbIterations, double tolerance )
{
    const bool Verbose = false;
    const double GaussianNoise3D = 0.0; //0.001;
    std::vector< vnl_vector_fixed<double,5> >errorRadius;
    printHeader("Sphere center estimation from sparse point on its surface",nbIterations, tolerance);
    double error;
    std::vector<long int> robustnessOK, robustnessTotal;
    long int i, compteur;
    unsigned int enum_method;
    for(enum_method=arlCore::ARLCORE_SCE_UNKNOWN+1,i=0; enum_method <arlCore::ARLCORE_SCE_NBMETHOD; ++enum_method, ++i)
    {
        for( compteur=0 ; compteur<nbIterations ; ++compteur )
        {
            if( compteur==0 )
            {
                vnl_vector_fixed<double,5> tmp(0.0);
                errorRadius.push_back(tmp);
                robustnessOK.push_back(0);
                robustnessTotal.push_back(0);
            }
            unsigned int nbPoints = (unsigned int)floor( arlRandom::Random::uniformDoubleRnd(4, 50) );
            arlCore::PointList surface_points(3), surface_points_rotated(3);
            arlCore::Point center(0,0,0), center_estimation(0,0,0);
            double diameter, angle, radius_estimation;
            std::vector<double> optimiser_parameter, log;
            arlCore::vnl_rigid_matrix T;
            T.uniform_random(0.0);
            diameter = arlRandom::Random::uniformDoubleRnd(20, 500);
            angle =  arlRandom::Random::uniformDoubleRnd(10, 360);
            center.shapeRandom(arlCore::ARLCORE_SHAPE_CUBE,1000);
            if(Verbose) std::cerr<< "nbPoints = "<<nbPoints <<std::endl;
            if(Verbose) std::cerr<< "diameter/2 = "<<diameter/2 <<std::endl;
            surface_points.shapeRandom(nbPoints, arlCore::ARLCORE_SHAPE_SPHERE_CAP, center, diameter, angle );
            T.trf(surface_points, surface_points_rotated);
            T.trf(center);
            //if(Verbose) std::cerr<< "centre = "<<center.print() <<std::endl;
            for( unsigned int i=0 ; i<surface_points_rotated.size() ; ++i )
                surface_points_rotated[i]->addGaussianNoise(GaussianNoise3D);
            arlCore::ARLCORE_SCE type = (arlCore::ARLCORE_SCE)enum_method;
            if( surface_points_rotated.sphereCenterEstimation( center_estimation, radius_estimation, type, optimiser_parameter, log) )
            {
                ++robustnessOK[i];
                //if(Verbose) std::cerr<< "centre_estimation = "<<center_estimation.print()<<std::endl;
                error = center_estimation.distance(center);
                addValue( errorRadius[i], error );
    /*          std::cerr<< "Erreur d'estimation centre sphere = " <<error<<std::endl;
                std::cerr<< "Rayon estime = " <<radius_estimation<<std::endl;
                std::cerr<< "log[0] = " <<log[0]<<std::endl;
                std::cerr<< "log[1] = " <<log[1]<<std::endl;
                std::cerr<< "log[2] = " <<log[2]<<std::endl;*/
            }
            ++robustnessTotal[i];
        }//boucle compteur
    }
    bool test = true;
    for(enum_method=arlCore::ARLCORE_SCE_UNKNOWN+1,i=0; enum_method<arlCore::ARLCORE_SCE_NBMETHOD; ++enum_method,++i)
    {
        std::string name="Radius error ("+arlCore::ARLCORE_SCE_NAMES[enum_method]+")";
        test = printErrors(name,errorRadius[i],robustnessOK[i],robustnessTotal[i],tolerance) && test;
    }
    return printFooter(nbIterations, test);
}

// testPolynomialFieldDistortion
bool arlCore::testPolynomialFieldDistortion( long int nbIterations, double tolerance )
{
    const bool Verbose = false;
    const double GaussianNoise3D = 0.0; //0.001;
    const double CubeRadius = 4.0;
    std::vector< vnl_vector_fixed<double,5> >errorCorrection;
    printHeader("Polynomial field distorsion",nbIterations, tolerance);
    std::vector<long int> robustnessOK, robustnessTotal;
    long int n=0, compteur;
    unsigned int i, j, k;
    unsigned int interpolationDegree, interpolationDegreeMin=2, interpolationDegreeMax=3;
    assert(interpolationDegreeMin<=interpolationDegreeMax);
    vnl_vector<double> coordinates(3), distorsion(3);
    for( interpolationDegree=interpolationDegreeMin ; interpolationDegree<=interpolationDegreeMax ; ++interpolationDegree, ++n )
    {

        for( compteur=0 ; compteur<nbIterations ; ++compteur )
        {
            if( compteur==0 )
            {
                vnl_vector_fixed<double,5> tmp(0.0);
                errorCorrection.push_back(tmp);
                robustnessOK.push_back(0);
                robustnessTotal.push_back(0);
            }
            const unsigned int NbEquations = 3;
            arlCore::PointList real;
            vnl_vector_fixed<double,3> ResGrid(0.6);
            for( i=2 ; i<CubeRadius/ResGrid[0] ; ++i )
                for( j=2 ; j<CubeRadius/ResGrid[1] ; ++j )
                    for( k=2 ; k<CubeRadius/ResGrid[2] ; ++k )
                        real.push_back(arlCore::Point( ResGrid[0]*(double)i, ResGrid[1]*(double)j, ResGrid[2]*(double)k));
            // Distorsion model
            const unsigned int modelDegree = 2;
            const double ModelNoise = 5.0;
            vnl_vector<double>model(nbPolynomialParameters(modelDegree, NbEquations));
            for( i=0 ; i<model.size() ; ++i )
                model[i] = arlRandom::Random::gaussianRnd( ModelNoise );
            arlCore::PointList distorded(real);
            for( i=0 ; i<real.size() ; ++i )
            {
                for( j=0 ; j<3 ; ++j )
                    coordinates[j] = real[i]->get(j);
                arlCore::computePolynomial( modelDegree, model, coordinates, distorsion );
                for( j=0 ; j<3 ; ++j )
                    real[i]->set(j, real[i]->x() - distorsion[j]);
            }
            // Estimation of model of deformation from synthetic points
            vnl_vector<double> parameters;
            double RMS;
            fieldCalibration( real, distorded, interpolationDegree, parameters, RMS );
            // Correct points with estimated model
            FieldCorrector fc( interpolationDegree );
            fc.setParameters( parameters );
            fc.activCorrection( true );
            for( i=0 ; i<distorded.size() ; ++i )
            {
                arlCore::Point undistorded(3);
                bool b = fc.correct( *distorded[i], undistorded );
                addValue( errorCorrection[n], undistorded.distance(*real[i]) );

            }
            ++robustnessOK[n];
            ++robustnessTotal[n];
        }//boucle compteur
    }
    bool test = true;
    for( i=0 ; i<=interpolationDegreeMax-interpolationDegreeMin ; ++i )
    {
        std::stringstream s;
        s<<"Correction error for polynomial of degree "<<i+interpolationDegreeMin;
        test = printErrors(s.str(),errorCorrection[i],robustnessOK[i],robustnessTotal[i],tolerance) && test;
    }
    return printFooter(nbIterations, test);
}

