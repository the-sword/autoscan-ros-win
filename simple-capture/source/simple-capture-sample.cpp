/********************************************************************
*
*	Project		Artec 3D Scanning SDK Samples
*
*	Purpose:	Simple capture sample
*
*	Copyright:	Artec Group
*
********************************************************************/

#undef NDEBUG
#include <iomanip>
#include <string>
#include <iostream>
#include <time.h> 
#include <artec/sdk/capturing/IFrameProcessor.h>
#include <artec/sdk/capturing/IFrame.h>
#include <artec/sdk/base/BaseSdkDefines.h>
#include <artec/sdk/base/Log.h>
#include <artec/sdk/base/Errors.h>
#include <artec/sdk/base/TArrayRef.h>


#include <artec/sdk/base/TRef.h>
#include <artec/sdk/base/RefBase.h>
#include <artec/sdk/base/IFrameMesh.h>
#include <artec/sdk/base/ICompositeMesh.h>
#include <artec/sdk/base/ICompositeContainer.h>
#include <artec/sdk/base/IModel.h>
#include <artec/sdk/base/ICancellationTokenSource.h>
#include <artec/sdk/base/io/ObjIO.h>
#include <artec/sdk/base/io/PlyIO.h>
#include <artec/sdk/base/AlgorithmWorkset.h>
#include <artec/sdk/capturing/IScanner.h>
#include <artec/sdk/capturing/IScannerObserver.h>
#include <artec/sdk/capturing/IArrayScannerId.h>
#include <artec/sdk/scanning/IScanningProcedure.h>
#include <artec/sdk/scanning/IArrayScanner.h>
#include <artec/sdk/scanning/IScanningProcedureBundle.h>
#include <artec/sdk/algorithms/IAlgorithm.h>
#include <artec/sdk/algorithms/Algorithms.h>

namespace asdk {
	using namespace artec::sdk::base;
	using namespace artec::sdk::capturing;
	using namespace artec::sdk::scanning;
	using namespace artec::sdk::algorithms;
};
using asdk::TRef;
using asdk::TArrayRef;

////**added by Minliang LIN for outlier alogrithm
//#include <artec/sdk/base/AlgorithmWorkset.h>
//#include <artec/sdk/algorithms/IAlgorithm.h>
//#include <artec/sdk/algorithms/Algorithms.h>
//namespace asdk {
//	using namespace artec::sdk::algorithms;
//};
//#define SAFE_SDK_CALL(x)                                                 \
//{                                                                        \
//    asdk::ErrorCode ec = (x);                                            \
//    if ( ec != asdk::ErrorCode_OK )                                      \
//    {                                                                    \
//        reportError( ec, __FILE__ " [ line " SDK_STRING(__LINE__) " ]"); \
//        return ec;                                                       \
//    }                                                                    \
//}
////**end

int main(int argc, char **argv)
{
	//add time to file to avoid replace the same file
	time_t currtime = time(NULL);
	tm* p = localtime(&currtime);
	char filename[100] = { 0 };

	// The log verbosity level is set here. It is set to the most
	// verbose value - Trace. If you have any problems working with 
	// our examples, please do not hesitate to send us this extensive 
	// information along with your questions. However, if you feel 
	// comfortable with these Artec Scanning SDK code examples,
	// we suggest you to set this level to asdk::VerboseLevel_Info.
	asdk::setOutputLevel(asdk::VerboseLevel_Trace);

	asdk::ErrorCode ec = asdk::ErrorCode_OK;

	TRef<asdk::IArrayScannerId> scannersList;

	std::wcout << L"Enumerating scanners... ";
	ec = asdk::enumerateScanners(&scannersList);
	if (ec != asdk::ErrorCode_OK)
	{
		std::wcout << L"failed" << std::endl;
		return 1;
	}
	std::wcout << L"done" << std::endl;

	int scanner_count = scannersList->getSize();
	if (scanner_count == 0)
	{
		std::wcout << L"No scanners found" << std::endl;
		return 3;
	}

	const asdk::ScannerId* idArray = scannersList->getPointer();

	const asdk::ScannerId& defaultScanner = idArray[0]; // just take the first available scanner

	std::wcout
		<< L"Connecting to " << asdk::getScannerTypeName(defaultScanner.type)
		<< L" scanner " << defaultScanner.serial << L"... "
		;

	TRef<asdk::IScanner> scanner;
	ec = asdk::createScanner(&scanner, &defaultScanner);

	if (ec != asdk::ErrorCode_OK)
	{
		std::wcout << L"failed" << std::endl;
		return 2;
	}
	std::wcout << L"done" << std::endl;

	std::wcout << L"Capturing frame... ";

	TRef<asdk::IFrame> frame;
	TRef<asdk::IFrameMesh> mesh;

	TRef<asdk::IFrameProcessor> processor;
	ec = scanner->createFrameProcessor(&processor);
	if (ec == asdk::ErrorCode_OK)
	{
		//processor->setScanningRange();
		//processor->setROI();
		float near, far;
		asdk::RectF rect;
		//获取扫描仪的扫描距离
		processor->getScanningRange(&near, &far);
		processor->setSensitivity(0.9f);		//region of interest
		processor->getROI(&rect);

		frame = NULL;
		ec = scanner->capture(&frame, true); // with texture

		if (ec == asdk::ErrorCode_OK)
		{
			mesh = NULL;
			ec = processor->reconstructAndTexturizeMesh(&mesh, frame);
			if (ec == asdk::ErrorCode_OK)
			{
				std::wcout << L"done" << std::endl;
				// save the mesh
				//ec = asdk::io::Obj::save(L"frame.obj", mesh); // save in text format

				// working with normals
				// 1. generate normals
				mesh->calculate(asdk::CM_Normals);

				// 2. get normals array using helper class
				asdk::TArrayPoint3F pointsNormals = mesh->getPointsNormals();

				// 3. get number of normals
				int normalCount = pointsNormals.size();
				ASDK_UNUSED(normalCount);

				// 4. use normal
				asdk::Point3F point = pointsNormals[0];
				ASDK_UNUSED(point);
				//以时间戳命名文件，防止多次调用时覆盖上次文件
				sprintf(filename, "%d%02d%02d%02d%02d%02d.obj", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);
				std::wcout << filename << std::endl;
				char *CStr = filename;
				size_t len = strlen(CStr) + 1;
				size_t converted = 0;
				wchar_t *WStr;
				WStr = (wchar_t*)malloc(len * sizeof(wchar_t));
				mbstowcs_s(&converted, WStr, len, CStr, _TRUNCATE);

				ec = asdk::io::Obj::save(WStr, mesh); // save in text format
				std::wcout << L"Captured mesh saved to disk" << std::endl;
			}
			else
			{
				std::wcout << L"failed" << std::endl;
				std::wcout << "EC: " << ec << std::endl;
			}
		}
	}

	scanner = NULL;
	std::wcout << L"Scanner released" << std::endl;
	return 0;
}
