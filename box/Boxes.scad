module roundedBox(length, width, height, radius)
{
	dRadius = 2 * radius;

	translate([ radius, radius, 0 ])
	{
		minkowski()
		{
			cube(size = [ width - dRadius, length - dRadius, height ]);
			cylinder(r = radius, h = 0.01);
		}
	}
}

module fullBox(length, width, height, radius, thickness, bottomThickness)
{
	difference()
	{
		roundedBox(length + thickness * 2, width + thickness * 2, height, radius);
		translate([ thickness, thickness, bottomThickness ])
		{
			roundedBox(length, width, height, radius);
		}
	}
}

module emptyBox(length, width, height, radius, thickness)
{
	difference()
	{
		roundedBox(length + thickness * 2, width + thickness * 2, height, radius);
		translate([ thickness, thickness, -1 ])
		{
			roundedBox(length, width, height + 2, radius);
		}
	}
}

module bottomBox(length, width, height1, height2, radius, thickness, bottomThickness)
{
	fullBox(length, width, height1, radius, thickness, bottomThickness);

	translate([ thickness / 2, thickness / 2, height1 ])
	{
		difference()
		{
			emptyBox(length, width, height2, radius, thickness / 2);

			translate([ width / 4 + thickness / 2, -0.2, height2 / 2 ])
			{
				sphere(1);
			}

			translate([ width / 4 * 3 + thickness / 2, -0.2, height2 / 2 ])
			{
				sphere(1);
			}

			translate([ width / 4 + thickness / 2, length + thickness + 0.2, height2 / 2 ])
			{
				sphere(1);
			}

			translate([ width / 4 * 3 + thickness / 2, length + thickness + 0.2, height2 / 2 ])
			{
				sphere(1);
			}
		}
	}
}
/**
 * Box with bottom
 *
 * @param length The box's length
 * @param width  The box's width
 */
module topBox(length, width, height1, height2, radius, thickness, bottomThickness)
{
	fullBox(length, width, height1, radius, thickness, bottomThickness);

	translate([ 0, 0, height1 ])
	{
		union()
		{
			emptyBox(length + thickness, width + thickness, height2, radius, thickness / 2);

			translate([ width / 4 + thickness / 2, thickness / 2 - 0.2, height2 / 2 ])
			{
				sphere(1);
			}

			translate([ width / 4 * 3 + thickness / 2, thickness / 2 - 0.2, height2 / 2 ])
			{
				sphere(1);
			}

			translate([ width / 4 + thickness / 2, length + thickness / 2 * 3 + 0.2, height2 / 2 ])
			{
				sphere(1);
			}

			translate([ width / 4 * 3 + thickness / 2, length + thickness / 2 * 3 + 0.2, height2 / 2 ])
			{
				sphere(1);
			}
		}
	}
}
