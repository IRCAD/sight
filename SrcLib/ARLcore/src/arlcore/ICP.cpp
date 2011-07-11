/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/ICP.h>

#include <vnl/vnl_trace.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_transpose.h>

#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/algo/vnl_powell.h>

#ifdef ANN
#include <ANN/ANN.h>
#endif // ANN

#include <arlcore/Optimization.h>
#include <arlcore/vnl_rigid_vector.h>

arlCore::ICP::ICP( const arlCore::PointList &model, const arlCore::PointList &cloud, bool justVisible ):
m_point2PointMode(true),
m_modelMesh(0),
m_cloud(0),
m_justVisible(justVisible),
m_maxIterations(50),
m_nbIterations(0),
m_startError(-1),
m_endError(-1),
m_initialization(false),
m_ANNtree(0),
m_modelPoints(0),
m_cloudPoints(0),
m_Pk(0),
m_Yk(0),
m_Pi(0),
m_nn_idx(0),
m_squaredDists(0),
m_modelSize(0),
m_cloudSize(0)
{
    m_solution.setIdentity();
#ifdef ANN
    const unsigned int m_dimension = model.getDimension();
    assert(m_dimension==cloud.getDimension());
    m_modelGravity.set_size(m_dimension);
    m_cloudGravity.set_size(m_dimension);
    unsigned int i, j, n;
    if(justVisible) m_modelSize = model.visibleSize();
    else m_modelSize = model.size();
    if(m_modelSize<1) return;
    m_modelPoints = annAllocPts( m_modelSize, m_dimension );
    for( i=0, n=0 ; i<m_modelSize ; ++i )
        if(model[i])
            if(!justVisible || (justVisible && model[i]->isVisible()))
            {
                for( j=0 ; j<m_dimension ; ++j )
                    m_modelPoints[n][j]=model[i]->get(j);
                ++n;
            }
    m_modelSize = n;
    if(justVisible) m_cloudSize = cloud.visibleSize();
    else m_cloudSize = cloud.size();
    if(m_modelSize<1 || m_cloudSize<1)
    {
        annDeallocPts( m_modelPoints );
        m_modelPoints = 0;
        return;
    }
    m_cloudPoints = annAllocPts( m_cloudSize, m_dimension );
    for( i=0, n=0 ; i<m_cloudSize ; ++i )
        if(cloud[i])
            if(!justVisible || (justVisible && cloud[i]->isVisible()))
            {
                for( j=0 ; j<m_dimension ; ++j )
                    m_cloudPoints[n][j]=cloud[i]->get(j);
                ++n;
            }
    m_cloudSize = n;
    if(m_cloudSize<1)
    {
        annDeallocPts( m_modelPoints );
        annDeallocPts( m_cloudPoints );
        m_modelPoints = 0;
        m_cloudPoints = 0;
        return;
    }
    m_Pk = annAllocPts( m_cloudSize, m_dimension );
    m_Yk = annAllocPts( m_cloudSize, m_dimension );
    m_Pi = annAllocPts( m_cloudSize, m_dimension );
    const int BucketSize = 1;
    m_ANNtree = new ANNkd_tree( m_modelPoints, m_modelSize, m_dimension, BucketSize, ANN_KD_SL_MIDPT );
    m_nbNN = 1;
    m_nn_idx = new ANNidx[m_nbNN];
    m_squaredDists = new ANNdist[m_nbNN];
#endif // ANN
}

arlCore::ICP::ICP( const arlCore::Mesh &model, const arlCore::PointList &cloud, bool justVisible ):
m_point2PointMode(false),
m_modelMesh(&model),
m_cloud(&cloud),
m_justVisible(justVisible),
m_maxIterations(50),
m_nbIterations(0),
m_startError(-1),
m_endError(-1),
m_initialization(false),
m_ANNtree(0),
m_modelPoints(0),
m_cloudPoints(0),
m_Pk(0),
m_Yk(0),
m_Pi(0),
m_nn_idx(0),
m_squaredDists(0)
{
    m_solution.setIdentity();
    if(justVisible) m_modelSize = model.getPointList().visibleSize();
    else m_modelSize = model.getPointList().size();
    if(justVisible) m_cloudSize = cloud.visibleSize();
    else m_cloudSize = cloud.size();
}

