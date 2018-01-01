/********************************************************  
    * @file    : process.cpp 
    * @brief   : 对点云进行去噪，配准 
    * @details :  
    * @author  : xiaohui zhou 
    * @version : ver 1.0
    * @date    : 2017-12-25 
*********************************************************/
#include <iomanip>
#include <iostream>

#include <string>
#include <vector>

#include <artec/sdk/base/Log.h>
#include <artec/sdk/base/Errors.h>

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
#include "Directory.h"
namespace asdk {
	using namespace artec::sdk::base;
	using namespace artec::sdk::capturing;
	using namespace artec::sdk::scanning;
	using namespace artec::sdk::algorithms;
};
using asdk::TRef;
using namespace std;

#define OUTPUT_DIR L"scans"

// simple error log handling for SDK calls
#define SDK_STRINGIFY(x) #x
#define SDK_STRING(x) SDK_STRINGIFY(x)
#define SAFE_SDK_CALL(x)                                                 \
{                                                                        \
    asdk::ErrorCode ec = (x);                                            \
    if ( ec != asdk::ErrorCode_OK )                                      \
    {                                                                    \
        reportError( ec, __FILE__ " [ line " SDK_STRING(__LINE__) " ]"); \
        return ec;                                                       \
    }                                                                    \
}

void reportError(asdk::ErrorCode ec, const char *place)
{
	const wchar_t* msg = L"No error";

	switch (ec) {

	case asdk::ErrorCode_OutOfMemory:
		msg = L"Not enough storage is available to process the operation";
		break;

	case asdk::ErrorCode_ArgumentInvalid:
		msg = L"Provided argument is invalid";
		break;

	case asdk::ErrorCode_OperationInvalid:
		msg = L"Requested operation is invalid";
		break;

	case asdk::ErrorCode_FormatUnsupported:
		msg = L"Data format is unsupported or invalid";
		break;

	case asdk::ErrorCode_ScannerNotConnected:
		msg = L"Requested scanner is not connected";
		break;

	case asdk::ErrorCode_ScannerNotLicensed:
		msg = L"Requested scanner is not licensed";
		break;

	case asdk::ErrorCode_ScannerLocked:
		msg = L"Requested scanner is already used by someone else";
		break;

	case asdk::ErrorCode_ScannerInitializationFailed:
		msg = L"Scanner initialization failed";
		break;

	case asdk::ErrorCode_FrameCorrupted:
		msg = L"Frame is corrupted";
		break;

	case asdk::ErrorCode_FrameReconstructionFailed:
		msg = L"Frame reconstruction failed";
		break;

	case asdk::ErrorCode_FrameRegistrationFailed:
		msg = L"Frame registration failed";
		break;

	case asdk::ErrorCode_OperationUnsupported:
		msg = L"Requested operation is unsupported. Check versions";
		break;

	case asdk::ErrorCode_OperationDenied:
		msg = L"Requested operation is denied. Check your license(s)";
		break;

	case asdk::ErrorCode_OperationFailed:
		msg = L"Requested operation has failed";
		break;

	case asdk::ErrorCode_OperationAborted:
		msg = L"Requested operation was canceled from client's side";
		break;

	case asdk::ErrorCode_AllFramesAreFilteredOut:
		msg = L"Unable to start algorithm because input data turned out to be invalid. Please rescan the object.";
		break;

	default:
		msg = L"Unexplained error";
		break;
	}

	std::wcerr << msg << " [error " << std::hex << ec << "] " << "at " << place << std::endl;
}

int main(int argc, char **argv)
{
	string path = "D:/Zhouxh-project/source code/artec/artec-sdk-samples-v2.0/samples/scanning-and-process/scans";
	//asdk::setOutputLevel(asdk::VerboseLevel_Info);
	//// create workset for scanned data
	//TRef<asdk::IModel> inputContainer;
	//TRef<asdk::IModel> outputContainer;
	//TRef<asdk::ICancellationTokenSource> ctSource;

	//SAFE_SDK_CALL(asdk::createModel(&inputContainer));
	//SAFE_SDK_CALL(asdk::createModel(&outputContainer));
	//SAFE_SDK_CALL(asdk::createCancellationTokenSource(&ctSource));

	//asdk::AlgorithmWorkset workset = { inputContainer, outputContainer, 0, ctSource->getToken(), 0 };

	vector< TRef<asdk::IFrameMesh> > frameMesh;
	std::vector<std::string> filenames = Directory::GetListFiles(path, "*.obj");
	if (filenames.size() == 1)
	{
		cout << "当前只扫描了一次，无法进行后处理，请获取更多数据" << endl;
	}
	else
	{

		TRef<asdk::IFrameMesh> inputMesh;
		for (int i=0;i<filenames.size();i++)
		{
			string filePath = path + filenames[0];
			std::wstring widestr = std::wstring(filePath.begin(), filePath.end());
			const  wchar_t* widecstr = widestr.c_str();

			artec::sdk::base::io::loadObjFrameFromFile(&inputMesh, widecstr);

		}
	}
	for (int i = 0; i < frameMesh.size(); i++)
	{
		TRef<asdk::IFrameMesh> frameMeshTest(frameMesh[i]);

		std::wstring pathFormat(OUTPUT_DIR L"\\frame-outliers-S%02dF%02d.obj");
		std::vector<wchar_t> pathBuffer(pathFormat.size() + 1);
		std::swprintf(pathBuffer.data(), pathBuffer.size(), pathFormat.c_str(), 0, 0);

		asdk::io::saveObjFrameToFile(pathBuffer.data(), frameMeshTest);
	}

	return 0;
}