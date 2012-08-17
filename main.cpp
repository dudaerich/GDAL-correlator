/**
 * @file
 * @author Andrew Migal migal.drew@gmail.com
 * @brief Main demonstration function
 *
 * This program is free software and
 * is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY
 */

#include "gdal.h"
#include "gdal_priv.h"
#include "cpl_conv.h"
#include "cpl_vsi.h"

#include "GDALSimpleSURF.h"
#include "GDALFeaturePoint.h"
#include "GDALFeaturePointsCollection.h"
#include "GDALMatchedPointsCollection.h"
#include "GDALCorrelator.h"

#include <fstream>

using namespace std;

/**
 * Main function, which invokes correlator methods
 */
int main(int argc, char* argv[])
{
    GDALAllRegister();

    GDALDataset  *poDataset_1;
    GDALDataset  *poDataset_2;

    poDataset_1 = (GDALDataset *) GDALOpen(
    		"/home/andrew/workspace/GDAL-correlator/Debug/1.jpg", GA_ReadOnly );
	if( poDataset_1 == NULL )
       return 0;

	poDataset_2 = (GDALDataset *) GDALOpen(
			"/home/andrew/workspace/GDAL-correlator/Debug/2.jpg", GA_ReadOnly );
	if( poDataset_2 == NULL )
		return 0;

	GDALFeaturePointsCollection *poFPCollection_1 =
			new GDALFeaturePointsCollection(poDataset_1);
	GDALFeaturePointsCollection *poFPCollection_2 =
			new GDALFeaturePointsCollection(poDataset_2);

	GDALMatchedPointsCollection *poMatched = new GDALMatchedPointsCollection();

	int nOctStart = 2;
	int nOctEnd = 2;
	double dfSURFTreshold = 0.001;
	double dfMatchingThreshold = 0.015;

	int* panBands = new int[3];
	for (int i = 0; i < 3; i++)
		panBands[i] = i + 1;

	// Find feature points on both images
	GatherFeaturePoints(poDataset_1, panBands,
			poFPCollection_1, nOctStart, nOctEnd, dfSURFTreshold);
	GatherFeaturePoints(poDataset_2, panBands,
			poFPCollection_2, nOctStart, nOctEnd, dfSURFTreshold);
	// Use gathered points to find correspondences
	MatchFeaturePoints(poMatched,
			poFPCollection_1, poFPCollection_2, dfMatchingThreshold);

/* -------------------------------------------------------------------- */
/*      Printing parameters for demonstration                           */
/* -------------------------------------------------------------------- */
	ofstream out;

	out.open("points_1.txt");
	for (int i = 0; i < poFPCollection_1->GetSize(); i++)
		out << poFPCollection_1->GetPoint(i)->GetX() << " "
			<< poFPCollection_1->GetPoint(i)->GetY() << endl;
	out.close();

	out.open("points_2.txt");
	for (int i = 0; i < poFPCollection_2->GetSize(); i++)
		out << poFPCollection_2->GetPoint(i)->GetX() << " "
			<< poFPCollection_2->GetPoint(i)->GetY() << endl;

	out.close();

	GDALFeaturePoint *point_1 = new GDALFeaturePoint();
	GDALFeaturePoint *point_2 = new GDALFeaturePoint();

	out.open("matched_points.txt");
	for (int i = 0; i < poMatched->GetSize(); i++)
	{
		poMatched->GetPoints(i, point_1, point_2);

		out << point_1->GetX() << ", " << point_1->GetY();
		out << " | ";
		out << point_2->GetX() << ", " << point_2->GetY();
		out << endl;
	}
	out.close();


	GDALFeaturePoint *p = poFPCollection_1->GetPoint(0);
	if (p != NULL)
	{
		for (int i = 0; i <= 64; i++)
			printf("%d: %f \n", i + 1, (*p)[i]);
	}


	delete poDataset_1;
	delete poDataset_2;
	delete poFPCollection_1;
	delete poFPCollection_2;
	delete poMatched;
	delete point_1;
	delete point_2;

	delete[] panBands;

	printf("Everything looks good...\n");

	return 0;
}