arlCore::ICP::ICP( const ICP& T ):
m_maxIterations(T.m_maxIterations),
m_solution(T.m_solution)
{
    // TODO
}

void arlCore::ICP::initSolution( const arlCore::vnl_rigid_matrix &M )
{
    m_solution = M;
    initialization();
}

const arlCore::vnl_rigid_matrix& arlCore::ICP::getSolution( void ) const
{
    return m_solution;
}

arlCore::ICP::~ICP( void )
{
#ifdef ANN
    if(m_ANNtree) delete m_ANNtree;
    if(m_modelPoints) annDeallocPts( m_modelPoints );
    if(m_cloudPoints) annDeallocPts( m_cloudPoints );
    if(m_Pk) annDeallocPts( m_Pk );
    if(m_Yk) annDeallocPts( m_Yk );
    if(m_Pi) annDeallocPts( m_Pi );
    if(m_nn_idx) delete[] m_nn_idx;
    if(m_squaredDists) delete[] m_squaredDists;
    annClose(); 
#endif // ANN
}

bool arlCore::ICP::setMaxIterations( unsigned int n )
{
    if(n<1) return false;
    m_maxIterations = n;
    return true;
}

unsigned int arlCore::ICP::getModelSize( void ) const
{
    return m_modelSize;
}

unsigned int arlCore::ICP::getCloudSize( void ) const
{
    return m_cloudSize;
}

unsigned int arlCore::ICP::getNbIterations( void ) const
{
    return m_nbIterations;
}

double arlCore::ICP::getStartError( void ) const
{
    return m_startError;
}

double arlCore::ICP::getEndError( void ) const
{
    return m_endError;
}

double arlCore::ICP::optimisePowell( void )
{
    unsigned int i;
    arlCore::OptimiseICP optimizer( *this );
    //optimizer.setObserver(true); 
    vnl_powell powell(&optimizer);
    const arlCore::vnl_rigid_vector V( m_solution );
    vnl_vector<double> init(6);
    for( i=0 ; i<6 ; ++i )
        init(i) = V[i];
    powell.minimize(init);
    m_solution.copy(arlCore::vnl_rigid_vector(init));
    m_startError = powell.get_start_error();
    m_endError = powell.get_end_error();
    m_nbIterations = powell.get_num_iterations();
    //m_nbIterations = optimizer.getNbIterations();
    m_solution.copy(arlCore::vnl_rigid_vector(init));
    m_solution.setRMS(m_endError);
    return m_endError;
}

double arlCore::ICP::optimiseLS( void )
{
    unsigned int i;
    m_startError = m_endError;
    arlCore::OptimiseICP_LS optimizer( *this, vnl_least_squares_function::use_gradient );
    //optimizer.setObserver(true); 
    vnl_levenberg_marquardt lm(optimizer);
    const arlCore::vnl_rigid_vector V( m_solution );
    vnl_vector<double> init(6);
    for( i=0 ; i<6 ; ++i )
        init(i) = V[i];
    lm.minimize_using_gradient(init);
    //m_startError = lm.get_start_error();
    //m_endError = lm.get_end_error();
    m_nbIterations = lm.get_num_iterations();
    //m_nbIterations = optimizer.getNbIterations();
    m_solution.copy(arlCore::vnl_rigid_vector(init));
    m_endError = computeCriterion( m_solution );
    m_solution.setRMS(m_endError);
    return m_endError;
}

double arlCore::ICP::computeCriterion( const arlCore::vnl_rigid_matrix &M )
{
    vnl_vector< double > fx;
    return computeCriterion( M, fx );
}

