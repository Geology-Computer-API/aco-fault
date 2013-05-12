/*
 * File:   Colony.cpp
 * Author: keoma
 *
 * Created on April 26, 2013, 6:29 PM
 */

#include <stdlib.h>

#include "Colony.h"
#include "Point.h"

#define INITIAL_PHEROMONE_ 0.1f
#define EVAPORATION_RATE_ 0.05f
#define MEMORY_ 100
#define VISIBILITY_THRESHOLD_ 0.0f
#define MINIMUM_PHEROMONE_ 0.01
#define _PHEROMONE_WEIGHT_ 1.0f
#define _VISIBILITY_WEIGHT_ 10.0f

Colony::Colony( Image* image, int nAnts )
{
    _environment = new Environment( INITIAL_PHEROMONE_, MINIMUM_PHEROMONE_, EVAPORATION_RATE_, image );

    addAnts( nAnts );
}



Colony::~Colony()
{
    delete _environment;
    _ants.clear();
}



void Colony::addAnt( Point point )
{
    _ants.push_back( Ant( point, MEMORY_, _PHEROMONE_WEIGHT_, _VISIBILITY_WEIGHT_ ) );
}



void Colony::addAnt()
{
    int x = rand() % _environment->getWidth();
    int y = rand() % _environment->getHeight();

    addAnt( Point(x,y) );
}



void Colony::addAnts( int n )
{
    for (int i = 0; i < n; ++i)
    {
        addAnt();
    }
}



void Colony::run( int nSteps )
{
    for (int step = 0; step < nSteps; ++step)
    {
        for (unsigned int i = 0; i < _ants.size(); ++i)
        {
            Point destination = _ants[i].pick( *_environment );

            if (destination.x < 0)
            {
                do
                {
                    destination.x = rand() % _environment->getWidth();
                    destination.y = rand() % _environment->getHeight();
                }while (!available( destination, _ants[i] ));
            }

            _ants[i].moveTo( destination );

            float visibility = _environment->getVisibility( destination );
            if (visibility >= VISIBILITY_THRESHOLD_)
            {
                _environment->addPheromone( visibility, destination );
                //_environment->addPheromone( 1.0, destination );
            }
            //_environment->addPheromone( 1.0, destination );
        }

        _environment->evaporatePheromone();
    }
}



Image* Colony::getPheromoneImage()
{
    return _environment->getPheromoneImage();
}



bool Colony::available( Point point, Ant& ant )
{
    for (unsigned int i = 0; i < _ants.size(); ++i)
    {
        if (_ants[i].getPosition() == point) return false;
        if (ant.visited( point )) return false;
    }

    return true;
}
