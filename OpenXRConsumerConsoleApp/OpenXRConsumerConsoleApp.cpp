// OpenXRConsumerConsoleApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "OpenXRConsumerConsoleApp.h"

int main()
{
    std::cout << "Hello World!\n";
	print_api_layers();
}

static void print_api_layers()
{
	uint32_t count = 0;
	XrResult result = xrEnumerateApiLayerProperties(0, &count, NULL);
	if (!xr_check(NULL, result, "Failed to enumerate api layer count"))
		return;

	if (count == 0)
		return;

	XrApiLayerProperties* props = new XrApiLayerProperties[count];
	for (uint32_t i = 0; i < count; i++) {
		props[i].type = XR_TYPE_API_LAYER_PROPERTIES;
		props[i].next = NULL;
	}

	result = xrEnumerateApiLayerProperties(count, &count, props);
	if (!xr_check(NULL, result, "Failed to enumerate api layers"))
		return;

	printf("API layers:\n");
	for (uint32_t i = 0; i < count; i++) {
		printf("\t%s v%d: %s\n", props[i].layerName, props[i].layerVersion, props[i].description);

		PrintInstanceExtensions(props[i].layerName);
	}

	delete[] props;
}

static void PrintInstanceExtensions(char* p_APILayer)
{
	uint32_t t_Count = 0;
	XrResult t_Result = xrEnumerateInstanceExtensionProperties(p_APILayer, 0, &t_Count, NULL);

	if (!xr_check(NULL, t_Result, "Failed to enumerate instance extension properties"))
		return;

	if (t_Count == 0)
		return;

	XrExtensionProperties* t_Properties = new XrExtensionProperties[t_Count];
	for (uint32_t i = 0; i < t_Count; i++)
	{
		t_Properties[i].type = XR_TYPE_EXTENSION_PROPERTIES;
		t_Properties[i].next = NULL;
	}

	t_Result = xrEnumerateInstanceExtensionProperties(p_APILayer, t_Count, &t_Count, t_Properties);
	if (!xr_check(NULL, t_Result, "Failed to enumerate instance extension properties"))
		return;

	printf("Extensions:\n");
	for (uint32_t i = 0; i < t_Count; i++) {
		printf("\t%s v%d:", t_Properties[i].extensionName, t_Properties[i].extensionVersion);
	}

	CreateInstance();

	delete[] t_Properties;
}

static void CreateInstance()
{
	uint32_t t_ApiCount = 0;
	XrResult t_Result = xrEnumerateApiLayerProperties(0, &t_ApiCount, NULL);
	if (!xr_check(NULL, t_Result, "Failed to enumerate api layer count"))
		return;

	if (t_ApiCount == 0)
		return;

	XrApiLayerProperties* t_ApiLayerProps = new XrApiLayerProperties[t_ApiCount];
	for (uint32_t i = 0; i < t_ApiCount; i++) {
		t_ApiLayerProps[i].type = XR_TYPE_API_LAYER_PROPERTIES;
		t_ApiLayerProps[i].next = NULL;
	}

	t_Result = xrEnumerateApiLayerProperties(t_ApiCount, &t_ApiCount, t_ApiLayerProps);
	if (!xr_check(NULL, t_Result, "Failed to enumerate api layers"))
		return;

	char** t_ApiNames = new char* [t_ApiCount];
	for (uint32_t i = 0; i < t_ApiCount; i++) {
		t_ApiNames[i] = t_ApiLayerProps[i].layerName;
	}

	uint32_t t_ExtensionCount = 0;

	//TODO make this work when multiple api layers are available.
	xrEnumerateInstanceExtensionProperties(t_ApiNames[0], 0, &t_ExtensionCount, NULL);
	XrExtensionProperties* t_ExtensionProperties = new XrExtensionProperties[t_ExtensionCount];
	for (uint32_t i = 0; i < t_ExtensionCount; i++) {
		t_ExtensionProperties[i].type = XR_TYPE_EXTENSION_PROPERTIES;
		t_ExtensionProperties[i].next = NULL;
	}
	xrEnumerateInstanceExtensionProperties(t_ApiNames[0], t_ExtensionCount, &t_ExtensionCount, t_ExtensionProperties);
		
	char** t_ExtensionNames = new char* [t_ExtensionCount];
	for (uint32_t i = 0; i < t_ExtensionCount; i++) {
		t_ExtensionNames[i] = t_ExtensionProperties[i].extensionName;
	}

	uint32_t t_ApplicationVersion = 1;
	uint32_t t_EngineVersion = 0;

	XrApplicationInfo t_ApplicationInfo{
		"Manus OpenXR Data-Consumer App",
		t_ApplicationVersion,
		"",
		t_EngineVersion,
		XR_CURRENT_API_VERSION
	};

	XrInstanceCreateInfo t_InstanceCreateInfo = { XR_TYPE_INSTANCE_CREATE_INFO };
	t_InstanceCreateInfo.next = NULL;
	t_InstanceCreateInfo.createFlags = 0;
	t_InstanceCreateInfo.applicationInfo = t_ApplicationInfo;
	t_InstanceCreateInfo.enabledApiLayerCount = t_ApiCount;
	t_InstanceCreateInfo.enabledApiLayerNames = t_ApiNames;
	t_InstanceCreateInfo.enabledExtensionCount = t_ExtensionCount;
	t_InstanceCreateInfo.enabledExtensionNames = t_ExtensionNames;

	XrInstance t_NewInstance;
	t_Result = xrCreateInstance(&t_InstanceCreateInfo, &t_NewInstance);

	if (!xr_check(NULL, t_Result, "Failed to create instance"))
		return;

	XrSystemGetInfo t_SystemGetInfo = { XR_TYPE_SYSTEM_GET_INFO };
	t_SystemGetInfo.next = NULL;
	t_SystemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

	XrSystemId t_SystemId;

	t_Result = xrGetSystem(t_NewInstance, &t_SystemGetInfo, &t_SystemId);
	if (!xr_check(NULL, t_Result, "Failed to get system"))
		return;

	XrSessionCreateInfo t_SessionCreateInfo = { XR_TYPE_SESSION_CREATE_INFO };
	t_SessionCreateInfo.next = NULL;
	t_SessionCreateInfo.createFlags = 0;
	t_SessionCreateInfo.systemId = t_SystemId;

	XrSession t_Session;

	t_Result = xrCreateSession(t_NewInstance, &t_SessionCreateInfo, &t_Session);
	if (!xr_check(NULL, t_Result, "Failed to create session"))
		return;
}

// true if XrResult is a success code, else print error message and return false
bool xr_check(XrInstance instance, XrResult result, const char* format, ...)
{
	if (XR_SUCCEEDED(result))
		return true;

	char resultString[XR_MAX_RESULT_STRING_SIZE];
	xrResultToString(instance, result, resultString);

	char formatRes[XR_MAX_RESULT_STRING_SIZE + 1024];
	snprintf(formatRes, XR_MAX_RESULT_STRING_SIZE + 1023, "%s [%s] (%d)\n", format, resultString,
		result);

	va_list args;
	va_start(args, format);
	vprintf(formatRes, args);
	va_end(args);

	return false;
}