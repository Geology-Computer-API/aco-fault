/*
 * File:   Colony.cpp
 * Author: keoma
 *
 * Created on July 29, 2013, 6:01 PM
 */

#include "Colony.h"
#include "Point.h"
#include "Environment.h"
#include "Ant.h"
#include "Parameters.h"

#include <stdio.h>
#include <stdlib.h>

Colony::Colony( Image* image )
{
    if (imgGetDimColorSpace( image ) != 1)
    {
        Image* aux = imgGrey( image );
        imgDestroy( image );
        image = aux;
        aux = 0;
    }
    
    // Normalize image
    float mean = imgComputeMean( image );
    float stdDev = sqrt( imgComputeVariance( image, mean ) );
    imgClipPositiveOutliers( image, mean + stdDev + stdDev + stdDev + stdDev + stdDev );
    imgNormalize( image );
    
    // Apply gaussian filter for noise reduction
    imgGauss( image );
    
    imgWriteBMP( (char*)"trueInput.bmp", image );
    
    _environment = new Environment( INITIAL_PHEROMONE, MIN_PHEROMONE, EVAPORATION_RATE, image );
    
    generateProbabilityImages();
}



Colony::~Colony()
{
}



void Colony::distributeAnts()
{
    //distributeAntsByBlock();
    distributeAntsByGamma();
}



void Colony::distributeAntsByBlock()
{
    int nHorizontalBlocks = _environment->getWidth() / BLOCK_SIZE;
    int nVerticalBlocks   = _environment->getHeight() / BLOCK_SIZE;

    for (int hb = 0; hb < nHorizontalBlocks; ++hb)
    {
        for (int vb = 0; vb < nVerticalBlocks; ++vb)
        {
            Point pMin( hb * BLOCK_SIZE, vb * BLOCK_SIZE );
            Point pMax( pMin.x + BLOCK_SIZE - 1, pMin.y + BLOCK_SIZE - 1 );
            addAntInBlock( pMin, pMax );
        }
    }
}



void Colony::distributeAntsByGamma()
{
    int nGammaImages = _probabilityDistributions.size();
    
    for (int a = 0; a < NUMBER_OF_ANTS; ++a)
    {
        addAntInImage( _probabilityDistributions[a % nGammaImages] );
    }
}



void Colony::generateProbabilityImages()
{
     Image* vis = _environment->getVisibilityImage(); imgWriteBMP( (char*)"gammaone.bmp", vis );
     
     Image* visGammaDown = imgCopy( vis );
     imgGamma( visGammaDown, 0.5f ); imgWriteBMP( (char*)"gammadown.bmp", visGammaDown );
     Image* gammaDownProb = generateProbabilityImage( visGammaDown );
     imgDestroy( visGammaDown );
     _probabilityDistributions.push_back( gammaDownProb );
     
     Image* visGammaUp = imgCopy( vis );
     imgGamma( visGammaUp, 1.5f ); imgWriteBMP( (char*)"gammaup.bmp", visGammaUp );
     Image* gammaUpProb = generateProbabilityImage( visGammaUp );
     imgDestroy( visGammaUp );
     _probabilityDistributions.push_back( gammaUpProb );
     
     Image* prob = generateProbabilityImage( vis );
     imgDestroy( vis );
     _probabilityDistributions.push_back( prob );
}



Image* Colony::generateProbabilityImage( Image* input )
{
    if (imgGetDimColorSpace( input ) != 1) return 0;
    
    Image* output = imgCopy( input );
    float* data = imgGetData( output );
    int size = imgGetWidth( output ) * imgGetHeight( output );
    float sum = 0;
    for (int i = 0; i < size; ++i)
    {
        if (isnan(data[i]))
        {
            printf( "Cannot generate probability image, NAN detected! on pixel %d\n", i );
        }
        sum += data[i];
    }
    for (int i = 0; i < size; ++i)
    {
        data[i] /= sum;
    }
    
    return output;
}



