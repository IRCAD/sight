/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opSofa/SofaBusiness.hpp"
#include "opSofa/SofaThread.hpp"
#include "opSofa/sofa/OglModelF4S.hpp"

#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/IService.hpp>

#include <sofa/component/contextobject/CoordinateSystem.h>
#include <sofa/component/contextobject/Gravity.h>
#include <sofa/component/init.h>
#include <sofa/component/odesolver/EulerSolver.h>
#include <sofa/component/typedef/Sofa_typedef.h>
#include <sofa/core/objectmodel/BaseContext.h>
#include <sofa/core/objectmodel/Context.h>
#include <sofa/gui/SofaGUI.h>
#include <sofa/helper/ArgumentParser.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/glut.h>
#include <sofa/simulation/tree/GNode.h>
#include <sofa/simulation/tree/TreeSimulation.h>


SofaBusiness::SofaBusiness()
{
}



SofaBusiness::~SofaBusiness()
{
    thread->stop();
    this->clearTranslationPointer();
    delete thread;
    delete meshs;
    delete springs;
}


void SofaBusiness::loadScn(std::string fileScn, ::fwMedData::ModelSeries::sptr ms,
                           ::fwServices::IService::sptr service)
{
    // init attributs
    this->timeStepAnimation = 100;
    meshs                   = new std::vector<fwData::Mesh::sptr>();
    springs                 = new std::map<std::string, StiffSpringForceField3*>();

    // initialize Sofa
    sofa::component::init();

    // load file scn in the scene
    sofa::simulation::Node::SPtr node = sofa::simulation::tree::getSimulation()->load(fileScn.c_str());
    groot = sofa::core::objectmodel::SPtr_dynamic_cast<GNode>(node);

    // Initialize the scene
    sofa::simulation::tree::getSimulation()->init(groot.get());

    // Fill Mesh vector
    std::vector<fwData::Mesh::sptr> meshsF4s;
    this->fillMeshVector(ms, &meshsF4s);

    // Fill OglModel vector
    std::vector<OglModel*> visuals;
    this->fillOglModelVector(groot.get(), &visuals);

    // Fill StiffSpringForceField3 map
    this->fillSpringForceField(groot.get(), springs);

    // Add correspond between mesh sofa and fw4spl
    for (int i = 0; i<visuals.size(); ++i)
    {
        std::string name = visuals[i]->getName();
        for (int j = 0; j<meshsF4s.size(); ++j)
        {
            std::string name2 = meshsF4s[j]->getName();
            if (name == name2)
            {
                // Add mesh to vector refresh by vtk
                meshs->push_back(meshsF4s[j]);

                // Fill OglModel with Mesh
                OglModelF4S* visual = (OglModelF4S*) visuals[i];
                visual->loadMesh(meshsF4s[j]);

                // Translate pointer between sofa and fw4spl
                translationPointer(visuals[i], meshsF4s[j]);
            }
        }
    }

    // Initialize the new scene
    sofa::simulation::tree::getSimulation()->init(groot.get());

    // Create Thread
    thread = new SofaThread(this, meshs, service);
}



