## ANSYS FLUENT: USER DEFINED FUNCTIONS (UDF) 

> AUTHOR: <b>Piotr Jażdżyk</b> <br>
> VERSION: 1.0.0 <br>
> DATE: 2022.04.10<br>
> LAST UPDATE: 2022.04.10<br>

---

### LOGHARITMIC WIND PROFILE BASED ON EUROCODE (EurocodeWindProfile.cpp)
Interpreted type UDF for Ansys Fluent wind simulations. This UDF implements a vertical velocity profile as the function of
height, terrain type, terrain roughness, and reference velocity at a height of 10m. This class follows the strict methodology
of the "Eurocode EN 1991-1-4 (2005) Eurocode 1: Actions on structures - Part 1-4: General actions - Wind actions".
Function profiles of vx and vy must be hooked into the correct BC in Fluent. 
To quickly investigate if the velocity profile is correct - just invoke printAll() method. UDF supports user-defined wind
direction including also the angle of rotation for the site.

Please note that turbulence parameter profiles are calculated according to Eurocode (turbulent kinetic energy k and turbulence intensity ratio)
might be problematic for convergence, and further investigation must be done to properly determine ke and tu profiles.

UDF is dedicated for any of large scale aerodynamic wind studies for building industry:
* static pressure analysis on building facades
* wind comfort studies (Lawson criteria)
* pollution spread in the environment