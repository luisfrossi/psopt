//////////////////////////////////////////////////////////////////////////
////////////////           PSOPT  Example             ////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////// Title:                 Bryson-Denham problem     ////////////////
//////// Last modified:         05 January 2009           ////////////////
//////// Reference:             GPOPS Handbook            ////////////////
//////// (See PSOPT handbook for full reference)           ///////////////
//////////////////////////////////////////////////////////////////////////
////////     Copyright (c) Victor M. Becerra, 2009        ////////////////
//////////////////////////////////////////////////////////////////////////
//////// This is part of the PSOPT software library, which ///////////////
//////// is distributed under the terms of the GNU Lesser ////////////////
//////// General Public License (LGPL)                    ////////////////
//////////////////////////////////////////////////////////////////////////


#include "psopt.h"

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the end point (Mayer) cost function //////////
//////////////////////////////////////////////////////////////////////////

adouble endpoint_cost(adouble* initial_states, adouble* final_states,
                      adouble* parameters,adouble& t0, adouble& tf,
                      adouble* xad, int iphase, Workspace* workspace)
{
    adouble x3f = final_states[ CINDEX(3) ];

    return x3f;
}

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the integrand (Lagrange) cost function  //////
//////////////////////////////////////////////////////////////////////////

adouble integrand_cost(adouble* states, adouble* controls,
                       adouble* parameters, adouble& time, adouble* xad,
                       int iphase, Workspace* workspace)
{
    return  0.0;
}

//////////////////////////////////////////////////////////////////////////
///////////////////  Define the DAE's ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void dae(adouble* derivatives, adouble* path, adouble* states,
         adouble* controls, adouble* parameters, adouble& time,
         adouble* xad, int iphase, Workspace* workspace )
{

   adouble x1 = states[CINDEX(1)];
   adouble x2 = states[CINDEX(2)];
   adouble x3 = states[CINDEX(3)];

   adouble u = controls[CINDEX(1)];

   derivatives[ CINDEX(1) ] = x2;
   derivatives[ CINDEX(2) ] = u;
   derivatives[ CINDEX(3) ] = u*u/2;
}



////////////////////////////////////////////////////////////////////////////
///////////////////  Define the events function ////////////////////////////
////////////////////////////////////////////////////////////////////////////

void events(adouble* e, adouble* initial_states, adouble* final_states,
            adouble* parameters,adouble& t0, adouble& tf, adouble* xad,
            int iphase, Workspace* workspace)
{
   adouble x10 = initial_states[ CINDEX(1) ];
   adouble x20 = initial_states[ CINDEX(2) ];
   adouble x30 = initial_states[ CINDEX(3) ];
   adouble x1f = final_states[ CINDEX(1) ];
   adouble x2f = final_states[ CINDEX(2) ];

   e[ CINDEX(1) ] = x10;
   e[ CINDEX(2) ] = x20;
   e[ CINDEX(3) ] = x30;
   e[ CINDEX(4) ] = x1f;
   e[ CINDEX(5) ] = x2f;
}


///////////////////////////////////////////////////////////////////////////
///////////////////  Define the phase linkages function ///////////////////
///////////////////////////////////////////////////////////////////////////

void linkages( adouble* linkages, adouble* xad, Workspace* workspace)
{
  // No linkages as this is a single phase problem
}


////////////////////////////////////////////////////////////////////////////
///////////////////  Define the main routine ///////////////////////////////
////////////////////////////////////////////////////////////////////////////


