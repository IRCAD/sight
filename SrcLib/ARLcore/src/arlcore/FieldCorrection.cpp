/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <arlcore/FieldCorrection.h>
#include <fstream>
#include <vnl/algo/vnl_powell.h>

#include <arlcore/Misc.h>
#include <arlcore/Optimization.h>

arlCore::FieldCorrector::FieldCorrector( void ):
m_correction( false ),
m_degree( 0 )
{}

arlCore::FieldCorrector::FieldCorrector( unsigned int degree ):
m_correction( false ),
m_degree( degree )
{
    const unsigned int NbEquations = 3;
    m_parameters.set_size(nbPolynomialParameters(degree, NbEquations));
    m_parameters.fill(0.0);
}

arlCore::FieldCorrector::FieldCorrector( const FieldCorrector& O )
{
    copy(O);
}

bool arlCore::FieldCorrector::activCorrection( bool b )
{
    m_correction = b;
    return m_correction;
}

arlCore::FieldCorrector::~FieldCorrector( void )
{}

bool arlCore::FieldCorrector::copy( const FieldCorrector& O )
{
    m_parameters = O.m_parameters;
    m_degree = O.m_degree;
    m_correction = O.m_correction;
    return true;
}

std::string arlCore::FieldCorrector::getString( void ) const
{
    std::stringstream s;
    const unsigned int NbEquations = 3;
    s<<"Polynomial of degree "<<m_degree<<" Nb parameters="<<m_parameters.size()<<"\n";
    s<<m_parameters<<"\n";
    // TODO Display equations
//  for( i=0 ; i<NbEquations ; ++i )
//      s<<C[i]<<" + "<<C[i+3]<<"*Z + "<<C[i+6]<<"*Z*Z + "<<C[i+9]<<"*Y + "<<C[i+12]<<"*Y*Z + "<<C[i+15]<<"*Y*Y + "<<C[i+18]<<"*X + "<<C[i+21]<<"*X*Z + "<<C[i+24]<<"*X*Y + "<<C[i+27]<<"*X*X\n";
    return s.str();
}

bool arlCore::FieldCorrector::setDegree( unsigned int degree )
{
    if(degree<1) return false;
    const unsigned int NbEquations = 3;
    m_degree = degree;
    m_parameters.set_size(nbPolynomialParameters(m_degree, NbEquations));
    m_parameters.fill(0.0);
    return true;
}

unsigned int arlCore::FieldCorrector::getDegree( void )
{
    return m_degree;
}

unsigned int arlCore::FieldCorrector::getNbParameters( void )
{
    return m_parameters.size();
}

bool arlCore::FieldCorrector::setParameters( const vnl_vector<double> &parameters )
{
    if(parameters.size()!=getNbParameters())
    {
        const unsigned int NbEquations = 3;
        unsigned int degree = 1;
        while(nbPolynomialParameters(degree, NbEquations)!=parameters.size() && degree<10)
        {
            ++degree;
        }
        if(nbPolynomialParameters(degree, NbEquations)!=parameters.size()) return false;
        setDegree(degree);
    }
    m_parameters = parameters;
    return true;
}

bool arlCore::FieldCorrector::load( const std::string &fileName )
{
    unsigned int i;
    std::fstream f;
    f.open (fileName.c_str(), std::fstream::in);
    if(!f.is_open()) return false;
    unsigned int nbParameters;
    f>>nbParameters;
    vnl_vector<double> parameters(nbParameters);
    for( i=0 ; i<nbParameters ; ++i )
        f>>parameters[i];
    f.close();
    return setParameters(parameters);
}

bool arlCore::FieldCorrector::correct( const Point &P1, Point &P2 ) const
{   // P1=distorted P2=undistorded
    assert(P1.size()==3 && P2.size()==3);
    const unsigned int NbEquations = 3;
    const bool Verbose = false;
    P2.copy(P1);
    if(!m_correction || P1.size()!=3 || P2.size()!=3) return false;
    vnl_vector<double> coordinates(NbEquations), distorsion;
    coordinates[0] = P1.x();
    coordinates[1] = P1.y();
    coordinates[2] = P1.z();
    bool b = computePolynomial( m_degree, m_parameters, coordinates, distorsion );
    if(b)
    {
        P2.x(coordinates[0]-distorsion[0]);
        P2.y(coordinates[1]-distorsion[1]);
        P2.z(coordinates[2]-distorsion[2]);
        if(Verbose) std::cout<<"Distorsion correction : "<<distorsion<<"\n";
    }
    return b;
}

bool arlCore::FieldCorrector::correct( const vnl_rigid_matrix &T1, vnl_rigid_matrix &T2 ) const
{   // T1=distorded T2=undistorded
    const unsigned int NbEquations = 3;
    const bool Verbose = true;
    T2.copy(T1);
    if(!m_correction) return false;
    vnl_vector<double> coordinates(NbEquations), distorsion;
    coordinates[0] = T1.getX();
    coordinates[1] = T1.getY();
    coordinates[2] = T1.getZ();
    bool b = computePolynomial( m_degree, m_parameters, coordinates, distorsion );
    if(b)
    {
        T2.setTranslation(coordinates[0]-distorsion[0], coordinates[1]-distorsion[1], coordinates[2]-distorsion[2]);
        if(Verbose) std::cout<<"Distorsion correction : "<<distorsion<<"\n";
    }
    return b;
}

bool arlCore::FieldCorrector::correct( vnl_rigid_matrix &T ) const
{
    vnl_rigid_matrix T0;
    if(!correct(T, T0)) return false;
    T.copy(T0);
    return true;
}

bool arlCore::fieldCalibration( const PointList &real, const PointList &distorded, unsigned int degree, vnl_vector<double> &parameters, double &RMS )
{
    if(degree<1) return false;
    Polynomial_cost_function pcf( real, distorded, degree );
    vnl_powell op(&pcf); 
    parameters.set_size(pcf.getNbParameters());
    parameters.fill(0.0);
    op.minimize(parameters);
    RMS = op.get_end_error();
    return true;
}