double arlCore::ICP::computeCriterion( const arlCore::vnl_rigid_matrix &M, vnl_vector< double > &fx )
{
    vnl_vector<double> RMS(1, 0.0), nbPoints(1, 0.0);
    const arlCore::vnl_rigid_matrix InvM = M.computeInverse();
    unsigned int i, j, noTriangle;
    double n = 0.0, result = 0.0;
    if(m_point2PointMode)
    {   // Points to points
#ifdef ANN
        const unsigned int Dimension = 3;
        vnl_vector_fixed<double,3> traInit = InvM.getTranslation();
        vnl_matrix_fixed<double,3,3> rotInit = InvM.getRotation();
        const double Epsilon = 0.0;// Error bound
        ANNpoint Pt = annAllocPt(Dimension); // Query point
        for( i=0 ; i<m_cloudSize ; ++i )
        {   // Search the matching point for every point of cloud
            for( j=0 ; j<3; ++j )
                Pt[j] = rotInit[j][0]*m_cloudPoints[i][0]+rotInit[j][1]*m_cloudPoints[i][1]+rotInit[j][2]*m_cloudPoints[i][2]+traInit[j];
            m_ANNtree->annkSearch( Pt, m_nbNN, m_nn_idx, m_squaredDists, Epsilon );
            if(fx.size()>i) fx[i] = m_squaredDists[0];
            RMS[0] += m_squaredDists[0];
        }
        annDeallocPt(Pt);
        n = (double)m_cloudSize;
#endif // ANN
    }else
    {   // Point to mesh
        assert(m_cloud!=0 && m_modelMesh!=0);
        RMS.set_size((unsigned int)m_modelMesh->getTriangles().size());
        RMS.fill(0.0);
        nbPoints.set_size(RMS.size());
        nbPoints.fill(0.0);
        unsigned int i;
        arlCore::Point point(3);
        for( i=0 ; i<m_cloud->size() ; ++i )
            if(m_cloud->get(i))
                if(!m_justVisible || m_cloud->get(i)->isVisible())
                {
                    InvM.trf(*m_cloud->get(i), point);
                    const double SquaredDist = m_modelMesh->computeDistance2(point, noTriangle);
                    if(SquaredDist>0.0)
                    {
                        RMS[noTriangle] += SquaredDist;
                        if(fx.size()>i) fx[i] = SquaredDist;
                        nbPoints[noTriangle] += 1;
                    }
                }
    }
    assert(nbPoints.size()==RMS.size());
    if(RMS.size()==1)
    {
        if(nbPoints[0]>0) return sqrt(RMS[i]/nbPoints[i]);
        else return -1.0;
    }
    for( i=0 ; i<RMS.size() ; ++i )
        if(nbPoints[i]>0)
        {
            result += sqrt(RMS[i]/nbPoints[i]);
            n += 1.0;
        }
    if(n>0) return result/n;
    else return -1;
}

bool arlCore::ICP::initialization( void )
{
#ifdef ANN
    if(m_cloudSize<1 || !m_point2PointMode) return false;
    unsigned int i, j;
    arlCore::vnl_rigid_matrix invS = m_solution.computeInverse();
    m_traInit = invS.getTranslation();
    m_rotInit = invS.getRotation();
    m_cloudGravity.fill(0.0);
    for( i=0 ; i<m_cloudSize ; ++i )
        for( j=0 ; j<3; ++j )
        {
            m_Pi[i][j] = m_Pk[i][j] = m_rotInit[j][0]*m_cloudPoints[i][0]+m_rotInit[j][1]*m_cloudPoints[i][1]+m_rotInit[j][2]*m_cloudPoints[i][2]+m_traInit[j];
            m_cloudGravity[j] += m_Pi[i][j];
        }
    m_cloudGravity /= (double)m_cloudSize;
    m_initialization=true;
    return true;
#endif // ANN
    return false;
}

