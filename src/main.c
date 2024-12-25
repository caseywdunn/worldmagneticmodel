/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>


#include "GeomagnetismHeader.h"

/*---------------------------------------------------------------------------*/

int main(int argc, char *argv[]){
    // Populated by user input
    
    MAGtype_Date UserDate;
    MAGtype_CoordGeodetic CoordGeodetic;

    // Populated by field calculation
    MAGtype_GeoMagneticElements GeoMagneticElements;
    MAGtype_GeoMagneticElements Errors;

    // Parse latitude, longitude, altitude, and year from the arguments
    double latitude = atof(argv[1]);
    double longitude = atof(argv[2]);
    double altitude = atof(argv[3]);
    double year = atof(argv[4]);

    // Create input structures
    ingestPoint(latitude, longitude, altitude, year, &CoordGeodetic, &UserDate);

    // Calculate the field at the point and time
    calculateMagneticField(&CoordGeodetic, &UserDate, &GeoMagneticElements, &Errors);

    // Print the results
   MAG_PrintUserDataWithUncertaintySimplified(GeoMagneticElements, Errors, CoordGeodetic, UserDate);

}