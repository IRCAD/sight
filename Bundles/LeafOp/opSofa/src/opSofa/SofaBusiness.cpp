#include <boost/foreach.hpp>

#include "opSofa/SofaBusiness.hpp"
#include "opSofa/SofaThread.hpp"
#include "opSofa/sofa/OglModelF4S.hpp"

#include <sofa/component/init.h>
#include <sofa/simulation/tree/xml/initXml.h>
#include <sofa/simulation/tree/TreeSimulation.h>
#include <sofa/helper/ArgumentParser.h>
#include <sofa/component/contextobject/Gravity.h>
#include <sofa/component/contextobject/CoordinateSystem.h>
#include <sofa/core/objectmodel/Context.h>
#include <sofa/component/odesolver/CGImplicitSolver.h>
#include <sofa/helper/system/FileRepository.h>
#include <sofa/gui/SofaGUI.h>
#include <sofa/component/typedef/Sofa_typedef.h>
#include <sofa/helper/system/glut.h>

using namespace sofa::simulation::tree;
using sofa::simulation::Node;
using sofa::component::odesolver::CGImplicitSolver;
using sofa::component::topology::MeshTopology;


/**
 * @brief Constructor
 *
 */
SofaBusiness::SofaBusiness()
{
}


/**
 * @brief Destructor
 */
SofaBusiness::~SofaBusiness()
{
    thread->stop();
    clearTranslationPointer();
    delete thread;
    delete meshs;
    delete springs;
    delete groot;
}


/**
 * @brief Builds the SOFA physical model and data and instanciates the SofaThread class
 *
 * @param fileScn : path of the file .scn
 * @param acquisition : object acquisition of FW4SPL
 * @param service : pointer to the SofaService object
 */
void SofaBusiness::loadScn(std::string fileScn, ::fwData::Acquisition::sptr acquisition,  ::fwServices::IService::sptr service)
{
    // init attributs
    this->timeStepAnimation = 100;
    meshs = new std::vector<fwData::Mesh::sptr>();
    springs = new std::map<std::string, StiffSpringForceField3*>();

    // initialize Sofa
    sofa::component::init();
    sofa::simulation::tree::xml::initXml();

    // load file scn in the scene
    groot = dynamic_cast<sofa::simulation::tree::GNode*>( sofa::simulation::tree::getSimulation()->load(fileScn.c_str()));

    // Initialize the scene
    getSimulation()->init(groot);

    // Fill Mesh vector
    std::vector<fwData::Mesh::sptr> meshsF4s;
    fillMeshVector(acquisition, &meshsF4s);

    // Fill OglModel vector
    std::vector<OglModel*> visuals;
    fillOglModelVector(groot, &visuals);

    // Fill StiffSpringForceField3 map
    fillSpringForceField(groot, springs);

    // Add correspond between mesh sofa anf fw4spl
    for (int i=0; i<visuals.size(); ++i) {
        std::string name = visuals[i]->getName();
        for (int j=0; j<meshsF4s.size(); ++j) {
            std::string name2 = meshsF4s[j]->getName();
            if (name == name2) {
                // Add mesh to vector refresh by vtk
                meshs->push_back(meshsF4s[j]);

                // Fill OglModel with Mesh
                OglModelF4S *visual = (OglModelF4S*) visuals[i];
                visual->loadMesh(meshsF4s[j]);

                // Translate pointer between sofa and fw4spl
                translationPointer(visuals[i], meshsF4s[j]);
            }
        }
    }

    // Initialize the new scene
    getSimulation()->init(groot);

    // Create Thread
    thread = new SofaThread(this, meshs, service);
}


/**
 * @brief Builds the SOFA physical model and data and instanciates the SofaThread class
 *
 * @param pMesh : pointer to the triangular mesh
 * @param service : pointer to the SofaService object
 */
