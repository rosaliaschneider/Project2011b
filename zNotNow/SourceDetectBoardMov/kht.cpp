/*
 * Copyright (C) 2008 Leandro A. F. Fernandes and Manuel M. Oliveira
 *
 * author   : Fernandes, Leandro A. F.
 * e-mail   : laffernandes@gmail.com
 * home page: http://www.inf.ufrgs.br/~laffernandes
 *
 *
 * The complete description of the implemented techinique can be found at
 *
 *      Leandro A. F. Fernandes, Manuel M. Oliveira
 *      Real-time line detection through an improved Hough transform voting scheme
 *      Pattern Recognition (PR), Elsevier, 41:1, 2008, 299-314.
 *      DOI: http://dx.doi.org/10.1016/j.patcog.2007.04.003
 *      Project Page: http://www.inf.ufrgs.br/~laffernandes/kht.html
 *
 * If you use this implementation, please reference the above paper.
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 */

#include <algorithm>
#include <RX/vec2.h>
#include "kht.h"
#include "linking.h"
#include "subdivision.h"
#include "voting.h"
#include "peak_detection.h"

using namespace std;

// Kernel-based Hough transform (KHT) for detecting straight lines in images.
void kht(lines_list_t &lines, unsigned char *binary_image, const size_t image_width, const size_t image_height, const size_t cluster_min_size, const double cluster_min_deviation, const double delta, const double kernel_min_height, const double n_sigmas)
{
	static strings_list_t strings;
	static clusters_list_t clusters;
	static accumulator_t accumulator;

	// Group feature pixels from an input binary into clusters of approximately collinear pixels.
	find_strings( strings, binary_image, image_width, image_height, cluster_min_size );
	find_clusters( clusters, strings, cluster_min_deviation, cluster_min_size );

	// Perform the proposed Hough transform voting scheme.
	accumulator.init( image_width, image_height, delta );
	voting( accumulator, clusters, kernel_min_height, n_sigmas );

	// Retrieve the most significant straight lines from the resulting voting map.
	peak_detection( lines, accumulator );
}

bool biggerRho(line_t l1, line_t l2)
{
	return l1.rho < l2.rho;
}

line_t fromPoints(RX::vec2 p1, RX::vec2 p2)
{
	if(p1.x < p2.x) swap(p1, p2);

	double a, b, c;
	a = p1.y-p2.y;
	b = p2.x-p1.x;
	c = p1.x*p2.y - p1.y*p2.x;

	line_t l;

	l.rho = abs(c) / sqrt(a*a + b*b);
	double y = p2.y-p1.y;
	double x = p2.x-p1.x;
	l.theta = atan2(x, -y);
	if(l.theta < 0) {
		l.theta = 3.141592 + l.theta;
		l.rho = -l.rho;
	}

	return l;
}

line_t mostSimilarLine(lines_list_t &lines, RX::vec2 p1, RX::vec2 p2)
{
	line_t mine, similar;
	mine = fromPoints(p1, p2);

	double difference = 9999999.0;
	for(int i = 0; i < std::min(200,(int)(lines.size())); ++i) {
		line_t line = lines[i];
		line.theta = (line.theta * 3.141592)/180.0;
		
		if(abs(line.rho - mine.rho)+200*abs(line.theta - mine.theta) < difference) {
			similar = line;
			difference = abs(similar.rho - mine.rho)+200*abs(similar.theta - mine.theta);
		}
	}
	
	return similar;
}

void convertLinesToGlobal(lines_list_t &lines, int width, int height, int bx, int by, int bw, int bh)
{
	RX::vec2 p1, p2;
	for(int i = 0; i < lines.size(); ++i)
	{
		line_t &line = lines[i];
		if(line.theta != 0.0) 
		{
			double theta = line.theta * (3.141592 / 180.0);
			double one_div_sin_theta = 1.0 / sin( theta );
			double cos_theta = cos( theta );
			double aux = bw / 2;
				
			double y1 = (line.rho + aux * cos_theta) * one_div_sin_theta + by + bh/2;
			double y2 = (line.rho - aux * cos_theta) * one_div_sin_theta + by + bh/2;

			p1 = RX::vec2(bx,  y1);
			p2 = RX::vec2(bx+bw, y2);
		}
		else 
		{
			double aux = bh / 2;
			p1 = RX::vec2(line.rho + bx + bw/2, -aux);
			p2 = RX::vec2(line.rho + bx + bw/2, aux);
		}
		line = fromPoints(p2, p1);
	}
}