bool arlCore::ICP::solve( void )
{
    const double Tau = 10e-8;
    const bool Verbose = false;
    m_startError = -1.0;
    m_nbIterations = 0;
#ifdef ANN
    if(!m_point2PointMode) return false;
    m_endError = FLT_MAX/2.0;
    double previousRMS = FLT_MAX;
    if(!m_initialization)
        if(!initialization())
            return false;
//  http://csdl2.computer.org/persagen/DLAbsToc.jsp?resourcePath=/dl/trans/tp/&toc=comp/trans/tp/1992/02/i2toc.xml&DOI=10.1109/34.121791
//  http://ieeexplore.ieee.org/iel1/34/3469/00121791.pdf?tp=&arnumber=121791&isnumber=3469
    unsigned int i, j, k;
    vnl_quaternion<double> qr(0,0,0,1);
    vnl_matrix_fixed<double,3,3> Id3, Ricp, Spy;
    Id3.set_identity();
    vnl_matrix_fixed<double,4,4> QSpy;
    vnl_vector_fixed<double,3> Delta;
    while( fabs(m_endError-previousRMS)>Tau && m_nbIterations<m_maxIterations ) //&& RMS>RMSMax )
    {
        ++m_nbIterations;
        std::cout<<m_nbIterations<<" ";
        previousRMS = m_endError;
        computeError(); // step 1
        if(m_startError<0) m_startError = m_endError;
        // step 2
        // Calculate the cross-variance matrix between m_Pk and Yo
        // Cov( P0, m_Yk ) = 1/n*somme(P0*m_Yk') - ComP0*Comm_Yk'
        Spy.set_identity();
        for( i=0 ; i<m_cloudSize ; ++i )
          for( j=0 ; j<3 ; ++j )
            for( k=0 ; k<3 ; ++k )
            Spy[j][k]+= m_Pi[i][j]*m_Yk[i][k];
        for( i=0 ; i<3 ; ++i )
          for( j=0 ; j<3 ; ++j )
            Spy[i][j] = Spy[i][j]/(double)m_cloudSize - m_cloudGravity[i]*m_modelGravity[j];
        // Delta = [A23 A31 A12] with Amn = Spy[m][n]-Spy[n][m]
        Delta[0] = Spy[1][2]-Spy[2][1];
        Delta[1] = Spy[2][0]-Spy[0][2];
        Delta[2] = Spy[0][1]-Spy[1][0];
        // calculate the symmetric 4x4 matrix needed to determine
        // the max eigenvalue
        QSpy[0][0] = vnl_trace( Spy );
        for( i=1 ; i<4 ; ++i )
        {
            QSpy[i][0] = Delta[i-1];
            for( j = 1; j < 4; ++j )
            {
                QSpy[0][j] = Delta[j-1];
                QSpy[i][j] = Spy[i-1][j-1]+Spy[j-1][i-1] - QSpy[0][0]*Id3[i-1][j-1];
            }
        }
        vnl_symmetric_eigensystem<double> eigQSpy(QSpy);
        // Optimal rotation matrix calculate from the quaternion
        // vector qr=[q0 q1 q2 q3] obtained by obtained the max eigen value
        // http://paine.wiau.man.ac.uk/pub/doc_vxl/core/vnl/html/vnl__symmetric__eigensystem_8h.html
        qr.update(eigQSpy.get_eigenvector(3));
        qr = vnl_quaternion<double>(qr[1], qr[2], qr[3], qr[0]);
        Ricp = vnl_transpose(qr.rotation_matrix_transpose()).asMatrix();
        // Optimal translation vector Ticp T = ComY - Ricp.ComP
        vnl_vector<double> TraTemp = m_modelGravity - Ricp*m_cloudGravity;
        // step 3 : Application of the transformation
        for( i=0 ; i<m_cloudSize ; ++i )
          for( j=0 ; j<3 ; ++j )
            m_Pk[i][j] = Ricp[j][0]*m_Pi[i][0] + Ricp[j][1]*m_Pi[i][1] + Ricp[j][2]*m_Pi[i][2] + TraTemp[j];
    }
    // Give the transformation from model ==> cloud
    // First we add the first transformation to Rend
    vnl_matrix_fixed<double,3,3> Rend = Ricp.transpose();
    vnl_vector_fixed<double,3> Tend;
    for( i=0 ; i<3 ; ++i )
        Tend[i] = m_cloudGravity[i]-(Rend[i][0]*m_modelGravity[0]+Rend[i][1]*m_modelGravity[1]+Rend[i][2]*m_modelGravity[2]);
    // Initial matrix [m_rotInit ; m_traInit ; 0 0 0 1]
    // Found matrix [Rend ; Tend ; 0 0 0 1]
    m_solution.vnl_matrix_fixed<double,4,4>::update(vnl_inverse(m_rotInit)*Rend);
    m_solution.set_column(3,vnl_inverse(m_rotInit)*(Tend-m_traInit));
    m_solution[3][3] = 1.0;
    m_solution.setRMS( m_endError );
    if(Verbose)
    {
        std::cout<<"ICP registration\n";
        std::cout<<"First RMS="<<m_startError<<"\nLast RMS="<<m_endError<<"\nIterations="<<m_nbIterations<<"\nPtsModel="<<m_modelSize<<"\nPtsCloud="<<m_cloudSize<<"\n";
        std::cout<<"Tau ="<<fabs(m_endError-previousRMS)<<"\n";
        std::cout<<"Matrix ="<<m_solution<<"\n";        
    }   
    return true;
#else // ANN
    m_endError = -1.0;
    return false;
#endif // ANN
}

