#include "CConfigStorageHjson.h"
#include "CSlrString.h"
#include "SYS_Main.h"
#include "CByteBuffer.h"
#include "CSlrString.h"
#include <string>
#include <sstream>

CConfigStorageHjson::CConfigStorageHjson(const char *configFileName)
{
	this->configFileName = new CSlrString(configFileName);	
	ReadConfig();
}

CConfigStorageHjson::~CConfigStorageHjson()
{
	delete configFileName;
}
	

void CConfigStorageHjson::ReadConfig()
{
	hjsonRoot.clear();

	CByteBuffer *byteBuffer = new CByteBuffer();
	bool available = byteBuffer->loadFromSettings(configFileName);

	if (available)
	{
		byteBuffer->ForwardToEnd();
		byteBuffer->PutU8(0x00);
		byteBuffer->Rewind();
		
		char *hjsonData = (char*)byteBuffer->data;

		std::stringstream ss;
		ss.str(hjsonData);
		
		try
		{
			ss >> hjsonRoot;
		}
		catch(const std::exception& e)
		{
			LOGError("CConfigStorageHjson::ReadConfig error: %s", e.what());
			hjsonRoot.clear();
		}
	}
	
	delete byteBuffer;
}

void CConfigStorageHjson::SaveConfig()
{
	std::stringstream ss;
	ss << Hjson::Marshal(hjsonRoot);
	
	std::string s = ss.str();
	const char *cstr = s.c_str();
	
	CByteBuffer *byteBuffer = new CByteBuffer();
	byteBuffer->PutBytes((u8*)cstr, (unsigned int)strlen(cstr));
	
	byteBuffer->storeToSettings(configFileName);
	
	delete byteBuffer;
}

void CConfigStorageHjson::SetBool(const char *name, bool *value)
{
	hjsonRoot[name] = *value;
	SaveConfig();
}

void CConfigStorageHjson::SetBoolSkipConfigSave(const char *name, bool *value)
{
	hjsonRoot[name] = *value;
}

void CConfigStorageHjson::GetBool(const char *name, bool *value, bool defaultValue)
{
	Hjson::Value hValue;
	hValue = hjsonRoot[name];
	
	if (hValue == Hjson::Type::Undefined)
	{
		*value = defaultValue;
		return;
	}
	
	try
	{
		*value = static_cast<const bool>(hValue);
	}
	catch(const std::exception& e)
	{
//		LOGError("CConfigStorageHjson::GetBool error: %s", e.what());
		*value = defaultValue;
	}
}

bool CConfigStorageHjson::GetBool(const char *name, bool defaultValue)
{
	bool value = defaultValue;
	GetBool(name, &value, defaultValue);
	return value;
}


void CConfigStorageHjson::SetInt(const char *name, int *value)
{
	hjsonRoot[name] = *value;
	SaveConfig();
}

void CConfigStorageHjson::GetInt(const char *name, int *value, int defaultValue)
{
	Hjson::Value hValue;
	hValue = hjsonRoot[name];
	
	if (hValue == Hjson::Type::Undefined)
	{
		*value = defaultValue;
		return;
	}
	
	try
	{
		*value = static_cast<const int>(hValue);
	}
	catch(const std::exception& e)
	{
//		LOGError("CConfigStorageHjson::GetInt error: %s", e.what());
		*value = defaultValue;
	}
}

void CConfigStorageHjson::SetFloat(const char *name, float *value)
{
	hjsonRoot[name] = *value;
	SaveConfig();
}

void CConfigStorageHjson::GetFloat(const char *name, float *value, float defaultValue)
{
	Hjson::Value hValue;
	hValue = hjsonRoot[name];
	
	if (hValue == Hjson::Type::Undefined)
	{
		*value = defaultValue;
		return;
	}
	
	try
	{
		*value = static_cast<const float>(hValue);
	}
	catch(const std::exception& e)
	{
//		LOGError("CConfigStorageHjson::GetDouble error: %s", e.what());
		*value = defaultValue;
	}
}

void CConfigStorageHjson::SetDouble(const char *name, double *value)
{
	hjsonRoot[name] = *value;
	SaveConfig();
}

void CConfigStorageHjson::GetDouble(const char *name, double *value, double defaultValue)
{
	Hjson::Value hValue;
	hValue = hjsonRoot[name];
	
	if (hValue == Hjson::Type::Undefined)
	{
		*value = defaultValue;
		return;
	}
	
	try
	{
		*value = static_cast<const double>(hValue);
	}
	catch(const std::exception& e)
	{
//		LOGError("CConfigStorageHjson::GetDouble error: %s", e.what());
		*value = defaultValue;
	}
}

void CConfigStorageHjson::SetString(const char *name, const char *value)
{
	hjsonRoot[name] = value;
	SaveConfig();
}

void CConfigStorageHjson::SetString(const char *name, const char **value)
{
	hjsonRoot[name] = *value;
	SaveConfig();
}

void CConfigStorageHjson::GetString(const char *name, const char **value, const char *defaultValue)
{
	Hjson::Value hValue;
	hValue = hjsonRoot[name];
	
	if (hValue == Hjson::Type::Undefined)
	{
		*value = defaultValue;
		return;
	}

	try
	{
		*value = static_cast<const char *>(hValue);
	}
	catch(const std::exception& e)
	{
//		LOGError("CConfigStorageHjson::GetString error: %s", e.what());
		*value = defaultValue;
	}
}

void CConfigStorageHjson::GetString(const char *name, char *value, int stringMaxLength, const char *defaultValue)
{
	Hjson::Value hValue;
	hValue = hjsonRoot[name];
	
	if (hValue == Hjson::Type::Undefined)
	{
		memset((u8*)value, 0, stringMaxLength);
		strncpy(value, defaultValue, stringMaxLength);
		return;
	}

	const char *strValue;
	
	try
	{
		strValue = static_cast<const char *>(hValue);
	}
	catch(const std::exception& e)
	{
//		LOGError("CConfigStorageHjson::GetString error: %s", e.what());
		strValue = defaultValue;
	}
	
	memset((u8*)value, 0, stringMaxLength);
	strncpy(value, strValue, stringMaxLength);
}

void CConfigStorageHjson::SetSlrString(const char *name, CSlrString **value)
{
//	LOGD("CConfigStorageHjson::SetSlrString: name=%s", name);
//	(*value)->DebugPrint("value=");
	
	char *str = (*value)->GetStdASCII();
	hjsonRoot[name] = str;
	delete [] str;
	SaveConfig();
}

// Note, this method deletes defaultValue so one can use GetSlrString("variableName", &str, new CSlrString("my default value"))
void CConfigStorageHjson::GetSlrString(const char *name, CSlrString **value, CSlrString *defaultValue)
{
	Hjson::Value hValue;
	hValue = hjsonRoot[name];
	
	if (hValue == Hjson::Type::Undefined)
	{
		*value = defaultValue;
		return;
	}

	try
	{
		const char *buf = static_cast<const char *>(hValue);
		*value = new CSlrString(buf);
		delete defaultValue;
	}
	catch(const std::exception& e)
	{
//		LOGError("CConfigStorageHjson::GetString error: %s", e.what());
		*value = defaultValue;
	}
}

bool CConfigStorageHjson::E_x_i_s_t_s(const char *name)
{
	Hjson::Value hValue;
	hValue = hjsonRoot[name];
	
	if (hValue == Hjson::Type::Undefined)
	{
		return false;
	}
	return true;
}
