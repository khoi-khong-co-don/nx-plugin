﻿#include "dahua_event_controller.h"
#include "dahua_camera.h"
#ifdef _WIN32
#else
#include <dlfcn.h>
#include <unistd.h>
#endif

inline std::string DateFormat(int year, int month, int day)
{
	char buf[128] = { 0 };
	snprintf(buf, sizeof(buf), "%02d-%02d-%04d",
		day,
		month,
		year);
	return std::string(buf);
}

inline std::string NetTimeEx2StrEx(const NET_TIME_EX& netTime)
{
	char buf[128] = { 0 };
	snprintf(buf, sizeof(buf), "%04d%02d%02d%02d%02d%02d%03d",
		netTime.dwYear,
		netTime.dwMonth,
		netTime.dwDay,
		netTime.dwHour,
		netTime.dwMinute,
		netTime.dwSecond,
		netTime.dwMillisecond);
	return std::string(buf);
}



namespace nx {
	namespace vms_server_plugins {
		namespace analytics {
			namespace stub {
				namespace TrafficPlate {

					DahuaEventController::DahuaEventController() {

					} 
					DahuaEventController::~DahuaEventController() {

					}

					void DahuaEventController::capture(DEV_EVENT_TRAFFICJUNCTION_INFO* pstInfo, BYTE* pBuffer, DWORD dwBufSize, LDWORD dwUser) {

						DahuaCamera* dahua_cam = (DahuaCamera*)dwUser;

						std::string strHost = dahua_cam->host();
						std::string strPort = dahua_cam->port();

						std::string strStoreImgPath;
						int lastSlashPos;
						std::string folderImage;
						LOG("CREATE IMAGE!!!!!!!!!");
#ifdef WIN32
						HMODULE hModule = NULL;

						hModule = ::GetModuleHandle("detection_analytics_plugin.dll");

						char szFullDllPath[_MAX_PATH] = { 0 };
						char szModuleDir[_MAX_PATH] = { 0 };

						::GetModuleFileName(hModule, szFullDllPath, _MAX_PATH);
						char* lastBackslash = strrchr(szFullDllPath, '\\');
						size_t lastBackslashPos = lastBackslash - szFullDllPath + 1;
						strncpy(szModuleDir, szFullDllPath, lastBackslashPos);
						strStoreImgPath = std::string(szModuleDir);
						strStoreImgPath.append("StoreImageTraffic\\");
						LOG(strStoreImgPath);
#else
						char buffer[4096];
						if (getcwd(buffer, sizeof(buffer)) != nullptr) {
							std::string currentDirectory(buffer);
							memset(buffer, 0, sizeof(buffer));
							strStoreImgPath = currentDirectory.append("/StoreImageTraffic/");
						}
#endif
						std::string strGUID(NetTimeEx2StrEx(pstInfo->UTC));
						std::string strJpgFileCut = strStoreImgPath + strGUID + std::string("Host-") + strHost + std::string("Port-") + strPort + std::string("_traffic-plate.jpg");
						std::string strJpgFileFull = strStoreImgPath + strGUID + std::string("Host-") + strHost + std::string("Port-") + strPort + std::string("_missing-traffic-plate.jpg");


						BYTE* pBuf = NULL;
						int dwOffset = pstInfo->stuObject.stPicInfo.dwOffSet;
						int dwOffsetLength = pstInfo->stuObject.stPicInfo.dwFileLenth;

						if (pBuffer != NULL)
						{
							pBuf = new BYTE[dwOffsetLength];
							memset(pBuf, 0, dwOffsetLength);
							memcpy(pBuf, pBuffer + dwOffset, dwOffsetLength);
						}

						// Chuyển đổi con trỏ pBuf thành một std::vector
						std::vector<BYTE> bufferVector(pBuf, pBuf + dwOffsetLength);

						// Kiểm tra dung lượng của mảng byte bufferVector
						size_t bufferSize = bufferVector.size();

						if (bufferSize != 0)
						{
							FILE* fpCut = fopen(strJpgFileCut.c_str(), "wb");
							if (fpCut == NULL)
							{
								LOG("Save picture failed!");
								return;
							}

							fwrite(pBuf, dwOffsetLength, 1, fpCut);
							fclose(fpCut);
							this->setImgpath(strJpgFileCut);
						}
						else
						{
							FILE* fpFull = fopen(strJpgFileFull.c_str(), "wb");
							if (fpFull == NULL)
							{
								LOG("Save picture failed!");
								return;
							}

							fwrite(pBuffer, dwBufSize, 1, fpFull);
							fclose(fpFull);
							this->setImgpath(strJpgFileFull);
						}

						LOG(strJpgFileCut);
						
						delete[] pBuf;
					}

					void DahuaEventController::saveFaceDetection(DEV_EVENT_FACERECOGNITION_INFO* pstInfo) {
						// NET_FACE_DATA* pstDataInfo = &(pstInfo->stuFaceData);

						// DH_MSG_OBJECT* pstFaceInfo = &(pstInfo->stuObject);

						// this->setFaceDataInfo({
						// 	to_string(pstDataInfo->nAge),
						// 	kSex[pstDataInfo->emSex],
						// 	kBeard[pstDataInfo->emBeard],
						// 	kEye[pstDataInfo->emEye],
						// 	kMouth[pstDataInfo->emMouth],
						// 	kMask[pstDataInfo->emMask],
						// 	kGlass[pstDataInfo->emGlass]
						// 	});

					}

					void DahuaEventController::saveFaceRecognition(CANDIDATE_INFOEX* pstCandidate) {
						// FACERECOGNITION_PERSON_INFOEX* pstPersonInfo = &(pstCandidate->stPersonInfo);
						// this->setRecognitionDataInfo({
						// 	pstPersonInfo->szPersonName,
						// 	DateFormat(pstPersonInfo->wYear, pstPersonInfo->byMonth, pstPersonInfo->byDay),
						// 	pstPersonInfo->szCity,
						// 	pstPersonInfo->szCountry
						// 	});
						
					}

					void DahuaEventController::saveTrafficPlateDetection(DEV_EVENT_TRAFFICJUNCTION_INFO* pstInfo) {

						LOG("SET DATA");

						this->setTrafficplateDataInfos({
							pstInfo->stTrafficCar.szPlateNumber,
							pstInfo->stTrafficCar.szPlateColor,
							pstInfo->stuVehicle.szObjectSubType,
							std::to_string(pstInfo->stTrafficCar.nLane),
							std::to_string(pstInfo->nSpeed)
							});
					}
				}
			} // namespace sample
		} // namespace analytics
	} // namespace vms_server_plugins
} // namespace nx