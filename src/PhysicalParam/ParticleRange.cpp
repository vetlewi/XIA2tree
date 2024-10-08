/*
 *
 *  Created on: 17.03.2010
 *      Author: Alexander Bürger
 */

#include "ParticleRange.h"

#include <fstream>
#include <iostream>
#include <sstream>

// ########################################################################

ParticleRange::ParticleRange()
    : Emin( 200 )
    , Estep( 10 )
{
}

// ########################################################################

ParticleRange::ParticleRange(const std::string &filename)
    : Emin( 200 )
    , Estep( 10 )
{
    if ( !filename.empty() )
        Read(filename);
}

// ########################################################################

ParticleRange::ParticleRange(const double *E, const double *val, const size_t &size)
    : Emin( 200 )
    , Estep( 10 )
{
    if ( size > 0 && E && val )
        Read(E, val, size);
}

// ########################################################################

//! Read one line from a zrange file.
/*! Jumps over comments (# at start) and empty lines. At the end of
 *  the file, energy, range, and uncertainty will be 0.
 */
static void readline(std::istream& in, /*!< The input stream. */
                     double& e,         /*!< The energy read from the stream, converted to keV. */
                     double& r,         /*!< The range at this energy, in um. */
                     double& dr         /*!< The range uncertainty, in um. */)
{
    std::string line;
    while( getline(in, line) ) {
        if( line.empty() )
            continue;
        if( line[0] == '#' )
            continue;

        std::istringstream l( line.c_str() );
        l >> e >> r >> dr; // read energy [MeV], range [um], range uncertainty [um]
        e *= 1000;         // convert to energy keV
        if( !in )          // check for errors
            break;
        return;
    }
    // end of file, or error: everything 0
    e = r = dr = 0;
}

// ########################################################################

void ParticleRange::Read(const std::string & filename)
{
    // forget old interpolation
    values.clear();

    // open zrange file
    std::ifstream f( filename.c_str() );
    double flE, flR, flDR; // last energy, range, drange from file
    readline( f, flE, flR, flDR );
    double fE, fR, fDR;
    readline( f, fE, fR, fDR );

    const double hstep = Estep/2.0;

    for(double E = Emin; !(!f); E += Estep ) {
        while( f && fE<E+hstep ) {
            flE  = fE;
            flR  = fR;
            flDR = fDR;
            readline( f, fE, fR, fDR );
        }
        if( !f || fE == 0 )
            break;
        const double x = (E+hstep-flE)/(fE-flE);
        const double eR = fR*x + flR*(1-x);
        values.push_back( eR );
    }
    //std::cout << "ParticleRange: " << values.size() << " interpolation points" << std::endl;
}

// ########################################################################

void ParticleRange::Read(const double *Eval, const double *val, const size_t &size)
{
    // forget old interpolation
    values.clear();

    double flE=0, flR=0; // last energy, range, drange from file
    double fE=0, fR=0;

    flE = Eval[0];
    flR = val[0];
    fE = Eval[1];
    fR = val[1];

    size_t i = 2;
    const double hstep = Estep/2.0;
    for ( double E = Emin ; i < size ; E += Estep ){
        while ( i < size && fE < E+hstep ){
            flE = fE;
            flR = fR;
            fE = Eval[i];
            fR = val[i];
            ++i;
        }
        if (i >= size || fE == 0 )
            break;
        const double x = (E+hstep-flE)/(fE-flE);
        const double eR = fR*x + flR*(1-x);
        values.push_back( eR );
    }
}


// ########################################################################

double ParticleRange::GetRange(double energy) const
{
    if( values.empty() )
        return 0;
    if( energy<Emin )
        return 0;
    unsigned int index = (energy-Emin)/Estep;
    if( index >= values.size() )
        return 1e6;
    return values[index];
}

// ########################################################################
// ########################################################################

#ifdef TEST_PARTICLERANGE

#include <iostream>

int main(int argc, char* argv[])
{
    if( argc != 2 )
        return -1;

    ParticleRange range;
    range.Read( argv[1] );

    const int E[] = { 1000, 2000, 3000, 3200, 10000, 30000, 40000, 75000, 79000, 81000, 85000, -1 };
    for( int i=0; E[i]>0; ++i )
        std::cout << "E=" << E[i] << " range=" << range.GetRange( E[i] ) << std::endl;

    return 0;
}

// g++ -Wall -W -O2 -DTEST_PARTICLERANGE -o /tmp/pr.exe ParticleRange.cpp
#endif