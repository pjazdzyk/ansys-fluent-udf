#include "udf.h"

/*
THIS IS ANSYS FLUENT UDF FILE. It is a macro, based on C language, but some of the syntax rules are different. It will not compile in clang.
Ansys Fluent must be used for use or testing.

UDF for simulation ABL (atmospheric boundary layer) as logaritmic wind profile, based on Eurocode EN 1991-1-4.

AUTHOR: Piotr Jażdżyk
Contact: piotr.jazdzyk@o2.pl
Linkedin: https://www.linkedin.com/in/pjazdzyk/

Table 4.1 from Eurocode Eurocode EN 1991-1-4:
z0[m] - aerodynamic terrain roughness length
z0[m] / zmin[m]  / Category
0.003 / 1  	 / Category 0 	– Sea or coastal area exposed to open sea 0.003 1
0.01  / 1  	 / Category I 	– Lakes or flat and horizontal area with negligible vegetation and without obstacles
0.05  / 2  	 / Category II	- Area with low vegetation such as grass and isolated obstacles (trees, buildings) with separations of at least 20 obstacle heights
0.3   / 5  	 / Category III - Area with regular cover of vegetation or buildings or with isolated obstacles with separations of maximum 20 obstacle heights
1.0   / 10   / Category IV	- Area in which at least 15% of the surface is covered with buildings and their average height exceeds 15 m
*/

// USER INPUT: DEFAULTS & CONSTANTS
#define PI 3.1415927 // Constant (PI)
#define K 0.42		 // von Karmans constant
#define z0II 0.05	 // terrain roughness heigh for second category [m]
#define Cu 0.09		 // turbulence model constant

// USER INPUT: WIND DIRECTION PROPERTIES
#define TN_CORR 0.0	   // Degrees Correction for True North to Plant North (clockwise positive) default = 0
#define WIND_DIR 270.0 // Wind Direction in clockwise degrees from True North
					   /* South wind means: it blows from south to north
						180.0 = North Wind
						135.0 = North West Wind
						270.0 = East Wind
						  0.0 = South Wind
						 45.0 = South West Wind
						 90.0 = West Wind */

// USER INPUT: DESIGN TERRAIN PROPERTIES
#define ZGROUND 0.000 // Z location of the ground default = 0 [m]
#define z0 0.003	  // arodynamic terrain roughness lengt defined in table 4.1 [m]
#define zMin 1.000	  // minimum height defined in table  4.1 [m]
#define zoneH 400.0	  // Maximum computational domain height

// USER INPUT: DESIGN WIND PROPERTY
#define vB0 22.0	 // basic wind velocity, at the reference height of z=10m [m/s]
#define cDir 1.00	 // directional factor -> see national annex, default is 1
#define cSeason 1.00 // seasonal factor -> see national annex, default is 1
#define c0z 1.00	 // oreography factor, taken as 1 unless otherwise specified in 4.3.3
#define kI 1.00		 // turbulence factor, default is 1

// CALCULATION FIELDS
real theta = (WIND_DIR + TN_CORR) * PI / 180; // angle for wind vectors calculation
real vB = vB0 * cDir * cSeason;				  // design velocity, at the reference height of z=10m [m/s]

extern Domain *domain;

real calcKr()
{
	return 0.19 * pow((z0 / z0II), 0.07);
}

real calcDelV()
{
	real kr = calcKr();
	return kr * vB * kI;
}

real calcCrz(real z)
{
	real kr = calcKr();
	if (z <= zMin)
		return kr * log(zMin / z0);
	else
		return kr * log(z / z0);
}

real calcVmz(real z)
{
	if (z <= 0)
		return (real)0.0;
	else
		return calcCrz(z) * c0z * vB;
}

real calcLenScale()
{
	real lenSc = 0.07 * zoneH;
	return lenSc;
}

real calcIu(real z)
{
	real delV = calcDelV();
	if (z <= 0)
		return (real)0.0;
	else if (z <= zMin)
		return delV / calcVmz(zMin);
	else
		return delV / calcVmz(z);
}

real calcTke(real z)
{
	real vel = calcVmz(z);
	real iu = calcIu(z);

	if (z <= 0)
		return (real)0.0;

	return (3.0 / 2.0) * pow((vel * iu), 2.0);
}

