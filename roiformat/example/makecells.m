## Copyright (C) 4-july-2004 André dos Anjos

## usage: makecells (FID, SAMPLING, DETA, DPHI, ETA, PHI, ETACELL,
## PHICELL, PEAK, OTHER)
##
## Creates a new set of cells in RoI dump format, that has a peak of
## energy deposition in ETA and PHI, with value PEAK. Creates a set of
## surrounding cells which span ETACELL cells in eta direction and
## PHICELL cells in phi direction. The values of all other cells is set
## to OTHER. All contents are written to a file given by the FID
## descriptor. The cells will belong to the SAMPLING indicated.
##
## Example:
##
## makecells(fid, 2, 0.025, 0.0245437, 1.05, 1.0308354, 9, 9, 10.0, 1.0);

function makecells (fid, sampling, deta, dphi, eta, phi, etacell, \
		    phicell, peak, other) 

  fprintf(fid, "%d,%f,%f,0,%f,%f,0,%f\n", sampling, eta, phi, deta, \
	  dphi, peak); 

  for i = (-(etacell-1)/2):((etacell-1)/2)
    for j = (-(phicell-1)/2):((phicell-1)/2)
      pphi = phi+(j*dphi);
      ## check for wrap around region
      if ( (pphi-(dphi/2) > 2*pi) || (pphi+(dphi/2) > 2*pi) )
	pphi -= 2*pi;
      endif
      ## print all but the central cell
      if (i != 0 || j != 0)
	fprintf(fid, "%d,%f,%f,0,%f,%f,0,%f\n", sampling, eta+(i*deta), \
		pphi, deta, dphi, other);
      endif
    endfor
  endfor

endfunction