void SofaBusiness::loadMesh(::fwData::Mesh::sptr pMesh,  ::fwServices::IService::sptr service)
{
    // Default value : 100 millisecond
    timeStepAnimation = 100;

    // Creation du noeud principal (correspond a la scene)
    groot = sofa::core::objectmodel::New<GNode>();
    groot->setName( "root" );
    groot->setGravity( GNode::Vec3(0,-10,0) );    // on definit la gravite

    // Creation d'un solveur (permet de calculer les nouvelles positions des particules)
    ::sofa::component::odesolver::EulerSolver::SPtr solver;
    solver = ::sofa::core::objectmodel::New< ::sofa::component::odesolver::EulerSolver >();
    groot->addObject(solver);

    // On definit les degres de liberte du Tetrahedre (coordonnees, vitesses...)
    MechanicalObject3::SPtr DOF = sofa::core::objectmodel::New<MechanicalObject3>();
    groot->addObject(DOF);
    DOF->resize(4);
    DOF->setName("DOF");
    sofa::core::objectmodel::Data< MechanicalObject3::VecCoord >* data = DOF->write(sofa::core::VecCoordId::position());
    // On definit les coordonnees
    MechanicalObject3::VecCoord x = data->getValue();
    x[0] = Coord3(0,10,0);
    x[1] = Coord3(10,0,0);
    x[2] = Coord3(-10*0.5,0,10*0.866);
    x[3] = Coord3(-10*0.5,0,-10*0.866);
    data->setValue(x);

    // On definit la masse du Tetrahedre
    UniformMass3::SPtr mass = sofa::core::objectmodel::New<UniformMass3>();
    groot->addObject(mass);
    mass->setMass(2);
    mass->setName("mass");

    // On definit le maillage du Tetrahedre (peut etre compose de lignes, triangles...)
    ::sofa::component::topology::MeshTopology::SPtr topology;
    topology = ::sofa::core::objectmodel::New< ::sofa::component::topology::MeshTopology >();
    topology->setName("mesh topology");
    groot->addObject( topology );
    topology->addTetra(0,1,2,3);

    // On definit les contraintes du Tetrahedre
    FixedConstraint3::SPtr constraints = sofa::core::objectmodel::New<FixedConstraint3>();
    constraints->setName("constraints");
    groot->addObject(constraints);
    constraints->addConstraint(0);

    // On definit les forces du Tetrahedre
    TetrahedronFEMForceField3::SPtr fem = sofa::core::objectmodel::New<TetrahedronFEMForceField3>();
    fem->setName("FEM");
    groot->addObject(fem);
    fem->setMethod("polar");
    fem->setUpdateStiffnessMatrix(true);
    fem->setYoungModulus(6);

    // Creation d'un noeud enfant (a la scene) pour accueillir le visuel du fichier .trian
    GNode::SPtr skin = sofa::core::objectmodel::New<GNode>("skin",groot.get());

    // Creation de la partie visuel du fichier .trian
    OglModelF4S* visual = new OglModelF4S();
    visual->setName( "visual" );
    visual->loadMesh(pMesh);
    visual->setColor("red");
    visual->applyScale(1,1,1);
    skin->addObject(visual);

    // Creation du mapping entre les deux objets (effectue une liaison entre deux objets
    // pour que le rendu suive le mouvement de la partie simulation)
    BarycentricMapping3_to_Ext3::SPtr mapping;
    mapping = sofa::core::objectmodel::New<BarycentricMapping3_to_Ext3>(DOF.get(), visual);
    mapping->setName( "mapping" );
    skin->addObject(mapping);

    // Initialisation de la scene
    sofa::simulation::tree::getSimulation()->init(groot.get());

    // Create Thread
    meshs = new std::vector<fwData::Mesh::sptr>();
    meshs->push_back(pMesh);
    thread = new SofaThread(this, meshs, service);

    // Translate pointer between sofa and fw4spl
    this->translationPointer(visual, pMesh);
}



void SofaBusiness::animate()
{
    sofa::simulation::tree::getSimulation()->animate(groot.get());
}



void SofaBusiness::startThread()
{
    thread->start();
}



void SofaBusiness::stopThread()
{
    thread->stop();
}


bool SofaBusiness::isAnimate()
{
    return thread->isRunning();
}



void SofaBusiness::reset()
{
    sofa::simulation::tree::getSimulation()->reset(groot.get());
}



void SofaBusiness::setTimeStepAnimation(unsigned int timeStepAnimation)
{
    groot->setDt((float)timeStepAnimation/(float)1000); // Animation step define
    this->timeStepAnimation = timeStepAnimation;
}



unsigned int SofaBusiness::getTimeStepAnimation()
{
    return timeStepAnimation;
}