void SofaBusiness::loadMesh(::fwData::Mesh::sptr pMesh,  ::fwServices::IService::sptr service)
{
    // Default value : 100 millisecond
    timeStepAnimation = 100;

    // Creation du noeud principal (correspond a la scene)
    groot = new GNode;
    groot->setName( "root" );
    groot->setGravityInWorld( Coord3(0,-10,0) );    // on definit la gravite

    // Creation d'un solveur (permet de calculer les nouvelles positions des particules)
    CGImplicitSolver* solver = new CGImplicitSolver;
    groot->addObject(solver);

    // On definit les degres de liberte du Tetrahedre (coordonnees, vitesses...)
    MechanicalObject3* DOF = new MechanicalObject3;
    groot->addObject(DOF);
    DOF->resize(4);
    DOF->setName("DOF");
    VecCoord3& x = *DOF->getX(); // On definit les coordonnees
    x[0] = Coord3(0,10,0);
    x[1] = Coord3(10,0,0);
    x[2] = Coord3(-10*0.5,0,10*0.866);
    x[3] = Coord3(-10*0.5,0,-10*0.866);

    // On definit la masse du Tetrahedre
    UniformMass3* mass = new UniformMass3;
    groot->addObject(mass);
    mass->setMass(2);
    mass->setName("mass");

    // On definit le maillage du Tetrahedre (peut etre compose de lignes, triangles...)
    MeshTopology* topology = new MeshTopology;
    topology->setName("mesh topology");
    groot->addObject( topology );
    topology->addTetra(0,1,2,3);

    // On definit les contraintes du Tetrahedre
    FixedConstraint3* constraints = new FixedConstraint3;
    constraints->setName("constraints");
    groot->addObject(constraints);
    constraints->addConstraint(0);

    // On definit les forces du Tetrahedre
    TetrahedronFEMForceField3* fem = new  TetrahedronFEMForceField3;
    fem->setName("FEM");
    groot->addObject(fem);
    fem->setMethod("polar");
    fem->setUpdateStiffnessMatrix(true);
    fem->setYoungModulus(6);

    // Creation d'un noeud enfant (a la scene) pour accueillir le visuel du fichier .trian
    GNode* skin = new GNode("skin",groot);

    // Creation de la partie visuel du fichier .trian
    OglModelF4S *visual = new OglModelF4S();
    visual->setName( "visual" );
    visual->loadMesh(pMesh);
    visual->setColor("red");
    visual->applyScale(1);
    skin->addObject(visual);

    // Creation du mapping entre les deux objets (effectue une liaison entre deux objets
    // pour que le rendu suive le mouvement de la partie simulation)
    BarycentricMapping3_to_Ext3* mapping = new BarycentricMapping3_to_Ext3(DOF, visual);
    mapping->setName( "mapping" );
    skin->addObject(mapping);

    // Initialisation de la scene
    getSimulation()->init(groot);

    // Create Thread
    meshs = new std::vector<fwData::Mesh::sptr>();
    meshs->push_back(pMesh);
    thread = new SofaThread(this, meshs, service);

    // Translate pointer between sofa and fw4spl
    translationPointer(visual, pMesh);
}


/**
 * @brief Processes the deformation of the triangular mesh for the next step
 */
void SofaBusiness::animate()
{
    getSimulation()->animate(groot);
}


/**
 * @brief Starts the thread dedicated to the deformation processing in SOFA
 */
void SofaBusiness::startThread()
{
    thread->start();
}


/**
 * @brief Stops the thread dedicated to the deformation processing in SOFA
 */
void SofaBusiness::stopThread()
{
    thread->stop();
}

/**
 * @brief Get stage of the animation
 *
 * @return true if animation is running
 */
bool SofaBusiness::isAnimate()
{
    return thread->isRunning();
}


/**
 * @brief Resets the SOFA scene
 *
 * Reload the initial conditions of the mesh and of the physical model in SOFA
 */
void SofaBusiness::reset()
{
    getSimulation()->reset(groot);
}


/**
 * @brief Initializes the time step animation
 *
 * @param timeStepAnimation : time between two calculation in millisecond
 */
void SofaBusiness::setTimeStepAnimation(unsigned int timeStepAnimation)
{
    groot->setDt((float)timeStepAnimation/(float)1000); // Animation step define
    this->timeStepAnimation = timeStepAnimation;
}


/**
 * @brief Gets time step animation
 *
 * @return time between two calculation in millisecond
 */
 unsigned int SofaBusiness::getTimeStepAnimation()
{
    return timeStepAnimation;
}


 /**
 * @brief Shake organ
 *
 * @param idMesh : id organ
 * @param value : value of force
 */
void SofaBusiness::shakeMesh(std::string idMesh, int value)
{
    if (springs->count(idMesh)) {
        StiffSpringForceField3 *spring = (*springs)[idMesh];

        spring->clear();
        spring->addSpring(1, 153, value, 5, 0);
    }
}