void Colony::addAntInBlock( Point pMin, Point pMax )
{
    // compute the probabilities
    std::vector<float> probabilities;

    float sum = 0.0f;
    for (int y = pMin.y; y <= pMax.y; ++y)
    {
        for (int x = pMin.x; x <= pMax.x; ++x)
        {
            sum += _environment->getVisibility( x, y );
        }
    }

    for (int y = pMin.y; y <= pMax.y; ++y)
    {
        for (int x = pMin.x; x <= pMax.x; ++x)
        {
            float pixelValue = _environment->getVisibility( x, y );
            probabilities.push_back( pixelValue / sum );
        }
    }
    
    int blockSize = pMax.x - pMin.x + 1;

    int pixel = Ant::pickIndex( probabilities );
    Point chosenPoint( pMin.x + pixel % blockSize, pMin.y + pixel / blockSize );
    Ant* ant = new Ant( chosenPoint, _environment );
    ant->setStepLength( STEP_LENGTH );
    ant->setPheromoneWeight( PHEROMONE_WEIGHT );
    ant->setVisibilityWeight( VISIBILITY_WEIGHT );
    _ants.push_back( ant );
}



void Colony::addAntInImage( Image* probabilityImage )
{
    float* probabilities = imgGetData( probabilityImage );
    int pixel = Ant::pickIndex( probabilities, _environment->getWidth()*_environment->getHeight() );
    int width = imgGetWidth( probabilityImage );
    Point chosenPoint( pixel % width, pixel / width );
    Ant* ant = new Ant( chosenPoint, _environment );
    ant->setStepLength( STEP_LENGTH );
    ant->setPheromoneWeight( PHEROMONE_WEIGHT );
    ant->setVisibilityWeight( VISIBILITY_WEIGHT );
    _ants.push_back( ant );
}



void Colony::run( int nSteps )
{
    for (int currentStep = 0; currentStep < nSteps; ++currentStep)
    {
        distributeAnts();
        moveAnts();
        updatePheromone();
        _ants.clear();
    }
}



void Colony::moveAnts()
{
    bool allDead = false;
    const int nAnts = _ants.size();
    const int environmentWidth = _environment->getWidth();
    const int environmentHeight = _environment->getHeight();
    
    while (!allDead)
    {
        allDead = true;
        #pragma omp parallel for
        for (int a = 0; a < nAnts; ++a)
        {
            Ant* ant = _ants[a];
            
            if (ant->isAlive())
            {
                allDead = false;
                ant->move();
            }
            
            if (ant->_position.x < 0 || ant->_position.x >= environmentWidth
                    || ant->_position.y < 0 || ant->_position.y >= environmentHeight)
            {
                printf( "INVALID MOVE BY ANT %d! (%d, %d)\n", a, ant->_position.x, ant->_position.y );
            }
        }
    }
    printDebugImage();
}



void Colony::updatePheromone()
{
    int nAnts = _ants.size();

    for (int a = 0; a < nAnts; ++a)
    {
        _ants[a]->depositPheromone();
    }
}



Image* Colony::getPheromoneImage()
{
    return 0;
}



bool Colony::available( Point point, Ant& ant )
{
    return false;
}



void Colony::printDebugImage()
{
    static int step = 0;
    Image* img = _environment->getPheromoneImage();//imgCreate( _environment->getWidth(), _environment->getHeight(), 3 );

    for (int x = 0; x < _environment->getWidth(); ++x)
    {
        for (int y = 0; y < _environment->getHeight(); ++y)
        {
            float lum = imgGetPixelf( img, x, y );
//            if (lum < 0.4f) lum = 0.0f;
//            else lum = 1.0f;
            imgSetPixelf( img, x, y, lum );
        }
    }
    
//    int nAnts = _ants.size();
//    for (int i = 0; i < nAnts; ++i)
//    {
//        Ant* ant = _ants[i];
//        if (ant->isAlive())
//        {
//            // each ant has a color
//            //imgSetPixel3f( img, ant->_position.x, ant->_position.y, 1.0f - i/(float)nAnts, (0.0f + i) / nAnts, 0.0f );
//            
//            //all ants are red
//            imgSetPixel3f( img, ant->_position.x, ant->_position.y, 1,0,0 );
//        }
//        else
//        {
//            imgSetPixel3f( img, ant->_position.x, ant->_position.y, 0.0f, 0.0f, 1.0f );
//        }
//    }
    
//    Image* aux = imgBinOtsu( img );
//    imgDestroy( img );
//    img = aux;
//    aux = 0;

    char filename[100];
    sprintf( filename, "/home/keoma/Dropbox/PUC/Mestrado/antColonyOptimization/zhao/build/debugImages/debugImage%04d.bmp", ++step );
    imgWriteBMP( filename, img );
    imgDestroy( img );
}
