/*
 * File:   Ant.h
 * Author: keoma
 *
 * Created on July 29, 2013, 6:04 PM
 */

#ifndef ANT_H
#define	ANT_H

#include <vector>

#include "Point.h"

class Environment;

class Ant
{
public:
    Ant( Point point, Environment* environment );
    virtual ~Ant();
    void setStepLength( int stepLength );
    void setVisibilityWeight( float visibilityWeight );
    void setPheromoneWeight( float pheromoneWeight );
    void move();
    bool isAlive();
    void depositPheromone();
    static int pickIndex( std::vector<float> probabilities );

private:
    void getVisiblePixels( std::vector<Point>& visiblePixels );
    Point choosePixel( std::vector<Point>& visiblePixels );
    void computeProbabilities( std::vector<Point>& pixels, std::vector<float>& probabilities );
    bool isInsideFOV( Point p );

    bool _isAlive;
    int _stepLength;
    float _pheromoneWeight;
    float _visibilityWeight;
    Point _position;
    float _pheromoneConstant;
    float _fieldOfView;
    Environment* _environment;
    std::vector<Point> _path;
};

#endif	/* ANT_H */

