/* 
*  BSD 2-Clause “Simplified” License
*  Copyright (c) 2019, Aldrik Ramaekers, aldrik.ramaekers@protonmail.com
*  All rights reserved.
*/

#include "external/cJSON.h"

static void* validate_license_thread(void *arg)
{
	char params[50];
	sprintf(params, "can_run?ti=%s", license_key);

	char response[MAX_INPUT_LENGTH];
	if (platform_send_http_request("api.aldrik.org", params, response))
	{
		cJSON *result = cJSON_Parse(response);
		if (!result) return false;
		cJSON *canRun = cJSON_GetObjectItem(result, "canRun");
		license_is_valid = canRun->valueint;
	}
	
	return 0;
}

void validate_license()
{
	license_is_valid = true;
	
#ifdef MODE_DEVELOPER
	return;
#endif
	
	thread license_thread = thread_start(validate_license_thread, NULL);
	thread_detach(&license_thread);
}
