## Creates file "test.rfd" with fake RoI's in it.

## opens file for writing
[fid, msg] = fopen ("test.rfd", "w");

lvl1 = 1;
eta = 1.05;
phi = 1.0308354;
fprintf(fid, "RoI 0 eta: %f phi: %f LVL1 ID: %d\n", eta, phi, lvl1);
makecells(fid, 1, 0.0031250, 0.098175, eta, phi, 129, 5, 10.0, 1.0);
makecells(fid, 2, 0.025, 0.0245437, eta, phi, 17, 17, 10.0, 1.0);
makecells(fid, 3, 0.05, 0.0245437, eta, phi, 9, 17, 10.0, 1.0);
makecells(fid, 12, 0.1, 0.098175, eta, phi, 5, 5, 10.0, 1.0);
makecells(fid, 13, 0.1, 0.098175, eta, phi, 5, 5, 10.0, 1.0);
makecells(fid, 14, 0.1, 0.098175, eta, phi, 5, 5, 10.0, 1.0);

## phi wrap around region
lvl1 = 2;
eta = 1.05;
phi = 6.1113794589;
fprintf(fid, "RoI 0 eta: %f phi: %f LVL1 ID: %d\n", eta, phi, lvl1);
makecells(fid, 1, 0.0031250, 0.098175, eta, phi, 129, 5, 10.0, 1.0);
makecells(fid, 2, 0.025, 0.0245437, eta, phi, 17, 17, 10.0, 1.0);
makecells(fid, 3, 0.05, 0.0245437, eta, phi, 9, 17, 10.0, 1.0);
makecells(fid, 12, 0.1, 0.098175, eta, phi, 5, 5, 10.0, 1.0);
makecells(fid, 13, 0.1, 0.098175, eta, phi, 5, 5, 10.0, 1.0);
makecells(fid, 14, 0.1, 0.098175, eta, phi, 5, 5, 10.0, 1.0);

fclose(fid);


