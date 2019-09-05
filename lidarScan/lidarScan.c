#include <stdio.h>
#include "lidar.cpp"

int main()
{
	float dist[360];
	float angle[360];
	int i=0;

	void* drv = createDriver_c();
	printf("create done\n");
	startMotor_c(drv);
	printf("motorstart\n");
	grabScanData_c(drv, 1, dist, angle);
	printf("grabdone\n");
	for(; i<360; i++)
	{
		printf("%f %f\n", angle[i], dist[i]);
	}
	stopMotor_c(drv);
	disposeDriver_c(drv);
	return 0;
}
