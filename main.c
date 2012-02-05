/*
 * main.c
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"

const double gravity = 6.67300E-11;
const double timestep = 0.1;
int numberofobjects;


Vector addVectors (Vector vector1, Vector vector2) {
    Vector result;
    result.x = vector1.x + vector2.x;
    result.y = vector1.y + vector2.y;
    result.z = vector1.z + vector2.z;
    return result;
}

Vector multiplyVector(Vector vector, double constant) {
	Vector result;
	result.x = vector.x * constant;
	result.y = vector.y * constant;
	result.z = vector.z * constant;
	return result;
}

Vector calculateForce( DataItem* object1, DataItem* object2) {

    Vector force;

    double xdistance = object1->position.x - object2->position.x;
    double ydistance = object1->position.y - object2->position.y;
    double zdistance = object1->position.z - object2->position.z;

    double rsquared = xdistance*xdistance + ydistance*ydistance + zdistance*zdistance;

    double forceacting = -(gravity*object1->mass*object2->mass)/rsquared;

    Vector unitvector;
    unitvector.x = xdistance/sqrt(rsquared);
    unitvector.y = ydistance/sqrt(rsquared);
    unitvector.z = zdistance/sqrt(rsquared);

    force = multiplyVector(unitvector, forceacting);

    return force;
}

Vector acceleration(DataItem* object) {
    Vector accel;
    Vector force = object->force;
    accel.x = force.x/object->mass;
    accel.y = force.y/object->mass;
    accel.z = force.z/object->mass;
    return accel;
}

void updateVelocity (DataItem* object) {

	Vector acceleration;
    Vector force = object->force;
    acceleration.x = force.x/object->mass;
    acceleration.y = force.y/object->mass;
    acceleration.z = force.z/object->mass;

    object->velocity.x += acceleration.x*timestep;
    object->velocity.y += acceleration.y*timestep;
    object->velocity.z += acceleration.z*timestep;
}

//Wants to happen before you update velocity
void updatePosition (DataItem* object) {

	//Ignoring the timestep^2 term as it is v. small

	Vector acceleration;
    Vector force = object->force;

    acceleration.x = force.x/object->mass;
    acceleration.y = force.y/object->mass;
    acceleration.z = force.z/object->mass;

    object->position.x += object->velocity.x*timestep;
    object->position.y += object->velocity.y*timestep;
    object->position.z += object->velocity.z*timestep;
}

void progress_timestep(DataItem* dataItems) {

    int i;
    int j;

    for (i=0; i<numberofobjects; i++) {

    	DataItem* object1 = &dataItems[i];

    	object1->force.x = 0;
        object1->force.y = 0;
        object1->force.z = 0;

        for (j=0; j<numberofobjects; j++) {
            if (i==j) {
                continue;
            }
            else {
                DataItem* object2 = &dataItems[j];
                object1->force = addVectors(object1->force, calculateForce(object1, object2));
            }
        }
    }

 //use acceleration to calculate new velocity
    for (i=0; i<numberofobjects; i++) {
        DataItem* object = &dataItems[i];
        updateVelocity (object);
    }

 //use velocity to calculate new positions
    for (i=0; i<numberofobjects; i++) {
        DataItem* object = &dataItems[i];
        updatePosition (object);
    }
}

void setupfile (char* filenamein, DataItem** dataItemsPtr) {

	FILE *filepointer;

	filepointer = fopen(filenamein, "r");

	fscanf(filepointer, "%i", &numberofobjects);

	DataItem* dataItems;
	dataItems = (DataItem*) malloc(numberofobjects*sizeof(DataItem));
	*dataItemsPtr = dataItems;

	int i;
	for (i=0; i<numberofobjects; i++) {

		DataItem object;

		fscanf (filepointer, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf",
				&object.position.x, &object.position.y, &object.position.z,
				&object.velocity.x, &object.velocity.y, &object.velocity.z,
				&object.force.x, &object.force.y, &object.force.z,
				&object.mass);

		dataItems[i] = object;
	}

	fclose(filepointer);
}

double drand (double low, double high )
{
    return ( (double)rand() * ( high - low ) ) / (double)RAND_MAX + low;
}


void setuprandom(int number, DataItem** dataItemsPtr) {

	DataItem* dataItems;
	dataItems = (DataItem*) malloc(numberofobjects*sizeof(DataItem));
	*dataItemsPtr = dataItems;

	int i;
	for (i=0; i<numberofobjects; i++) {

		DataItem object;
		object.position.x = drand(-1,1);
		object.position.y = drand(-1,1);
		object.position.z = drand(-1,1);
		object.velocity.x = drand(-1,1);
		object.velocity.y = drand(-1,1);
		object.velocity.z = drand(-1,1);
		object.mass = drand(0,2);

		dataItems[i] = object;
	}
}


void output (char* filenameOut, DataItem* dataItems) {

	FILE *filepointer;

	filepointer = fopen(filenameOut, "w");

	int i;
	for (i=0; i<numberofobjects; i++) {

		DataItem object = dataItems[i];

		fprintf (filepointer, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",
				object.position.x, object.position.y, object.position.z,
				object.velocity.x, object.velocity.y, object.velocity.z,
				object.force.x, object.force.y, object.force.z,
				object.mass);
	}

	fclose(filepointer);
}

int main(int argc, char** argv) {

	DataItem* dataItems;

	if (argc<1) {
		fprintf(stderr, "Please supply a mode of operation\n");
		exit(-1);
	}

	int mode = atoi(argv[1]);

	int numberoftimesteps;
	char* filenameout;

	switch (mode) {
	case 1:
		if (argc<5) {
			fprintf(stderr, "Please supply the number of objects to be created and the number of steps for this mode of operation\n");
			exit(-1);
		}
		numberofobjects = atoi(argv[2]);
		filenameout = argv[3];
		numberoftimesteps = atoi(argv[4]);
		setuprandom(numberofobjects, &dataItems);
		break;

	case 2:
		if (argc<5) {
			fprintf(stderr, "Please supply the number of steps and an input and output files for this mode of operation\n");
			exit(-1);
		}
		char* filenamein = argv[2];
		filenameout = argv[3];
		numberoftimesteps = atoi(argv[4]);
	    setupfile(filenamein, &dataItems);
	    break;

	default:
		exit(-1);
	}

    int i;
    for (i=0; i<numberoftimesteps; i++) {
        progress_timestep(dataItems);
    }

    output(filenameout, dataItems);

	free(dataItems);

    return 0;
}
