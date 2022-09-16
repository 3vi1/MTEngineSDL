#include "DBG_Log.h"
#include "SYS_FileSystem.h"
#include "CFeatureConfig.h"
#include "CConfigStorageHjson.h"
#include "CSlrString.h"
#include "CByteBuffer.h"
#include "SYS_Funct.h"
#include <sstream>

CFeatureConfig::CFeatureConfig()
{
	featureErrorText[0] = 0;
}

CFeatureConfig::~CFeatureConfig()
{
	
}

void CFeatureConfig::InitConfig(const char *featureName)
{
	LOGD("CFeatureConfig::InitConfig name=%s", featureName);
	featureErrorText[0] = 0;
	
	char *buf = SYS_GetCharBuf();
	sprintf(buf, "%s.hjson", featureName);
	
	// default program config
	this->featureSettings = new CConfigStorageHjson(buf);
	
	EMPTY_PATH_ALLOC(featureConfigPath);
	EMPTY_PATH_ALLOC(featureRootFolderPath);
	featureSettings->GetSlrString("DefaultFolder", &featureDefaultFolder, gUTFPathToDocuments);
	featureSettings->GetSlrString("DefaultConfigPath", &featureDefaultConfigPath, NULL);

	if (featureDefaultConfigPath)
	{
		LoadConfig(featureDefaultConfigPath);
	}
	
	SYS_ReleaseCharBuf(buf);
}

void CFeatureConfig::SetFeatureRootFolderPath(CSlrString *path)
{
	CSlrString *folder = path->GetFilePathWithoutFileNameComponentFromPath();

	featureDefaultFolder->Set(folder);
	featureSettings->SetSlrString("DefaultFolder", &folder);
	folder->AddPathSeparatorAtEnd();
	char *strFolder = folder->GetStdASCII();
	delete folder;
	
	strcpy(featureRootFolderPath, strFolder);
	
	STRFREE(strFolder);
}

void CFeatureConfig::LoadConfig(CSlrString *path)
{
	char *cstr = path->GetStdASCII();
	strcpy(featureConfigPath, cstr);
	delete [] cstr;
	
	CByteBuffer *byteBuffer = new CByteBuffer(featureConfigPath);
	if (!byteBuffer->IsEmpty())
	{
		char *jsonText = new char[byteBuffer->length+2];
		memcpy(jsonText, byteBuffer->data, byteBuffer->length);
		jsonText[byteBuffer->length] = 0;
		delete byteBuffer;
		
		Hjson::Value hjsonRoot;
		std::stringstream ss;
		ss.str(jsonText);

		try
		{
			ss >> hjsonRoot;
			this->InitFromHjson(hjsonRoot);
		}
		catch(const std::exception& e)
		{
			LOGError("CFeatureConfig::InitFromHjson error: %s", e.what());
			sprintf(featureErrorText, "Config file not correct, %s", e.what());
			hjsonRoot.clear();
			delete [] jsonText;
			return;
		}
		delete [] jsonText;
	}
	else
	{
		LOGError("CFeatureConfig::LoadConfig: empty CByteBuffer, failed to init");
		sprintf(featureErrorText, "Config file not correct, empty CByteBuffer, failed to init");
		return;
	}

	CSlrString *folder = path->GetFilePathWithoutFileNameComponentFromPath();
	featureSettings->SetSlrString("DefaultFolder", &folder);
	folder->AddPathSeparatorAtEnd();
	char *strFolder = folder->GetStdASCII();
	delete folder;
	
	LOGD("Config loaded, root folder=%s", strFolder);
	strcpy(featureRootFolderPath, strFolder);
	
	STRFREE(strFolder);
	
	featureSettings->SetSlrString("DefaultConfigPath", &path);
	featureErrorText[0] = 0;
}

void CFeatureConfig::SaveConfig(CSlrString *path)
{
	featureErrorText[0] = 0;

	const char *cstrPath = path->GetStdASCII();
	strcpy(featureConfigPath, cstrPath);
	delete [] cstrPath;
	
	CSlrString *folder = path->GetFilePathWithoutFileNameComponentFromPath();
	featureSettings->SetSlrString("DefaultFolder", &folder);
	delete folder;

	featureSettings->SetSlrString("DefaultConfigPath", &path);

	FILE *fp = fopen(featureConfigPath, "wb");
	
	if (!fp)
	{
		LOGError("CFeatureConfig::SaveConfig: can't write to %s", featureConfigPath);
		return;
	}
	
	
	Hjson::Value hjsonRoot;
	StoreToHjson(hjsonRoot);
			
	// store config
	std::stringstream ss;
	ss << Hjson::Marshal(hjsonRoot);
	
	std::string s = ss.str();
	const char *cstrHjson = s.c_str();

	fprintf(fp, "%s", cstrHjson);
	
	fclose(fp);
}

void CFeatureConfig::InitFromHjson(Hjson::Value hjsonRoot)
{
	// Example:
//	settingExportToPrg 	= static_cast<const bool>(hjsonRoot["ExportToPrg"]);
//	strcpy(settingPrgOutputPath, static_cast<const char *>(hjsonRoot["PrgOutputPath"]));
//
//	const char *hexValueStr;
//	hexValueStr = static_cast<const char *>(hjsonRoot["PackedTexturesAddr"]);
//	PACKED_TEXTURES_ADDR = strtoul( hexValueStr, NULL, 16 );

}

void CFeatureConfig::StoreToHjson(Hjson::Value hjsonRoot)
{
	// Example:
//	hjsonRoot["ExportToPrg"] = settingExportToPrg;
//	hjsonRoot["PrgOutputPath"] = settingPrgOutputPath;

//	char hexStr[16];
//	sprintf(hexStr, "%04x", PACKED_TEXTURES_ADDR);
//	hjsonRoot["PackedTexturesAddr"] = hexStr;
}

