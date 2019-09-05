#ifdef __cplusplus

#include "rplidar.h"
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <limits>

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#define DEG2RAD(x) ((x)*M_PI/180.)

using namespace rp::standalone::rplidar;

#endif

#ifdef __cplusplus
extern "C"
{
#endif

	void* createDriver_c();
	void startMotor_c(void* instance);
	void stopMotor_c(void* instance);
	void grabScanData_c(void* instance, int angle_compensate_multiple, float dist[], float angle[]);
	void disposeDriver_c(void* instance);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

bool checkRPLIDARHealth(RPlidarDriver * drv)
{
    u_result     op_result;
    rplidar_response_device_health_t healthinfo;


    op_result = drv->getHealth(healthinfo);
    if (IS_OK(op_result)) 
    { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
        printf("RPLidar health status : %d\n", healthinfo.status);
        if (healthinfo.status == RPLIDAR_STATUS_ERROR) 
        {
            fprintf(stderr, "Error, rplidar internal error detected. Please reboot the device to retry.\n");
            // enable the following code if you want rplidar to be reboot by software
            // drv->reset();
            return false;
        } 
        else 
        {
            return true;
        }

    }
    else {
        fprintf(stderr, "Error, cannot retrieve the lidar health code: %x\n", op_result);
        return false;
    }
}

bool getRPLIDARDeviceInfo(RPlidarDriver * drv)
{
    u_result     op_result;
    rplidar_response_device_info_t devinfo;

    op_result = drv->getDeviceInfo(devinfo);
    if (IS_FAIL(op_result)) 
    {
        if (op_result == RESULT_OPERATION_TIMEOUT) 
            printf("Error, operation time out. RESULT_OPERATION_TIMEOUT! ");
        else         
            printf("Error, unexpected error, code: %x",op_result);
        return false;
    }

    // print out the device serial number, firmware and hardware version number..
    printf("RPLIDAR S/N: ");
    for (int pos = 0; pos < 16 ;++pos)
        printf("%02X", devinfo.serialnum[pos]);
    printf("\n");
    printf("Firmware Ver: %d.%02d",devinfo.firmware_version>>8, devinfo.firmware_version & 0xFF);
    printf("Hardware Rev: %d",(int)devinfo.hardware_version);
    return true;
}

static float getAngle(const rplidar_response_measurement_node_hq_t& node)
{
    return node.angle_z_q14 * 90.f / 16384.f;
}

bool start_motor(RPlidarDriver* drv)
{
  if(!drv)
       return false;
  printf("Start motor");
  drv->startMotor();
  drv->startScan(0,1);
  return true;
}

bool stop_motor(RPlidarDriver* drv)
{
  if(!drv)
       return false;

  printf("Stop motor");
  drv->stop();
  drv->stopMotor();
  return true;
}

RPlidarDriver* createDriver()
{
	const char * opt_com_path = NULL;
	_u32 		 opt_com_baudrate = 115200;
	u_result	 op_result; 
	rplidar_response_device_info_t devinfo;
    bool connectSuccess = false;
	#ifdef _WIN32
        // use default com port
        opt_com_path = "\\\\.\\com3";
	#else
        opt_com_path = "/dev/ttyUSB0";
	#endif
	RPlidarDriver * drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
    if (!drv) {
        fprintf(stderr, "insufficent memory, exit\n");
        exit(-2);
    }
    if(!drv)
        drv = RPlidarDriver::CreateDriver(DRIVER_TYPE_SERIALPORT);
    if (IS_OK(drv->connect(opt_com_path, opt_com_baudrate)))
    {
        op_result = drv->getDeviceInfo(devinfo);

        if (IS_OK(op_result)) 
        {
            // connectSuccess = true;
        }
        else
        {
        	fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n", opt_com_path);
            delete drv;
            drv = NULL;
        }
    }
    return drv;
}

void disposeDriver(RPlidarDriver * drv)
{
	drv->stop();
    drv->stopMotor();
    RPlidarDriver::DisposeDriver(drv);
    drv = NULL;
}

rplidar_response_measurement_node_hq_t * grab_ScanData(rplidar_response_measurement_node_hq_t angle_compensate_nodes[], RPlidarDriver * drv, int angle_compensate_multiple, int angle_compensate_nodes_count)
{
	rplidar_response_measurement_node_hq_t nodes[360*8];
	size_t   count = _countof(nodes);
	u_result     op_result;
	int angle_compensate_offset = 0;
	

	op_result = drv->grabScanDataHq(nodes, count);
	if (op_result == RESULT_OK) 
	{
		op_result = drv->ascendScanData(nodes, count);
    	// float angle_min = DEG2RAD(0.0f);
    	// float angle_max = DEG2RAD(359.0f);
		if (op_result == RESULT_OK)
		{

			memset(angle_compensate_nodes, 0, angle_compensate_nodes_count*sizeof(rplidar_response_measurement_node_hq_t));

			int i = 0, j = 0;
			for( ; i < (int)count; i++ ) 
			{
                        // if (nodes[i].dist_mm_q2 != 0) {
				float angle = getAngle(nodes[i]);
				int angle_value = (int)(angle * angle_compensate_multiple);
				if ((angle_value - angle_compensate_offset) < 0) angle_compensate_offset = angle_value;
				for (j = 0; j < angle_compensate_multiple; j++) 
				{
    				angle_compensate_nodes[angle_value-angle_compensate_offset+j] = nodes[i];
    				// printf("%d %f %d %d %d %d %d\n",count, angle, angle_value, angle_compensate_offset, 
    				// 	angle_value-angle_compensate_offset+j, i, h);
    			}  		
                        // }                        
    		}
    	}
    }
    //int i=0;
    //for(; i<360; i++)
    //{
    //	printf("%d %d\n", angle_compensate_nodes[i].dist_mm_q2, resultNodes[i].dist_mm_q2);
    //}
    
    return angle_compensate_nodes;

}


extern "C"
{
	void* createDriver_c()
	{
		printf("creat\n");
		RPlidarDriver * drv = createDriver();
		printf("created\n");
		return (void*)drv;
	}
	void startMotor_c(void* instance)
	{
		RPlidarDriver* drv = (RPlidarDriver*)instance;
		start_motor(drv);
	}
	void stopMotor_c(void* instance)
	{
		RPlidarDriver* drv = (RPlidarDriver*)instance;
		stop_motor(drv);
	}
	void grabScanData_c(void* instance, int angle_compensate_multiple, float dist[],float angle[])
	{
		int i=0;
		const int angle_compensate_nodes_count = 360*angle_compensate_multiple;
		RPlidarDriver* drv = (RPlidarDriver*)instance;
		rplidar_response_measurement_node_hq_t angle_compensate_nodes[angle_compensate_nodes_count];
		grab_ScanData(angle_compensate_nodes, drv, angle_compensate_multiple, angle_compensate_nodes_count);
		printf("grabStart\n");
		for (i = 0; i < 360; i++) {
			float read_value = (float) angle_compensate_nodes[i].dist_mm_q2/4.0f/1000.;
			//printf("%f\n", read_value);
			// if (read_value == 0.0)
			// 	printf("%f, ", std::numeric_limits<float>::infinity());
			// else
			dist[i] = (float)read_value;
			angle[i] = getAngle(angle_compensate_nodes[i]);
			//printf("%f %f\n", dist[i], angle[i]);
		}
	}
	void disposeDriver_c(void* instance)
	{
		RPlidarDriver* drv = (RPlidarDriver*)instance;
		disposeDriver(drv);
	}
}
#endif
