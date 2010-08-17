/* {{{ winapi_get_module - get the hmodule of something we've already loaded - ewww I know
       I hate windows - TODO: cache this stuff a bit!*/
HMODULE winapi_get_module(const char* dllname)
{
	/* Decidedly evil hack to be able to GetProcAddress with NULL handle properly */
	HANDLE cur_proc;
	HMODULE modules[1024];
	HMODULE found = NULL;
	DWORD needed, i;

	/* yes, this makes this require psapi.dll, I don't care */
	cur_proc = GetCurrentProcess();
	/* ok, alloc away, we're going to free this in a bit */
#ifdef _WIN64
	EnumProcessModulesEx(cur_proc, modules, sizeof(modules), &needed, LIST_MODULES_ALL);
#else
	EnumProcessModules(cur_proc, modules, sizeof(modules), &needed);
#endif
	for (i=0; i < needed/sizeof(HMODULE); i++)
	{
		TCHAR   module_path[MAX_PATH];
		DWORD worked;
		/* get the base name for each item, does it match what we want */
		worked = GetModuleBaseName(cur_proc, modules[i], module_path, MAX_PATH);
		if(worked && _stricmp(module_path, dllname) == 0) {
			found = modules[i];
			break;
		}		
	}
	return found;
}

/* {{{ win_is_xp - are we running on xp or higher (some things won't work unless we are) */
BOOL winapi_is_xp () 
{
   OSVERSIONINFOEX version_info;
   DWORDLONG check_mask = 0;

   ZeroMemory(&version_info, sizeof(OSVERSIONINFOEX));
   version_info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
   version_info.dwMajorVersion = 5;
   version_info.dwMinorVersion = 1;

   VER_SET_CONDITION( check_mask, VER_MAJORVERSION, VER_GREATER_EQUAL);
   VER_SET_CONDITION( check_mask, VER_MINORVERSION, VER_GREATER_EQUAL);

   return VerifyVersionInfo(
      &version_info, 
      VER_MAJORVERSION | VER_MINORVERSION,
      check_mask);
}
/* }}} */

/* Get the version of the dll currently loaded, I hate windows */
char* php_winapi_getdllversion(const char* dllname)
{
	/* Decidedly evil hack to be able to GetProcAddress with NULL handle properly */
	HANDLE cur_proc;
	HMODULE modules[1024];
	DWORD needed, i;
	char* minor = emalloc(sizeof(char*));
    char* version = emalloc(sizeof(char*));

	/* yes, this makes this require psapi.dll, I don't care */
	cur_proc = GetCurrentProcess();
	/* ok, alloc away, we're going to free this in a bit */
	EnumProcessModules(cur_proc, modules, sizeof(modules), &needed);
	for (i=0; i < needed/sizeof(HMODULE); i++)
	{
			TCHAR   szModulePath[MAX_PATH];
			DWORD worked;
			/* get the base name for each item, does it match what we want */
			worked = GetModuleBaseName(cur_proc, modules[i], szModulePath, MAX_PATH);
			if(worked && strcmp(szModulePath, dllname) == 0) {
				DLLGETVERSIONPROC pDllGetVersion;
				pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress(modules[i], "DllGetVersion");
				if(pDllGetVersion) {

					DLLVERSIONINFO info = {0};
					HRESULT result;

					info.cbSize = sizeof(info);

					result = (*pDllGetVersion)(&info);
					//php_printf("%d . %d", info.dwMajorVersion, info.dwMinorVersion);

					if(SUCCEEDED(result)) {
						_ltoa(info.dwMajorVersion, version, 10);
						_ltoa(info.dwMinorVersion, minor, 10);
						strcat(version, ".");
						strcat(version, minor);
					}
				}
				
				break;
			} else {
				version = "Unknown";
			}
	}
	efree(minor);
	free(modules);
	return version;
}