void SofaBusiness::shakeMesh(std::string idMesh, int value)
{
    if (springs->count(idMesh))
    {
        StiffSpringForceField3* spring = (*springs)[idMesh];

        spring->clear();
        spring->addSpring(1, 153, value, 5, 0);
    }
}

void SofaBusiness::moveMesh(std::string idMesh, int x, int y, int z, float rx, float ry, float rz)
{
    GNode::SPtr souris                  = groot;
    MechanicalObjectRigid3f* mechanical =
        (MechanicalObjectRigid3f*) (souris->getObject(sofa::core::objectmodel::TClassInfo<MechanicalObjectRigid3f>::get(),
                                                      idMesh));
    sofa::core::objectmodel::Data< MechanicalObjectRigid3f::VecCoord >* data;
    data = mechanical->write(sofa::core::VecCoordId::position());
    MechanicalObjectRigid3f::VecCoord coord = data->getValue();
    coord[0][0] = x;
    coord[0][1] = y;
    coord[0][2] = z;
    data->setValue(coord);
}



void SofaBusiness::fillOglModelVector(GNode* node, std::vector<OglModel*>* model)
{
    sofa::helper::vector<sofa::core::objectmodel::BaseNode*> gchild = node->getChildren();
    for (unsigned int i = 0; i<gchild.size(); i++)
    {
        GNode* children;
        GNode::dynamicCast(children, node->getChild(gchild[i]->getName()));
        OglModel* visu = (OglModel*) (children->getObject(sofa::core::objectmodel::TClassInfo<OglModel>::get(), ""));
        if (visu != NULL)
        {
            model->push_back(visu);
        }
        this->fillOglModelVector(children, model);
    }
}



void SofaBusiness::fillSpringForceField(GNode* node, std::map<std::string, StiffSpringForceField3*>* springs)
{
    sofa::helper::vector<sofa::core::objectmodel::BaseNode*> gchild = node->getChildren();
    for (unsigned int i = 0; i<gchild.size(); i++)
    {
        GNode* children;
        GNode::dynamicCast(children, node->getChild(gchild[i]->getName()));
        StiffSpringForceField3* spring =
            (StiffSpringForceField3*) (children->getObject(sofa::core::objectmodel::TClassInfo<StiffSpringForceField3>::
                                                           get(),
                                                           ""));
        if (spring != NULL)
        {
            std::string name = spring->getName();
            (*springs)[name] = spring;
        }
        this->fillSpringForceField(children, springs);
    }
}



void SofaBusiness::fillMeshVector(::fwMedData::ModelSeries::sptr ms, std::vector<fwData::Mesh::sptr>* meshs)
{
    for(::fwData::Reconstruction::sptr rec :  ms->getReconstructionDB())
    {
        // Info
        std::string organName = rec->getOrganName();

        // Get Mesh
        ::fwData::Mesh::sptr mesh = rec->getMesh();
        mesh->setName(organName);
        meshs->push_back(mesh);
    }
}



void SofaBusiness::translationPointer(OglModel* visual, ::fwData::Mesh::sptr pMesh)
{
    // Change pointer vertices
    float* verticesSofa = (float*) visual->getVertices().getData()->data();
    ::fwData::Array::sptr pointArray = pMesh->getPointsArray();
    ::fwDataTools::helper::Array arrayHelper(pointArray);
    arrayHelper.setBuffer(
        verticesSofa,
        false,
        pointArray->getType(),
        pointArray->getSize(),
        pointArray->getNumberOfComponents()
        );
}



void SofaBusiness::clearTranslationPointer()
{
    // Reset organs position
    sofa::simulation::tree::getSimulation()->reset(groot.get());
    thread->refreshVtk();

    // Travel each Mesh
    for (int i = 0; i<meshs->size(); ++i)
    {
        ::fwData::Mesh::sptr pMesh = meshs->at(i);

        ::fwData::Array::sptr pointArray;
        pointArray = ::fwData::Object::copy(pMesh->getPointsArray());
        pMesh->setPointsArray(pointArray);
    }
}