double arlCore::ICP::computeError( void )
{
#ifndef ANN
    m_endError = -1.0;
#else // ANN
    if(m_point2PointMode)
    {   // Points to points
        unsigned int i, j;
        m_endError = 0.0;
        m_modelGravity.fill(0.0);
        const double Epsilon = 0.0;// Error bound
        for( i=0 ; i<m_cloudSize ; ++i )
        {   // search the matching point for every point of cloud
            m_ANNtree->annkSearch(
                m_Pk[i],  // Query point
                m_nbNN,   // Number of near neighbors
                m_nn_idx, // Nearest neighbors (returned)
                m_squaredDists,  // Distance for each neighbors (returned)
                Epsilon );
            for( j=0 ; j<3 ; ++j )
                m_modelGravity[j] += m_Yk[i][j] = m_modelPoints[m_nn_idx[0]][j];
            m_endError += m_squaredDists[0];
        }
        m_endError = sqrt(m_endError/(double)m_cloudSize);
        m_modelGravity /= (double)m_cloudSize;
    }
#endif // ANN
    if(!m_point2PointMode)
    {   // Point to mesh
        assert(m_cloud!=0 && m_modelMesh!=0);
        unsigned int i;
        double n = 0.0;
        m_endError = 0.0;
        const arlCore::vnl_rigid_matrix InvM = m_solution.computeInverse();
        arlCore::Point point(3);
        assert(m_cloud!=0 && m_modelMesh!=0);
        for( i=0 ; i<m_cloud->size() ; ++i )
            if(m_cloud->get(i))
                if(!m_justVisible || m_cloud->get(i)->isVisible())
                {
                    InvM.trf(*m_cloud->get(i), point);
                    m_endError += m_modelMesh->computeDistance2(point);
                    ++n;
                }
        if(n!=0) m_endError = sqrt(m_endError/n);
        else m_endError = -1.0;
    }
    return m_endError;
}

unsigned int arlCore::ICP::computeDistances( std::vector<double> &distances )
{
    distances.clear();
    unsigned int i;
    if(m_point2PointMode)
    {   // Point to point
#ifdef ANN
        const double Epsilon = 0.0;
        for( i=0 ; i<m_cloudSize ; ++i )
        {
            m_ANNtree->annkSearch( m_Pk[i], m_nbNN, m_nn_idx, m_squaredDists, Epsilon );
            const double SquaredDistance = m_squaredDists[0];
            const double Distance = sqrt(SquaredDistance);
            distances.push_back(Distance);
        }
#endif // ANN
    }else
    {   // Point to mesh
        assert(m_cloud!=0 && m_modelMesh!=0);
        const arlCore::vnl_rigid_matrix InvM = m_solution.computeInverse();
        arlCore::Point point(3);
        for( i=0 ; i<m_cloud->size() ; ++i )
            if(m_cloud->get(i))
                if(!m_justVisible || m_cloud->get(i)->isVisible())
                {
                    InvM.trf(*m_cloud->get(i), point);
                    distances.push_back(m_modelMesh->computeDistance(point));
                }
    }
    return (unsigned int)distances.size();
}

unsigned int arlCore::ICP::computeDistances( arlCore::PointList &points )
{
    unsigned int i;
    std::vector<double> distances;
    points.clear();
    arlCore::Point point(3);
    if(m_point2PointMode)
    {
        computeDistances( distances );
#ifdef ANN
        for( i=0 ; i<distances.size() ; ++i )
        {
            point.x(m_Pk[i][0]), point.y(m_Pk[i][1]), point.z(m_Pk[i][2]);
            point.setError(distances[i]);
            points.push_back(point);
        }
#endif // ANN
    }else
    {   // Point to mesh
        const arlCore::vnl_rigid_matrix InvM = m_solution.computeInverse();
        assert(m_cloud!=0 && m_modelMesh!=0);
        for( i=0 ; i<m_cloud->size() ; ++i )
            if(m_cloud->get(i))
                if(!m_justVisible || m_cloud->get(i)->isVisible())
                {
                    InvM.trf(*m_cloud->get(i), point);
                    point.setError(m_modelMesh->computeDistance(point));
                    points.push_back(point);
                }
    }
    return (unsigned int)points.size();
}
