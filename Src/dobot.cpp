#include "dobot.h"
#include "Protocol.h"
#include "command.h"
#include "main.h"
#include "usart.h"

JOGJointParams gJOGJointParams;
JOGCommonParams gJOGCommonParams;
JOGCmd gJOGCmd;

PTPCoordinateParams gPTPCoordinateParams;
PTPCommonParams gPTPCommonParams;
PTPCmd point_idle;


uint64_t gQueuedCmdIndex;

void dobot_init(void)
{
	gJOGJointParams.velocity[0] = 100;
	gJOGJointParams.velocity[1] = 100;
	gJOGJointParams.velocity[2] = 100;
	gJOGJointParams.velocity[3] = 100;
	gJOGJointParams.acceleration[0] = 80;
	gJOGJointParams.acceleration[1] = 80;
	gJOGJointParams.acceleration[2] = 80;
	gJOGJointParams.acceleration[3] = 80;

	gJOGCommonParams.velocityRatio = 50;
	gJOGCommonParams.accelerationRatio = 50;

	gJOGCmd.cmd = 0;
	gJOGCmd.isJoint = true;

	gPTPCoordinateParams.xyzVelocity = 100;
	gPTPCoordinateParams.rVelocity = 100;
	gPTPCoordinateParams.xyzAcceleration = 80;
	gPTPCoordinateParams.rAcceleration = 80;

	gPTPCommonParams.velocityRatio = 50;
	gPTPCommonParams.accelerationRatio = 50;

	point_idle.ptpMode = MOVJ_XYZ;
	point_idle.x = 206.9603;
	point_idle.y = 53.9545;
	point_idle.z = 51.0530;
	point_idle.r = 56.1713;
	
	gQueuedCmdIndex = 0;
	
	
	ProtocolInit();
	
	SetPTPCoordinateParams(&gPTPCoordinateParams, true, &gQueuedCmdIndex);
	SetPTPCommonParams(&gPTPCommonParams, true, &gQueuedCmdIndex);
	ProtocolProcess();	
}

