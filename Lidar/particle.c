#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> //add '-lm' parameter when compile with gcc
#include <errno.h>
#include <unistd.h>
#include "lidar.cpp"

#define MAPROW 172
#define MAPCOL 116

#define NOP 100 //number of particle
#define NOS 8 //number of sense
int map[MAPROW][MAPCOL];

typedef struct _Particle {
	int x;
	int y;
	float angle;
	float weight;
} Particle;

int map_init()
{
	char line[MAPCOL+5]; //if not enough, it can cause something what you don't want
	// printf("char\n");
	FILE * fp = fopen("final_map.txt", "r");
	// printf("file\n");s
	int i=0, j=0;
	if(fp == NULL)
	{
		printf("errno %d\n", errno);
		perror("error: ");
		return -1;
	}
	for(i=0; i<3; i++)
	{
		fgets(line, sizeof(line), fp);
	}
	for(i=0; i<MAPROW; i++)
	{
		if(feof(fp))
			return -1;
		fgets(line, sizeof(line), fp);
		// printf("fscanf\n");
		// printf("%s",line);
		for(j=0; j<MAPCOL; j++)
		{
			 map[i][j] = (int)(line[j]-'0');
			//printf("%d",line[j]);
		}
		// printf("\n");
		
	}
	// printf("printf\n");
	fclose(fp);
	return 0;
}

double deg(float r)
{
	return (360.0*r)/((2.0)*M_PI);
}

double rad(float d)
{
	return (2.0*M_PI)*d/360.0;
}

void particle_init(Particle * particles, int particle_count, int pos_angle)
{
	int i=0;
	srand(time(NULL));
	for(i=0; i<particle_count; i++)
	{
		particles[i].x = rand()%42;
		particles[i].y = rand()%42;
		particles[i].angle = rand()/(float)RAND_MAX*180.0f-90+pos_angle;
		particles[i].weight=0.0;
	}
}

void append_particle(Particle * target, Particle * ori)
{
	target->x = ori->x;
	target->y = ori->y;
	target->angle = ori->angle;
	target->weight = ori->weight;
}

float particle_sense(Particle * particle, float angle, int pos_x, int pos_y)//eval dist
{
	double x = particle->x;
	double y = particle->y;
	angle = angle + particle->angle;
	int dist=5;
	int limit=1000;

	// printf("ori: %d %d %f %d , ", (pos_x-3)+(int)((x/6)), (pos_y-3)+((int)(y/6)), particle->angle,map[(pos_x-3)+(int)((x/6))][(pos_y-3)+((int)(y/6))]);
	if(map[(pos_x-3)+(int)((x/6))][(pos_y-3)+((int)(y/6))]!=0)
	{
		// printf("particle_sense:firstif\n");
		return -100;
	}
	while(dist<limit)
	{
		x = x+(-cos(rad(angle)));
		y = y+sin(rad(angle));

		// printf("x:%f y:%f dist:%f",x,y,dist);

		if(map[(pos_x-3)+((int)(x/6))][(pos_y-3)+((int)(y/6))]!=0)
			break;
		dist+=5;
	}
	if(dist>=limit)
		dist=0;
	return dist;
}

float angle_calc(Particle * particles, int particle_count)
{
	double result;
	// float theta_sum;
	double x=0.0, y=0.0;
	int i=0, j=0;
	for(i=0; i<particle_count; i++)
	{
		x += cos(rad(particles[i].angle));
		y += sin(rad(particles[i].angle));
	}
	// float theta1 = rad(10.0);
	// float theta2 = rad(190.0);
	// //float theta3 = rad(135.0);
	// float x = cos(theta1)+cos(theta2);//+cos(theta3);
	// float y = sin(theta1)+sin(theta2);//+sin(theta3);
	// printf("t1:%e, t2:%e, x:%f, y:%f, %f, %f, %f, %f\n", theta1, theta2, x, y, cos(theta1), cos(theta2), sin(theta1), sin(theta2));
	
	if(x == 0)
	{
		if(y==0)
			result=deg(particles[0].angle);
		else if (y>0)
			result = 0;
		else
			result = 180;
	}
	else
		result = deg(atan2(y,x));
	// printf("%f\n", deg(atan2(y,x)));	
	return (float)result;
}

double calcGaussianProbability(double mu, double sigma, double x) //p_dist, sensor noise, c_dist
{
	// printf("%e %e , ", exp(-((mu-x)*(mu-x))/(sigma*sigma)/2.0),  sqrt(2.0*M_PI*(sigma*sigma)));
	return exp(-((mu-x)*(mu-x))/(sigma*sigma)/2.0) / sqrt(2.0*M_PI*(sigma*sigma));
	// return exp(- pow((mu - x), 2) / pow(sigma, 2) / 2.0) / sqrt(2.0 * pi * pow(sigma, 2));// it need experiment. what is fast?
}

