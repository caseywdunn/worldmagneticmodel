/*--------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "GeomagnetismHeader.h"

/*---------------------------------------------------------------------------*/

#define MAX_LINE_LENGTH 256
#define NUM_FIELDS 18

typedef struct {
    double decimal_year;
    int altitude;
    int latitude;
    int longitude;
    double declination;
    double inclination;
    double H;
    double X;
    double Y;
    double Z;
    double F;
    double dD_dt;
    double dI_dt;
    double dH_dt;
    double dX_dt;
    double dY_dt;
    double dZ_dt;
    double dF_dt;
} WMMData;

void parse_line(char *line, WMMData *data) {
    sscanf(line, "%lf %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
           &data->decimal_year, &data->altitude, &data->latitude, &data->longitude,
           &data->declination, &data->inclination, &data->H, &data->X, &data->Y, &data->Z,
           &data->F, &data->dD_dt, &data->dI_dt, &data->dH_dt, &data->dX_dt, &data->dY_dt,
           &data->dZ_dt, &data->dF_dt);
}


int run_tests(char *filename){
    printf("Running tests in file %s\n", filename);

        FILE *file = fopen("WMM2025_TestValues.txt", "r");
    if (!file) {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

    char line[MAX_LINE_LENGTH];
    WMMData data[100]; // Assuming there are at most 100 data lines
    int data_count = 0;

    while (fgets(line, sizeof(line), file)) {
        // Skip comment lines
        if (line[0] == '#') {
            continue;
        }

        // Parse the line into the data structure
        parse_line(line, &data[data_count]);
        data_count++;
    }

    fclose(file);

    // Print the parsed data
    for (int i = 0; i < data_count; i++) {
        printf("Record %d: %lf %d %d %d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n",
               i + 1, data[i].decimal_year, data[i].altitude, data[i].latitude, data[i].longitude,
               data[i].declination, data[i].inclination, data[i].H, data[i].X, data[i].Y, data[i].Z,
               data[i].F, data[i].dD_dt, data[i].dI_dt, data[i].dH_dt, data[i].dX_dt, data[i].dY_dt,
               data[i].dZ_dt, data[i].dF_dt);
    }

    int success = TRUE;

    // Loop through the data and compare the calculated values with the expected values
    for (int i = 0; i < data_count; i++) {
        MAGtype_Date UserDate;
        MAGtype_CoordGeodetic CoordGeodetic;
        MAGtype_GeoMagneticElements GeoMagneticElements;
        MAGtype_GeoMagneticElements Errors;

        ingestPoint(data[i].latitude, data[i].longitude, data[i].altitude, data[i].decimal_year, &CoordGeodetic, &UserDate);
        calculateMagneticField(&CoordGeodetic, &UserDate, &GeoMagneticElements, &Errors);

        printf("declination, inclination, H, X, Y, Z, F, decimal_year\n");
        printf("Record %d\n", i + 1);
        printf("Expected: %lf %lf %lf %lf %lf %lf %lf %lf\n", data[i].declination, data[i].inclination, data[i].H, data[i].X, data[i].Y, data[i].Z, data[i].F, data[i].decimal_year);
        printf("Calculated: %lf %lf %lf %lf %lf %lf %lf %lf\n", GeoMagneticElements.Decl, GeoMagneticElements.Incl, GeoMagneticElements.H, GeoMagneticElements.X, GeoMagneticElements.Y, GeoMagneticElements.Z, GeoMagneticElements.F, data[i].decimal_year);

        double epsilon = 0.1;

        if (fabs(abs(GeoMagneticElements.Decl - data[i].declination)/data[i].declination) > epsilon) {
            printf("Declination error\n");
            success = FALSE;
        }
        if (fabs(abs(GeoMagneticElements.Incl - data[i].inclination)/data[i].inclination) > epsilon) {
            printf("Inclination error\n");
            success = FALSE;
        }
        if (fabs(abs(GeoMagneticElements.H - data[i].H)/data[i].H) > epsilon) {
            printf("H error\n");
            success = FALSE;
        }
        if (fabs(abs(GeoMagneticElements.X - data[i].X)/data[i].X) > epsilon) {
            printf("X error\n");
            success = FALSE;
        }
        if (fabs(abs(GeoMagneticElements.Y - data[i].Y)/data[i].Y) > epsilon) {
            printf("Y error\n");
            success = FALSE;
        }
        if (fabs(abs(GeoMagneticElements.Z - data[i].Z)/data[i].Z) > epsilon) {
            printf("Z error\n");
            success = FALSE;
        }
        if (fabs(abs(GeoMagneticElements.F - data[i].F)/data[i].F) > epsilon) {
            printf("F error\n");
            success = FALSE;
        }


    }

    if (success) {
        printf("All tests passed\n");
        return 0;
    } else {
        printf("Some tests failed\n");
        return 1;
    }

}

int main(int argc, char *argv[]){

    if (argc < 2) {
        printf("Usage: %s [-t testfile] latitude longitude altitude year\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-t") == 0) {
        if (argc < 3) {
            printf("Usage: %s -t testfile\n", argv[0]);
            return 1;
        }
        return run_tests(argv[2]);
    }

    if (argc < 5) {
        printf("Usage: %s latitude longitude altitude year\n", argv[0]);
        return 1;
    }

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