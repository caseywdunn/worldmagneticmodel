/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


#include "GeomagnetismHeader.h"

/*---------------------------------------------------------------------------*/

int main(int argc, char *argv[]){
    // Populated by user input
    MAGtype_GeoMagneticElements GeoMagneticElements;
    MAGtype_Date UserDate;

    // Populated by field calculation
    MAGtype_CoordGeodetic CoordGeodetic;
    MAGtype_MagneticModel *TimedMagneticModel;

    // Parse latitude, longitude, altitude, and year from the arguments
    double latitude = atof(argv[1]);
    double longitude = atof(argv[2]);
    double altitude = atof(argv[3]);
    double year = atof(argv[4]);

    // Create input structures
    ingestPoint(latitude, longitude, altitude, year, &GeoMagneticElements, &UserDate);

    // Calculate the field at the point and time
    calculateMagneticField(&CoordGeodetic, &UserDate, TimedMagneticModel, &GeoMagneticElements);

    // Print the results
    MAG_PrintUserDataSimplified(GeoMagneticElements, CoordGeodetic, UserDate, TimedMagneticModel);

}