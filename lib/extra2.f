c
c
c     ###################################################
c     ##  COPYRIGHT (C)  1990  by  Jay William Ponder  ##
c     ##              All Rights Reserved              ##
c     ###################################################
c
c     ############################################################
c     ##                                                        ##
c     ##  subroutine extra2  --  atomwise user defined Hessian  ##
c     ##                                                        ##
c     ############################################################
c
c
c     "extra2" calculates second derivatives of any additional
c     user defined potential energy contribution for a single
c     atom at a time
c
c
      subroutine extra2 (i)
      use sizes
      use atoms
      use hessn
      use atomid
      use deriv
      use energi
      use umbrella
      use bound
      use iounit
      use virial
      implicit none
      integer i
c
c     compute the Hessian elements for extra energy terms
c
c     do j = 1, n
c        hessx(1,j) = hessx(1,j) + ......
c        hessy(2,j) = hessy(2,j) + ......
c        hessz(3,j) = hessz(3,j) + ......
c     end do
c

c     ADD  Umbrella Sampling Code
      if ( pull_switch .eq. 0) then
        return
      else
        write(iout,*) "warning ! not implemented code"
        stop
      end if


      return
      end