real calcTdr(real z)
{
	real k = calcTke(z);
	real l = calcLenScale();

	return pow(Cu, (3.0 / 4.0)) * pow(k, (3.0 / 2.0)) / l;
}

real calcSdr(real z)
{
	real k = calcTke(z);
	real l = calcLenScale();
	return pow(k, 0.5) / pow(Cu, 0.25) * l;
}

DEFINE_PROFILE(x_vel_profile, thread, position)
{
	real vel, xv, zloc;
	real x[ND_ND];
	face_t f;
	begin_f_loop(f, thread)
	{
		F_CENTROID(x, f, thread);
		zloc = x[2] - ZGROUND;
		vel = calcVmz(zloc);   // wind velocity at height z
		xv = sin(theta) * vel; // CA wind veloicty: x compound
		F_PROFILE(f, thread, position) = xv;
	}
	end_f_loop(f, thread)
}

DEFINE_PROFILE(y_vel_profile, thread, position)
{
	real vel, yv, zloc;
	real x[ND_ND];
	face_t f;
	begin_f_loop(f, thread)
	{
		F_CENTROID(x, f, thread);
		zloc = x[2] - ZGROUND;
		vel = calcVmz(zloc);   // wind velocity at height z
		yv = cos(theta) * vel; // CA wind veloicty: y compound
		F_PROFILE(f, thread, position) = yv;
	}
	end_f_loop(f, thread)
}

DEFINE_PROFILE(k_profile, thread, position)
{
	real zloc, tke;
	real x[ND_ND];
	face_t f;
	begin_f_loop(f, thread)
	{
		F_CENTROID(x, f, thread);
		zloc = x[2] - ZGROUND;
		tke = calcTke(zloc);
		F_PROFILE(f, thread, position) = tke;
	}
	end_f_loop(f, thread)
}

DEFINE_PROFILE(e_profile, thread, position)
{
	real zloc, tdr;
	real x[ND_ND];
	face_t f;
	begin_f_loop(f, thread)
	{
		F_CENTROID(x, f, thread);
		zloc = x[1] - ZGROUND;
		tdr = calcTdr(zloc);
		F_PROFILE(f, thread, position) = tdr;
	}
	end_f_loop(f, thread)
}

DEFINE_PROFILE(sdr_profile, thread, position)
{
	real zloc, sdr;
	real x[ND_ND];
	face_t f;
	begin_f_loop(f, thread)
	{
		F_CENTROID(x, f, thread);
		zloc = x[1] - ZGROUND;
		sdr = calcSdr(zloc);
		F_PROFILE(f, thread, position) = sdr;
	}
	end_f_loop(f, thread)
}

// Calculates and prints log wind profile in console based on provided input data
void printAll()
{
	real tempXv;
	real tempYv;
	real tempIu;
	real tempTke;
	real tempTdr;
	real tempSdr;
	real i;
	real kr = calcKr();
	real delV = calcDelV();
	printf("LOG PROFILE INPUT VALUES::");
	printf("\nkr= %3f", kr);
	printf("\nvB= %.3f", vB);
	printf("\ndelV= %.3f", delV);
	printf("\ntheta= %.3f", theta);
	printf("\n\nCALCULATED PROFILE:\n");

	while (i <= zoneH)
	{
		tempXv = calcVmz(i) * sin(theta);
		tempYv = calcVmz(i) * cos(theta);
		tempIu = calcIu(i);
		tempTke = calcTke(i);
		tempTdr = calcTdr(i);
		tempSdr = calcSdr(i);
		printf("z= %.1f\t", i);
		printf("\tnxV= %.3f", tempXv);
		printf("\tyV= %.3f ", tempYv);
		printf("\tIu= %.3f\t", tempIu);
		printf("\tk= %.3f\t", tempTke);
		printf("\te= %.5f\t", tempTdr);
		printf("\tw= %.5f\n", tempSdr);
		if (i < 10)
			i = i + 0.5;
		else if (i >= 10 && i < 20)
			i = i + 1;
		else if (i >= 20 && i < 100)
			i = i + 5;
		else
			i = i + 10;
	}
}