int main(void)
{

////////////////////////////////////////////////////////////////////////////
///////////////////  Declare key structures ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    Alg  algorithm;
    Sol  solution;
    Prob problem;

////////////////////////////////////////////////////////////////////////////
///////////////////  Register problem name  ////////////////////////////////
////////////////////////////////////////////////////////////////////////////

    problem.name        		= "Bryson-Denham Problem";
    problem.outfilename                 = "bryden.txt";

////////////////////////////////////////////////////////////////////////////
////////////  Declare problem level constants & do level 1 setup ///////////
////////////////////////////////////////////////////////////////////////////

    problem.nphases   			= 1;
    problem.nlinkages                   = 0;

    psopt_level1_setup(problem);


/////////////////////////////////////////////////////////////////////////////
/////////   Define phase related information & do level 2 setup /////////////
/////////////////////////////////////////////////////////////////////////////


    problem.phases(1).nstates   		= 3;
    problem.phases(1).ncontrols 		= 1;
    problem.phases(1).nevents   		= 5;
    problem.phases(1).npath     		= 0;
    problem.phases(1).nodes                     = "[10, 50]";

    psopt_level2_setup(problem, algorithm);

////////////////////////////////////////////////////////////////////////////
///////////////////  Declare DMatrix objects to store results //////////////
////////////////////////////////////////////////////////////////////////////

    DMatrix x, u, t;
    DMatrix lambda, H;

////////////////////////////////////////////////////////////////////////////
///////////////////  Enter problem bounds information //////////////////////
////////////////////////////////////////////////////////////////////////////


    problem.phases(1).bounds.lower.states(1) 		= 0.0;
    problem.phases(1).bounds.lower.states(2) 		= -10.0;
    problem.phases(1).bounds.lower.states(3) 		= -10.0;

    problem.phases(1).bounds.upper.states(1)	 	= 1.0/9.0;
    problem.phases(1).bounds.upper.states(2) 		= 10.0;
    problem.phases(1).bounds.upper.states(3) 		= 10.0;

    problem.phases(1).bounds.lower.controls(1)		= -10.0;
    problem.phases(1).bounds.upper.controls(1)	 	=  10.0;

    problem.phases(1).bounds.lower.events(1) 		= 0.0;
    problem.phases(1).bounds.lower.events(2) 		= 1.0;
    problem.phases(1).bounds.lower.events(3) 		= 0.0;
    problem.phases(1).bounds.lower.events(4) 		= 0.0;
    problem.phases(1).bounds.lower.events(5) 		= -1.0;

    problem.phases(1).bounds.upper.events(1) 		= 0.0;
    problem.phases(1).bounds.upper.events(2) 		= 1.0;
    problem.phases(1).bounds.upper.events(3) 		= 0.0;
    problem.phases(1).bounds.upper.events(4) 		= 0.0;
    problem.phases(1).bounds.upper.events(5) 		= -1.0;


    problem.phases(1).bounds.lower.StartTime   		= 0.0;
    problem.phases(1).bounds.upper.StartTime   		= 0.0;
    problem.phases(1).bounds.lower.EndTime     		= 0.0;
    problem.phases(1).bounds.upper.EndTime     		= 50.0;


////////////////////////////////////////////////////////////////////////////
///////////////////  Register problem functions  ///////////////////////////
////////////////////////////////////////////////////////////////////////////


    problem.integrand_cost 	= &integrand_cost;
    problem.endpoint_cost 	= &endpoint_cost;
    problem.dae 		= &dae;
    problem.events 		= &events;
    problem.linkages		= &linkages;

////////////////////////////////////////////////////////////////////////////
///////////////////  Define & register initial guess ///////////////////////
////////////////////////////////////////////////////////////////////////////

    DMatrix x0(3,10);

    x0(1,colon()) = linspace(0.0, 0.0, 10);
    x0(2,colon()) = linspace(1.0,-1.0, 10);
    x0(3,colon()) = linspace(0.0, 0.0, 10);

    problem.phases(1).guess.controls       = zeros(1,10);
    problem.phases(1).guess.states         = x0;
    problem.phases(1).guess.time           = linspace(0.0, 0.5, 10);


////////////////////////////////////////////////////////////////////////////
///////////////////  Enter algorithm options  //////////////////////////////
////////////////////////////////////////////////////////////////////////////

    algorithm.nlp_method                  = "IPOPT";
    algorithm.scaling                     = "automatic";
    algorithm.derivatives                 = "automatic";
    algorithm.nlp_iter_max                = 1000;
    algorithm.nlp_tolerance               = 1.e-6;

////////////////////////////////////////////////////////////////////////////
///////////////////  Now call PSOPT to solve the problem   /////////////////
////////////////////////////////////////////////////////////////////////////


    psopt(solution, problem, algorithm);


////////////////////////////////////////////////////////////////////////////
///////////  Extract relevant variables from solution structure   //////////
////////////////////////////////////////////////////////////////////////////

    x      = solution.get_states_in_phase(1);
    u      = solution.get_controls_in_phase(1);
    t      = solution.get_time_in_phase(1);
    lambda = solution.get_dual_costates_in_phase(1);
    H      = solution.get_dual_hamiltonian_in_phase(1);

////////////////////////////////////////////////////////////////////////////
///////////  Save solution data to files if desired ////////////////////////
////////////////////////////////////////////////////////////////////////////

    x.Save("x.dat");
    u.Save("u.dat");
    t.Save("t.dat");
    lambda.Save("lambda.dat");
    H.Save("H.dat");

////////////////////////////////////////////////////////////////////////////
///////////  Plot some results if desired (requires gnuplot) ///////////////
////////////////////////////////////////////////////////////////////////////

    plot(t,x,problem.name, "time (s)", "states", "x1 x2 x3");

    plot(t,u, problem.name ,"time (s)", "control", "u");

    plot(t,lambda, problem.name ,"time (s)", "lambda", "1 2 3");

    plot(t,x,problem.name, "time (s)", "states", "x1 x2 x3",
                                  "pdf", "bryden_states.pdf");

    plot(t,u, problem.name ,"time (s)", "control", "u",
                                  "pdf", "bryden_control.pdf");

    plot(t,lambda, problem.name ,"time (s)", "lambda", "1 2 3",
                                  "pdf", "bryden_lambda.pdf");





}

////////////////////////////////////////////////////////////////////////////
///////////////////////      END OF FILE     ///////////////////////////////
////////////////////////////////////////////////////////////////////////////