void grip_chops(int chops_num)
{
	PTPCmd point[10];
	if(chops_num == 2)
	{
		//筷子后
		//筷子1就位点
		point[0].ptpMode = MOVJ_XYZ;
		point[0].x = 187.2871;
		point[0].y = 20.0920;
		point[0].z = 20.7805;
		point[0].r = -31.4446;
		
		//筷子1抓取点
		point[1].ptpMode = MOVL_XYZ;
		point[1].x = 187.2873;
		point[1].y = 20.0920;
		point[1].z = -27.6757;
		point[1].r = -31.4446;
		
		//筷子1取出点
		point[2].ptpMode = MOVL_XYZ;
		point[2].x = 187.2870;
		point[2].y = 20.0920;
		point[2].z = 80.6364;
		point[2].r = -31.4446;
	}
	else if(chops_num == 1)
	{
		//筷子前
		//筷子2就位点
		point[0].ptpMode = MOVJ_XYZ;
		point[0].x = 191.7784;
		point[0].y = -35.3899;
		point[0].z = 4.6727;
		point[0].r = -29.6531;
		
		//筷子2抓取点
		point[1].ptpMode = MOVL_XYZ;
		point[1].x = 191.7785;
		point[1].y = -35.3899;
		point[1].z = -29.2397;
		point[1].r = -29.6531;
		
		//筷子2取出点
		point[2].ptpMode = MOVL_XYZ;
		point[2].x = 191.7792;
		point[2].y = -35.3900;
		point[2].z = 108.7605;
		point[2].r = -29.6531;
	}
	//筷子放置点
	point[3].ptpMode = MOVJ_XYZ;
	point[3].x = 179.9614;
	point[3].y = -218.9740;
	point[3].z = 122.1701;
	point[3].r = -29.2861;
	//移动到筷子上方
	SetPTPCmd(&point[0], true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//进入抓取位置
	SetPTPCmd(&point[1], true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//抓取
	CtrEndEffectorGripper(engage, hold, true, &gQueuedCmdIndex);
	ProtocolProcess();
	HAL_Delay(2000);	
	//取出筷子
	SetPTPCmd(&point[2], true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//移动到泡面上方
	SetPTPCmd(&point[3], true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//释放
	CtrEndEffectorGripper(engage, release, true, &gQueuedCmdIndex);
	ProtocolProcess();
	HAL_Delay(2000);
	//回到初始位置
	SetPTPCmd(&point_idle, true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//关闭气泵
	CtrEndEffectorGripper(idle, release, true, &gQueuedCmdIndex);
	ProtocolProcess();
	printf("[INFO][DOBOT]Grip Chops Success\r\n");
}

void grip_RS(int rs_num)
{
	PTPCmd point[10];
	if(rs_num == 2)
	{
		//红色酱包左
		//红色1就位点
		point[0].ptpMode = MOVJ_XYZ;
		point[0].x = 269.7323;
		point[0].y = 116.4751;
		point[0].z = 64.8922;
		point[0].r = 64.2978;
		
		//红色1抓取点
		point[1].ptpMode = MOVL_XYZ;
		point[1].x = 269.7351;
		point[1].y = 116.4763;
		point[1].z = 22.1959;
		point[1].r = 64.2978;
		
		//红色1取出点
		point[2].ptpMode = MOVL_XYZ;
		point[2].x = 269.7382;
		point[2].y = 116.4776;
		point[2].z = 131.5238;
		point[2].r = 64.2978;
	}
	else if(rs_num == 1)
	{
		//红色右
		//红色2就位点
		point[0].ptpMode = MOVJ_XYZ;
		point[0].x = 225.4056;
		point[0].y = 117.7000;
		point[0].z = 64.8924;
		point[0].r = 64.2978;
		
		//红色2抓取点
		point[1].ptpMode = MOVL_XYZ;
		point[1].x = 225.4055;
		point[1].y = 117.7000;
		point[1].z = 22.6763;
		point[1].r = 64.2978;
		
		//红色2取出点
		point[2].ptpMode = MOVL_XYZ;
		point[2].x = 225.4062;
		point[2].y = 117.7003;
		point[2].z = 136.5081;
		point[2].r = 64.2978;
	}
	//酱料放置点
	point[3].ptpMode = MOVJ_XYZ;
	point[3].x = 173.8375;
	point[3].y = -193.3654;
	point[3].z = 159.0950;
	point[3].r = 11.1381;
	//移动到红色上方
	SetPTPCmd(&point[0], true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//进入抓取位置
	SetPTPCmd(&point[1], true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//抓取
	CtrEndEffectorGripper(engage, hold, true, &gQueuedCmdIndex);
	ProtocolProcess();
	HAL_Delay(2000);	
	//取出红色
	SetPTPCmd(&point[2], true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//移动到泡面上方
	SetPTPCmd(&point[3], true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//释放
	CtrEndEffectorGripper(engage, release, true, &gQueuedCmdIndex);
	ProtocolProcess();
	HAL_Delay(2000);
	//回到初始位置
	SetPTPCmd(&point_idle, true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//关闭气泵
	CtrEndEffectorGripper(idle, release, true, &gQueuedCmdIndex);
	ProtocolProcess();
	printf("[INFO][DOBOT]Grip RedSauce Success\r\n");
}

void grip_GS(int gs_num)
{
	PTPCmd point[10];
	if(gs_num == 2)
	{
		//绿色酱包左
		//绿色1就位点
		point[0].ptpMode = MOVJ_XYZ;
		point[0].x = 169.2775;
		point[0].y = 102.5537;
		point[0].z = 38.4343;
		point[0].r = 59.0402;
		
		//绿色1抓取点
		point[1].ptpMode = MOVL_XYZ;
		point[1].x = 169.2768;
		point[1].y = 105.2174;
		point[1].z = 22.0191;
		point[1].r = 59.0402;
		
		//绿色取出点
		point[2].ptpMode = MOVL_XYZ;
		point[2].x = 169.2744;
		point[2].y = 105.2159;
		point[2].z = 112.1156;
		point[2].r = 59.0402;
	}
	else if(gs_num == 1)
	{
		//绿色右
		//绿色2就位点
		point[0].ptpMode = MOVJ_XYZ;
		point[0].x = 136.7574;
		point[0].y = 103.7061;
		point[0].z = 41.8665;
		point[0].r = 59.0402;
		
		//绿色2抓取点
		point[1].ptpMode = MOVL_XYZ;
		point[1].x = 136.7571;
		point[1].y = 103.7059;
		point[1].z = 22.6903;
		point[1].r = 59.0402;
		
		//绿色取出点
		point[2].ptpMode = MOVL_XYZ;
		point[2].x = 169.2744;
		point[2].y = 105.2159;
		point[2].z = 112.1156;
		point[2].r = 59.0402;
	}
	//酱料放置点
	point[3].ptpMode = MOVJ_XYZ;
	point[3].x = 173.8375;
	point[3].y = -193.3654;
	point[3].z = 159.0950;
	point[3].r = 11.1381;
	//移动到绿色上方
	SetPTPCmd(&point[0], true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//进入抓取位置
	SetPTPCmd(&point[1], true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//抓取
	CtrEndEffectorGripper(engage, hold, true, &gQueuedCmdIndex);
	ProtocolProcess();
	HAL_Delay(2000);	
	//取出绿色
	SetPTPCmd(&point[2], true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//移动到泡面上方
	SetPTPCmd(&point[3], true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//释放
	CtrEndEffectorGripper(engage, release, true, &gQueuedCmdIndex);
	ProtocolProcess();
	HAL_Delay(2000);
	//回到初始位置
	SetPTPCmd(&point_idle, true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);
	//关闭气泵
	CtrEndEffectorGripper(idle, release, true, &gQueuedCmdIndex);
	ProtocolProcess();
	printf("[INFO][DOBOT]Grip GreenSauce Success\r\n");
}

void test_point(void)
{
	PTPCmd test_p;
	test_p.ptpMode = MOVJ_XYZ;
		test_p.x = 262.7873;
		test_p.y = -43.9204;
		test_p.z = -29.5056;
		test_p.r = -1.5106;
	
	SetPTPCmd(&test_p, true, &gQueuedCmdIndex);
	ProtocolProcess();	
	HAL_Delay(2000);

}