void SofaBusiness::moveMesh(std::string idMesh, int x, int y, int z, float rx, float ry, float rz)
{
    GNode *souris = groot;
    MechanicalObjectRigid3f *mechanical = (MechanicalObjectRigid3f*) (souris->getObject(sofa::core::objectmodel::TClassInfo<MechanicalObjectRigid3f>::get(), idMesh));
    std::string name = mechanical->getName();
    VecCoordRigid3f& coord = *mechanical->getX();
    coord[0][0] = x;
    coord[0][1] = y;
    coord[0][2] = z;
}


/**
 * @brief Bring OglModel of Sofa
 *
 * @param node : scene root of Sofa
 * @param model : OglModel vector at fill
 */
void SofaBusiness::fillOglModelVector(GNode *node, std::vector<OglModel*> *model)
{
   sofa::helper::vector<sofa::core::objectmodel::BaseNode*> gchild = node->getChildren();
   for (unsigned int i=0; i<gchild.size(); i++) {
        GNode *children = node->getChild(gchild[i]->getName());
        OglModel *visu = (OglModel*) (children->getObject(sofa::core::objectmodel::TClassInfo<OglModel>::get(), ""));
        if (visu != NULL) {
            model->push_back(visu);
        }
        fillOglModelVector(children, model);
    }
}


/**
 * @brief Bring SpringForceField of Sofa
 *
 * @param node : scene root of Sofa
 * @param model : SpringForceField map at fill
 */
void SofaBusiness::fillSpringForceField(GNode *node, std::map<std::string, StiffSpringForceField3*> *springs)
{
   sofa::helper::vector<sofa::core::objectmodel::BaseNode*> gchild = node->getChildren();
   for (unsigned int i=0; i<gchild.size(); i++) {
        GNode *children = node->getChild(gchild[i]->getName());
        StiffSpringForceField3 *spring = (StiffSpringForceField3*) (children->getObject(sofa::core::objectmodel::TClassInfo<StiffSpringForceField3>::get(), ""));
        if (spring != NULL) {
            std::string name = spring->getName();
            (*springs)[name] = spring;
        }
        fillSpringForceField(children, springs);
    }
}


/**
 * @brief Bring Mesh of Fw4spl
 *
 * @param acquisition : object acquisition of Fw4spl
 * @param meshs : Mesh vector at fill
 */
void SofaBusiness::fillMeshVector(::fwData::Acquisition::sptr acquisition, std::vector<fwData::Mesh::sptr> *meshs)
{
    BOOST_FOREACH(::fwData::Reconstruction::sptr rec, acquisition->getReconstructions())
    {
        // Info
        std::string organName = rec->getOrganName();
        bool isVisible = rec->getIsVisible();
        std::string uid = rec->getID();

        // Get Mesh
        ::fwData::Mesh::sptr mesh = rec->getMesh();
        boost::filesystem::path path = rec->getPath();
        mesh->setName(organName);
        meshs->push_back(mesh);
    }
}


/**
 * @brief Translates pointer between sofa and fw4spl to set a shared memory access
 *
 * @param visual : object visual of sofa
 * @param pMesh : object mesh of fw4spl
 */
void SofaBusiness::translationPointer(OglModel *visual, ::fwData::Mesh::sptr pMesh)
{
    // Change pointer vertices
    float *verticesSofa = (float*) visual->getVertices()->getData()->data();
    ::fwData::Array::sptr pointArray = pMesh->getPointsArray();
    pointArray->setBuffer(
            verticesSofa,
            false,
            pointArray->getType(),
            pointArray->getSize(),
            pointArray->getNumberOfComponents()
            );
}


/**
 * @brief Cancel translates pointer between sofa and fw4spl
 *
 */
 void SofaBusiness::clearTranslationPointer()
 {
     // Reset organs position
     getSimulation()->reset(groot);
     thread->refreshVtk();

     // Travel each Mesh
     for (int i=0; i<meshs->size(); ++i)
     {
        ::fwData::Mesh::sptr pMesh = meshs->at(i);

        ::fwData::Array::NewSptr pointArray;
        pointArray->deepCopy(pMesh->getPointsArray());
        pMesh->setPointsArray(pointArray);
     }
 }
