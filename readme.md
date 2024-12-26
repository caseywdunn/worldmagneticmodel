# WMM Decoder

This is a reduced implementation of the World Magnetic Model (WMM) decoder provided by NOAA. The intent is to have a simplre interface that can be called from other code.

The World Magnetic Model (WMM) is described in the [NOAA Technical Report](https://repository.library.noaa.gov/view/noaa/24390) section 1.2.

## Contents

- `WMM2020_Linux/` the reference implementation available at https://www.ncei.noaa.gov/products/world-magnetic-model . This directory is preserved in its original form.

- `src/` a reduced implementation meant to be called from other code in a minimal environment. Incorporates some code from `wmm_point.c` into a simplified `GeomagnetismLibrary.c`

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
