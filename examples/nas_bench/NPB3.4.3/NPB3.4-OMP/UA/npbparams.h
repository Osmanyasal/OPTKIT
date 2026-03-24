! CLASS = D
!  
!  
!  This file is generated automatically by the setparams utility.
!  It sets the number of processors and the class of the NPB
!  in this directory. Do not modify it by hand.
!  
        integer          lelt, lmor, refine_max, fre_default
        integer          niter_default, nmxh_default
        character        class_default
        double precision alpha_default
        parameter(  lelt=514400, &
     &             lmor=19134400, &
     &              refine_max=10, &
     &              fre_default=5, &
     &              niter_default=250, &
     &              nmxh_default=10, &
     &              class_default="D", &
     &              alpha_default=0.046d0 )
        logical  convertdouble
        parameter (convertdouble = .false.)
        character compiletime*11
        parameter (compiletime='21 Mar 2026')
        character npbversion*5
        parameter (npbversion='3.4.3')
        character cs1*8
        parameter (cs1='gfortran')
        character cs2*5
        parameter (cs2='$(FC)')
        character cs3*6
        parameter (cs3='(none)')
        character cs4*6
        parameter (cs4='(none)')
        character cs5*12
        parameter (cs5='-O3 -fopenmp')
        character cs6*9
        parameter (cs6='$(FFLAGS)')
        character cs7*6
        parameter (cs7='randi8')
