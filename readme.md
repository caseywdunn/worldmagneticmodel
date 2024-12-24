# WMM Decoder

This implements the World Magnetic Model (WMM) decoder as described in the [NOAA Technical Report](https://repository.library.noaa.gov/view/noaa/24390) section 1.2. 

## Contents

- `WMM2020_Linux/` the reference implementation available at https://www.ncei.noaa.gov/products/world-magnetic-model . This directory is preserved in its original form.

## Background



## Notes on the NOAA report

"All variables in this section adhere to the following measurement conventions: angles are in radians, lengths are in meters, magnetic intensities are in nanoteslas (nT)"

"A step by step procedure is provided below for computing the magnetic field elements at a given location and time (λ, φ, hMSL, t), where λ and φ are the longitude and geodetic latitude, hMSL is height above Mean Sea Level (MSL), and t is the time given in decimal years."

"In the first step, the user provides the time, location and height above MSL at which the magnetic elements are to be calculated. The height above MSL is then converted to height h above the WGS 84 ellipsoid by using the geopotential model EGM96" This step has minor impact on results.

"The geodetic coordinates (λ, φ, h) are then transformed into spherical geocentric coordinates (λ, φ´, r) by recognizing that λ is the same in both coordinate systems, and that (φ´, r) is computed from (φ, h) according to the equations (7)"

"In the second step, the Gauss coefficients ... are determined for the desired time t from the model coefficients (9)"

"In the third step, the field vector components X´, Y´ and Z´ in geocentric coordinates are computed
as (10-12)"

"In the fourth step, the geocentric magnetic field vector components X´, Y´ and Z´, are rotated into the ellipsoidal reference frame, using (17-18)"

"In the last step, the magnetic elements H, F, I and D are computed from the orthogonal components (19-20)"

## Notes on reference implementation

Examining `wmm_point.c`, which is a thin front end to `GeomagnetismLibrary`.

`wmm_point.c` has main function

### Reading the model

A call in `main()` to `MAG_robustReadMagModels(filename, &MagneticModels, epochs)` reads the model:

    char filename[] = "WMM.COF";
    MAGtype_MagneticModel * MagneticModels[1], *TimedMagneticModel;
    int epochs = 1;
    MAG_robustReadMagModels(filename, &MagneticModels, epochs)

Here is the `MAGtype_MagneticModel` struct:

    typedef struct {
        double EditionDate;
        double epoch; /*Base time of Geomagnetic model epoch (yrs)*/
        char ModelName[32];
        double *Main_Field_Coeff_G; /* C - Gauss coefficients of main geomagnetic model (nT) Index is (n * (n + 1) / 2 + m) */
        double *Main_Field_Coeff_H; /* C - Gauss coefficients of main geomagnetic model (nT) */
        double *Secular_Var_Coeff_G; /* CD - Gauss coefficients of secular geomagnetic model (nT/yr) */
        double *Secular_Var_Coeff_H; /* CD - Gauss coefficients of secular geomagnetic model (nT/yr) */
        int nMax; /* Maximum degree of spherical harmonic model */
        int nMaxSecVar; /* Maximum degree of spherical harmonic secular model */
        int SecularVariationUsed; /* Whether or not the magnetic secular variation vector will be needed by program*/
        double CoefficientFileEndDate; 
        
    } MAGtype_MagneticModel;



### Date ingestion

`main()` calls `MAG_GetUserInput` to get user input in lat, lon, alt, date. Lat and lon are in positive and negative degrees, alt is in kilometers, date is in decimal years.

calls `MAG_GetDeg()` and `MAG_GetAltitude()`. `MAG_GetDeg(char* Query_String, double* latitude, double bounds[2])` does strong parsing and updates value at pointer to latitude, which then gets loaded into `CoordGeodetic` struct.

    strcpy(buffer, ""); /*Clear the input    */
    strcpy(Qstring, "\nPlease enter latitude\nNorth latitude positive, For example:\n30, 30, 30 (D,M,S) or 30.508 (Decimal Degrees) (both are north)\n");
    MAG_GetDeg(Qstring, &CoordGeodetic->phi, lat_bound);
    strcpy(buffer, ""); /*Clear the input*/
    strcpy(Qstring,"\nPlease enter longitude\nEast longitude positive, West negative.  For example:\n-100.5 or -100, 30, 0 for 100.5 degrees west\n");
    MAG_GetDeg(Qstring, &CoordGeodetic->lambda, lon_bound);

Here is the structure that is being populated:

    typedef struct {
        double lambda; /* longitude */
        double phi; /* geodetic latitude */
        double HeightAboveEllipsoid; /* height above the ellipsoid (HaE) */
        double HeightAboveGeoid; /* (height above the EGM96 geoid model ) */
        int UseGeoid;
    } MAGtype_CoordGeodetic;

`MAG_GetAltitude()` also updates the same `MAGtype_CoordGeodetic` struct.

If user enters hight above elipsoid, it is entedred into both the `HeightAboveEllipsoid` and `HeightAboveGeoid` fields.

If the user enters hight above MSL, then it is stored in `HeightAboveGeoid` and `MAG_ConvertGeoidToEllipsoidHeight()` is called to populate `HeightAboveEllipsoid`. 

It then parses the date and stores it in `MAGtype_Date` struct:

    typedef struct {
        int Year;
        int Month;
        int Day;
        double DecimalYear; /* decimal years */
    } MAGtype_Date;

So inputting user data results in two updated structs: `MAGtype_CoordGeodetic` and `MAGtype_Date`. Note that location is still in degrees.

Here is the template:

    int MAG_GetUserInput(MAGtype_MagneticModel *MagneticModel, MAGtype_Geoid *Geoid, MAGtype_CoordGeodetic *CoordGeodetic, MAGtype_Date *MagneticDate)

And here is the call to it from `main()`:

    MAG_GetUserInput(MagneticModels[0], &Geoid, &CoordGeodetic, &UserDate) == 1

### Calculating the magnetic field

This is then the entire call in `main()` to calculate the magnetic field:

            MAG_GeodeticToSpherical(Ellip, CoordGeodetic, &CoordSpherical); /*Convert from geodetic to Spherical Equations: 17-18, WMM Technical report*/
            MAG_TimelyModifyMagneticModel(UserDate, MagneticModels[0], TimedMagneticModel); /* Time adjust the coefficients, Equation 19, WMM Technical report */
            MAG_Geomag(Ellip, CoordSpherical, CoordGeodetic, TimedMagneticModel, &GeoMagneticElements); /* Computes the geoMagnetic field elements and their time change*/
            MAG_CalculateGridVariation(CoordGeodetic, &GeoMagneticElements);
            MAG_WMMErrorCalc(GeoMagneticElements.H, &Errors);
            MAG_PrintUserDataWithUncertainty(GeoMagneticElements, Errors, CoordGeodetic, UserDate, TimedMagneticModel, &Geoid); /* Print the results */

The first step in `MAG_GeodeticToSpherical()` is to convert the lat and lon from degrees to radians:

    CosLat = cos(DEG2RAD(CoordGeodetic.phi));
    SinLat = sin(DEG2RAD(CoordGeodetic.phi));

## Notes on Yale cubesat code

`FRESULT Geomag_GetMagEquatorial(time_t* t, const Vec3D_t* SatEquatorial, Vec3D_t* MagEquatorial)`

# Our implementation

There are a few steps and assumptions in this implementation.


- We will assume that altitude is relative to the WGS 84 ellipsoid.