double getProbability(float* dists, float* angles, Particle * particle, float sensor_noise, int pos_x, int pos_y)
{
	int i=0, j=0;
	double prob=1;
	float dist;
	for(i = 0, j=0; i<NOS; i++)
	{
		while(angles[i*(360/NOS)+j]==0&&j<NOS)
			j++;
		dist = particle_sense(particle, angles[i*(360/NOS)+j],pos_x, pos_y);
		// printf("%d %d %f %f %f %d , ", particle->x, particle->y, angles[i*(360/NOS)+j], dists[i*(360/NOS)+j], dist, i*(360/NOS)+j);
		if(dist>=0)
		{
			prob *= calcGaussianProbability(dist, sensor_noise, dists[i*(360/NOS)+j]);
			// printf("prob: %e\n", calcGaussianProbability(dist, sensor_noise, dists[i*(360/NOS)+j]));
		}
		else{
			prob=0; break;	}
	}
	return prob;
}

void predict(Particle * particles, int particle_count, int pos_x, int pos_y ,int * result_x, int * result_y, float * result_angle)
{
	float x=0, y=0;
	int i=0;

	for(i=0; i<particle_count; i++)
	{
		x += particles[i].x;
		y += particles[i].y;
		//printf("%f %f\n",x,y);
	}
	*result_x = (pos_x-3)+((int)(x/particle_count+0.5))/6;
	*result_y = (pos_y-3)+((int)(y/particle_count+0.5))/6;
	*result_angle = angle_calc(particles, particle_count);
}

int main(int argc, char* argv[])
{
	int i=0, j=0, t=0, index, max_index = 0;
	int pos_x=25, pos_y=95;
	float pos_angle = 180.0f;
	int particle_count = NOP;
	// float forward_noise = 1.0;
	// float turn_noise = 0.05;
	float sensor_noise = 100.0;
	// float angle_result;
	double beta, max_weight;

	float dists[360];
	float angles[360];
	void* drv;
	Particle particles[particle_count];
	Particle rsParticles[particle_count];
	double weights[particle_count];

	// FILE * rfile = fopen("sensor1.txt","r");
	pos_x=atoi(argv[1]); pos_y=atoi(argv[2]); pos_angle=(float)atoi(argv[3]);

	printf("ready\n");
	if(map_init()<0)
	{
		printf("failed map initial\n");
		return 0;
	}
	
	printf("map init\n");
	// for(i=0; i<MAPROW; i++)
	// {
	// 	for(j=0; j<MAPCOL; j++)
	// 		printf("%d",map[i][j]);
	// 	printf("\n");
	// }
	drv = createDriver_c();
	if(!drv)
	{
		printf("Error, cannot Connect\n");
		return 0;
	}
	startMotor_c(drv);

	while(t>-1)
	{
	particle_init(particles, particle_count, pos_angle);
	// angle_result = angle_calc(particles, particle_count);

	grabScanData_c(drv, 1, dists, angles);
	// if(rfile==NULL)
	// 	{	printf("not such sensor file\n"); return 0;	}
	// for(i=0; i<360; i++)
	// {
	// 	fscanf(rfile, "%f %f", &angles[i], &dists[i]);
	// }
	//printf("grabed\n");	

	weights[0] = getProbability(dists, angles, &particles[0], sensor_noise, pos_x, pos_y);
	max_weight = weights[0];
	for(i=1; i<particle_count; i++)
	{
		// printf("------%d-------\n",i);
		weights[i] = getProbability(dists, angles, &particles[i], sensor_noise,  pos_x, pos_y);
		if(weights[i]>max_weight)
		{
			max_weight = weights[i];
			max_index = i;
		}
	}
	// printf("max index: %d\n", max_index);
	// for(i=0; i<particle_count; i++)
	// {
	// 	// printf("%f %f\n", angles[i], dists[i]);
	// 	printf("%d: %d %d %f %e\n", i, particles[i].x, particles[i].y, particles[i].angle, weights[i]);
	// }

	srand(time(NULL));
	index = rand()%particle_count;
	beta = 0.0;

	for(i=0; i<particle_count; i++)
	{
		beta += (rand()/(float)RAND_MAX)*2.0f*max_weight;

		while(beta>weights[index])
		{
			beta -= weights[index];
			index = (index+1)%particle_count;
		}
		// append_particle(&rsParticles[i], &particles[index]);
		rsParticles[i].x = particles[index].x;
		rsParticles[i].y = particles[index].y;
		rsParticles[i].angle = particles[index].angle;
		rsParticles[i].weight = particles[index].weight;
		// printf("%d, %d, %d, %d\n", i, index, particles[i].x, particles[index].y);
	}

	for(i=0; i<particle_count; i++)
	{
		// printf("%d\t%d\t%f\t%f\n", particles[i].x, particles[i].y, weights[i], max_weight);
	}
	// printf("\n");
	// for(i=0; i<particle_count; i++)
	// {
	// 	printf("%d\t%d\t%f\n", rsParticles[i].x, rsParticles[i].y, weights[i]);
	// }

	predict(rsParticles, particle_count, pos_x, pos_y , &pos_x, &pos_y, &pos_angle);
	//printf("x:%d y:%d a:%f %f\n", pos_x, pos_y, pos_angle, particles[max_index].angle);
	t++;
	//sleep(1);
	}

	stopMotor_c(drv);
	disposeDriver_c(drv);
	// fclose(rfile);

	return 0;
}
// ./particle.c << final_map.txt