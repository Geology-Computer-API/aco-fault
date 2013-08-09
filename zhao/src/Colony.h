/*
 * File:   Colony.h
 * Author: keoma
 *
 * Created on July 29, 2013, 6:01 PM
 */

#ifndef COLONY_H
#define	COLONY_H

#include <vector>

#include "image.h"

class Point;
class Ant;
class Environment;

class Colony
{
public:
    Colony( Image* image );
    virtual ~Colony();
    void distributeAnts();
    void addAntInBlock( Point pMin, Point pMax );
    void run( int nSteps );
    void moveAnts();
    void updatePheromone();
    Image* getPheromoneImage();
    bool available( Point point, Ant& ant );

private:
    Environment* _environment;
    std::vector<Ant*> _ants;
};

#endif	/* COLONY_H */

