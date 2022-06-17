#include "udf.h"

/*
THIS IS ANSYS FLUENT UDF FILE. It is a macro, based on C language, but some of the syntax rules are different. It will not compile in clang.
Ansys Fluent must be used for use or testing.

UDF for simple t^square fire curve, for heat and smoke generation from source, based on Mulholland formula.

AUTHOR: Piotr Jażdżyk
Contact: piotr.jazdzyk@o2.pl
Linkedin: https://www.linkedin.com/in/pjazdzyk/
*/

// REQUIRED USER INPUT
#define HRR_MAX 2500.0    // Peak heat release rate in KW
#define GROWTH_COEF 0.045 // Fire growth coefficient in kW/s^2
#define SRC_VOL 10.5      // Source geometry volume in m3
#define SOOT_Y 0.12       // Soot yield in kg.soot/kg.fuel
#define HC 25.0           // Burning material heat of combustion in MJ/kg

real get_current_HRR(real time)
{
    return MIN(HRR_MAX, GROWTH_COEF * time * time) * 1000.0 / SRC_VOL; // in W/m3
}

real get_smoke_flow(real time)
{
    return get_current_HRR(time) / (HC * 1000000.0) * SOOT_Y / SRC_VOL;
}

DEFINE_SOURCE(heat_release, c, ct, dS, eqn)
{
    real current_hrr = get_current_HRR(CURRENT_TIME);
    dS[eqn] = 2000.0 * (GROWTH_COEF * CURRENT_TIME) / SRC_VOL;
    return current_hrr;
}

DEFINE_SOURCE(soot_release, c, ct, dS, eqn)
{
    real smoke_emission = get_smoke_flow(CURRENT_TIME);
    dS[eqn] = (GROWTH_COEF * SOOT_Y * CURRENT_TIME) / (500.0 * HC * SRC_VOL * SRC_VOL);
    return smoke_emission; // in kg/s/m3
}

void print_test_curve()
{
    real max_time = 900; // s
    real steps = 15;
    real time_incr = max_time / steps;
    real hrr;
    real smoke_flow;
    real time;
    real i;

    printf("%s\n", "FIRE CURVE VALUES:");

    for (i = 0; i <= steps; i++)
    {
        if (i == 0)
        {
            time = 0;
        }
        else
        {
            time += time_incr;
        }

        hrr = get_current_HRR(time);       // in W/m3
        smoke_flow = get_smoke_flow(time); // in kg/s/m3

        printf("Time = %.1f s  |  HRR = %.2f kW  |  G.smoke = %.4f kg/s |  SRC_VOL = %.2f m3  |  HRR/Vol = %.4f W/m3  |  G.smoke/Vol = %.6f kg/s/m3 \n", time, (hrr / 1000) * SRC_VOL, smoke_flow * SRC_VOL, SRC_VOL, hrr, smoke_flow);
    }

    printf("\n");
}