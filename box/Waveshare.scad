
include <Boxes.scad>

$fa = 1;
$fs = 0.4;

longueurCircuit = 107;
largeurCircuit = 69;

rayonCoin = 2;

hauteurBoite = 13;
hauteurCouvercle = 14;
epaisseurParoi = 4;
epaisseurFond = 2;

hauteurConnexion = 4;

longueurBoite = longueurCircuit + epaisseurParoi * 2;
largeurBoite = largeurCircuit + epaisseurParoi * 2;

module bottomWaveshare()
{
	union()
	{
		difference()
		{
			roundedBox(longueurBoite, largeurBoite, hauteurBoite, rayonCoin);

            // Ouvrir la boite
            //
			translate([ epaisseurParoi, epaisseurParoi, epaisseurFond ])
			{
				cube([ largeurBoite - 2 * epaisseurParoi, longueurBoite - 2 * epaisseurParoi, hauteurBoite ]);
			}

            // Carte SD
            //
			translate([ -0.1, 18, 8 ])
			{
				cube([ epaisseurParoi + 0.2, 18, 6 ]);
			}

            // Ports USB
            //
			translate([ 10, longueurBoite - epaisseurParoi - 0.1, epaisseurFond + 5 ])
			{
				cube([ 40, epaisseurParoi + 0.2, 7 ]);
			}

            // Connexion
            //
			translate([ epaisseurParoi / 2, epaisseurParoi / 2, hauteurBoite - hauteurConnexion + 0.1 ])
			{
				cube([ largeurBoite - epaisseurParoi, longueurBoite - epaisseurParoi, hauteurConnexion ]);
			}

            // Pressions
            //
			translate([
				largeurBoite / 4 + epaisseurParoi / 2, epaisseurParoi / 2 - 0.2, hauteurBoite - hauteurConnexion / 2
			])
			{
				sphere(1);
			}

			translate([
				largeurBoite / 4 * 3 + epaisseurParoi / 2, epaisseurParoi / 2 - 0.2, hauteurBoite - hauteurConnexion / 2
			])
			{
				sphere(1);
			}

			translate([
				largeurBoite / 4 + epaisseurParoi / 2, longueurBoite - epaisseurParoi / 2 + 0.2,
				hauteurBoite - hauteurConnexion / 2
			])
			{
				sphere(1);
			}

			translate([
				largeurBoite / 4 * 3 + epaisseurParoi / 2, longueurBoite - epaisseurParoi / 2 + 0.2,
				hauteurBoite - hauteurConnexion / 2
			])
			{
				sphere(1);
			}

			translate([ epaisseurParoi / 2 - 0.2, longueurBoite / 2 + 0.2, hauteurBoite - hauteurConnexion / 2 ])
			{
				sphere(1);
			}

			translate([
				largeurBoite - epaisseurParoi / 2 + 0.2, longueurBoite / 2 + 0.2, hauteurBoite - hauteurConnexion / 2
			])
			{
				sphere(1);
			}

		}
        // Supports
        //
		translate([ epaisseurParoi, epaisseurParoi, epaisseurFond ])
		{
			cube([ 7, 7, 6 ]);
		}
		translate([ largeurBoite - epaisseurParoi - 7, epaisseurParoi, epaisseurFond ])
		{
			cube([ 7, 7, 6 ]);
		}
		translate([ epaisseurParoi, longueurBoite - epaisseurParoi - 7, epaisseurFond ])
		{
			cube([ 7, 7, 6 ]);
		}
        /*
		translate([ largeurBoite - epaisseurParoi - 7, longueurBoite - epaisseurParoi - 7, epaisseurFond ])
		{
			cube([ 7, 7, 6 ]);
		}
        */
	}
}

module topWaveshare()
{
	union()
	{
		difference()
		{
			roundedBox(longueurBoite, largeurBoite, hauteurCouvercle, rayonCoin);

            // Ouvrir le couvercle
            //
			translate([ epaisseurParoi, epaisseurParoi, epaisseurFond ])
			{
				cube([ largeurBoite - 2 * epaisseurParoi, longueurBoite - 2 * epaisseurParoi, hauteurCouvercle ]);
			}

            // Connexion
            //
			translate([ -0.1, 0, hauteurCouvercle - hauteurConnexion ])
			{
				cube([ epaisseurParoi/2+0.1, longueurBoite , hauteurConnexion+0.1 ]);
			}

			translate([ 0, -0.1, hauteurCouvercle - hauteurConnexion ])
			{
				cube([ largeurBoite, epaisseurParoi/2+0.1 , hauteurConnexion+0.1 ]);
			}

			translate([ largeurBoite-epaisseurParoi/2, 0, hauteurCouvercle - hauteurConnexion ])
			{
				cube([ epaisseurParoi/2+0.1, longueurBoite , hauteurConnexion+0.1 ]);
			}

			translate([ 0, longueurBoite-epaisseurParoi/2, hauteurCouvercle - hauteurConnexion ])
			{
				cube([ largeurBoite, epaisseurParoi/2+0.1 , hauteurConnexion+0.1 ]);
			}

            // Ecran
            //
   			translate([ epaisseurParoi+9, epaisseurParoi+5, -0.1 ])
			{
				cube([ 56, 97, epaisseurFond+0.2 ]);
			}

            // Carte SD
            //
			translate([ largeurBoite-epaisseurParoi-0.1, 18, hauteurCouvercle - 5 ])
			{
				cube([ epaisseurParoi + 0.2, 18, 5.1 ]);
			}

            // Ports USB
            //
			translate([ largeurBoite-50, longueurBoite - epaisseurParoi - 0.1, hauteurCouvercle - 5 ])
			{
				cube([ 40, epaisseurParoi + 0.2, 5.1 ]);
			}

		}

        // Pressions
        //
		translate([
			largeurBoite / 4 + epaisseurParoi / 2, epaisseurParoi / 2 + 0.2, hauteurCouvercle - hauteurConnexion / 2
		])
		{
			sphere(1);
		}

		translate([
			largeurBoite / 4 * 3 + epaisseurParoi / 2, epaisseurParoi / 2 + 0.2, hauteurCouvercle - hauteurConnexion / 2
		])
		{
			sphere(1);
		}

		translate([
			largeurBoite / 4 + epaisseurParoi / 2, longueurBoite - epaisseurParoi / 2 - 0.2,
			hauteurCouvercle - hauteurConnexion / 2
		])
		{
			sphere(1);
		}

        /*
		translate([
			largeurBoite / 4 * 3 + epaisseurParoi / 2, longueurBoite - epaisseurParoi / 2 - 0.2,
			hauteurCouvercle - hauteurConnexion / 2
		])
		{
			sphere(1);
		}
        */

		translate([ epaisseurParoi / 2 + 0.2, longueurBoite / 2, hauteurCouvercle - hauteurConnexion / 2 ])
		{
			sphere(1);
		}

		translate([
			largeurBoite - epaisseurParoi / 2 - 0.2, longueurBoite / 2, hauteurCouvercle - hauteurConnexion / 2
		])
		{
			sphere(1);
		}
	}
}
//translate([largeurCircuit+epaisseurParoi*2, 0,hauteurBoite*2-hauteurConnexion])
//    rotate([180,0,180])
        topWaveshare();
//bottomWaveshare();
