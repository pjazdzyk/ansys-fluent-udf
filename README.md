## ANSYS FLUENT: USER DEFINED FUNCTIONS (UDF) 

> AUTHOR: <b>Piotr Jażdżyk</b> <br>
> COMPANY: SYNERSET | www.synerset.com
> CONTACT: info@synerset.com
> VERSION: 1.0.0 <br>
> DATE: 2022.04.10<br>
> LAST UPDATE: 2022.04.10<br>

---

### LOGHARITMIC WIND PROFILE BASED ON EUROCODE EN 1991-1-4
FILENAME: EurocodeWindProfile.c
Interpreted type UDF for Ansys Fluent wind simulations. This UDF implements a vertical velocity profile as the function of
height, terrain type, terrain roughness, and reference velocity at a height of 10m. This class follows the strict methodology
of the "Eurocode EN 1991-1-4 (2005) Eurocode 1: Actions on structures - Part 1-4: General actions - Wind actions".
Function profiles of vx and vy must be hooked into the correct BC in Fluent. 
To quickly investigate if the velocity profile is correct - just invoke printAll() method. UDF supports user-defined wind
direction including also the angle of rotation for the site.

Please note that turbulence parameter profiles are calculated according to Eurocode (turbulent kinetic energy k and turbulence intensity ratio)
might be problematic for convergence, and further investigation must be done to properly determine ke and tu profiles.

UDF is may be used as velocity inlet BC for any most large scale aerodynamic wind studies for building industry, for an example:
* static pressure analysis on building facades
* wind comfort studies (Lawson criteria)
* pollution spread in the environment

### USER GUIDE
1. Determine appropriate reference velocity at height of 10m suitable for your case (for structural analysis use values provided in Eurocode)
2. Determine your wind direction
3. Determine your terrain class based on table 4.1 in Eurocode
4. Adjust fields in UDF file accordingly:
```                                                     
#define WIND_DIR	270.0	<- wind direction here                                                            
#define z0		0.003	<- z0 value based on terrain category from table 4.1                 
#define zMin		1.000	<- zMin value based on terrain category from table 4.1                                      
#define zoneH		400.0	<- your domain height in m                             
#define vB0		22.0	<- velocity at reference height of 10m                                                        

```
5. Under velocity inlet BC, select velocity specified in cartesian coordinate system: vx, by, vz.
6. Hook x_vel_profile as vx, and y_vel_profile as vy in each velicity inlet BC.
7. Leave at least one BC as pressure outlet.
8. Use printAll() method, to check calculated values.

Table 4.1:
z0[m]	zmin[m] 	Category
- - - - - - - - - - - - - 
0.003	1		Category 0	– Sea or coastal area exposed to open sea 0.003 1
0.01	1		Category I	– Lakes or flat and horizontal area with negligible vegetation and without obstacles
0.05	2		Category II	- Area with low vegetation such as grass and isolated obstacles (trees, buildings) with separations of at least 20 obstacle heights
0.3	5		Category III	- Area with regular cover of vegetation or buildings or with isolated obstacles with separations of maximum 20 obstacle heights
1.0	10		Category IV	- Area in which at least 15% of the surface is covered with buildings and their average height exceeds 15 m


### LICENSE
Free for academic and educational use.