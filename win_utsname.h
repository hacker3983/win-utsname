/*
   USES WIN32API
   Written By catsanddogs
*/
#include <regex.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

#ifndef platform
#define platform "Windows"
#endif

struct win_utsname {
	char sysname[65];
	char nodename[65];
	char release[65];
	char version[65];
	char win_version[65];
	char machine[65];
	char processor[130];
	char full_version[65];
	bool vt_supported;
};

// Get Operating System Version
void win_getfullversion(struct win_utsname* __sys__) {
	memset(__sys__->full_version, 0, 65);
	int i=0;
	char c;
	FILE* f = popen("ver", "r");
	while((c = fgetc(f)) != EOF) { if(c != '\n') { __sys__->full_version[i] = c; i++; } }
	fclose(f);
}
/* # Examples of VER command output:
   #   Windows 2000:  Microsoft Windows 2000 [Version 5.00.2195]
   #   Windows XP:    Microsoft Windows XP [Version 5.1.2600]
   #   Windows Vista: Microsoft Windows [Version 6.0.6002]
   #
   # Note that the "Version" string gets localized on different
   # Windows versions.
*/

void win_getversion(struct win_utsname* __sys__) {
	win_getfullversion(__sys__);
	memset(__sys__->version, 0, 65);
	regex_t preg;
	char* pattern = "[0-9]+.[0-9]+.[0-9]+.[0-9]+", *string = __sys__->full_version;
	int x=0, size = strlen(string);
	regmatch_t pmatch[size];
	regcomp(&preg, pattern, REG_EXTENDED);
	regexec(&preg, string, size+1, pmatch, REG_NOTBOL);
	for(int i=pmatch[0].rm_so;i<pmatch[0].rm_eo;i++) { __sys__->version[x] = string[i]; x++; }
	regfree(&preg);
}

/* Gets windows release version
   Examples:
   Version 21H1
   Version 20H1
   Version 2004
   Version 1909
*/
void win_getwinversion(struct win_utsname* __sys__) {
	HKEY hkey;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_QUERY_VALUE, &hkey);
	memset(__sys__->win_version, 0, 65);
	long unsigned int size = 65;
	RegQueryValueEx(hkey, "DisplayVersion", NULL, NULL, __sys__->win_version, &size);
	RegCloseKey(hkey);
}

// Get windows release stores it in win_utsname structure
void win_getrelease(struct win_utsname* __sys__) {
	win_getfullversion(__sys__);
	regex_t preg;
	int x=0;
	char* pattern = "[0-9]+", *string = __sys__->full_version;
	memset(__sys__->release, 0, 65);
	regmatch_t pmatch[strlen(string)];
	regcomp(&preg, pattern, REG_EXTENDED);
	regexec(&preg, string, strlen(string)+1, pmatch, REG_NOTBOL);
	for(int i=pmatch[0].rm_so;i<pmatch[0].rm_eo;i++) { __sys__->release[x] = string[i]; x++; }
	regfree(&preg);
}

/* Virtualization supported
   checks if virtualization is enabled and accessible by the operating system
   returns true if enabled otherwise 0
*/
bool win_vtsupported() {
	switch(IsProcessorFeaturePresent(PF_VIRT_FIRMWARE_ENABLED)) {
		case true: return true;
		default: return false;
	}
}

/* Detecting Processor Architecture
   From: https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/ns-sysinfoapi-system_info
   PROCESSOR_ARCHITECTURE_AMD64 (9)
   PROCESSOR_ARCHITECTURE_ARM (5)
   PROCESSOR_ARCHITECTURE_ARM64 (12)
   PROCESSOR_ARCHITECTURE_IA64 (6)
   PROCESSOR_ARCHITECTURE_INTEL (0)
   PROCESSOR_ARCHITECTURE_UNKNOWN(0xffff or 65535)
*/
void win_getarchitecture(struct win_utsname* __sys__) {
	SYSTEM_INFO sys;
	GetNativeSystemInfo(&sys);
	switch(sys.wProcessorArchitecture) {
		case PROCESSOR_ARCHITECTURE_AMD64: strcpy(__sys__->machine, "AMD64"); break;
		case PROCESSOR_ARCHITECTURE_ARM: strcpy(__sys__->machine, "ARM32"); break;
		#ifdef PROCESSOR_ARCHITECTURE_ARM64
		case PROCESSOR_ARCHITECTURE_ARM64: strcpy(__sys__->machine, "ARM64"); break;
		#endif
		case PROCESSOR_ARCHITECTURE_IA64: strcpy(__sys__->machine, "IA-64"); break;
		case PROCESSOR_ARCHITECTURE_INTEL: strcpy(__sys__->machine, "x86"); break;
		default: strcpy(__sys__->machine, "UNKNOWN");
	}
}
// Gets Processor name
void win_getprocessor(struct win_utsname* __sys__) {
	HKEY hkey;
	long unsigned int size = 130;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment", 0, KEY_QUERY_VALUE, &hkey);
	RegQueryValueEx(hkey, "PROCESSOR_IDENTIFIER", NULL, NULL, __sys__->processor, &size);
	RegCloseKey(hkey);
}
// Gets system information stores information in structure win_utsname
void win_uname(struct win_utsname* __sys__) {
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { puts("failed to start winsock"); }
	win_getarchitecture(__sys__);
	win_getfullversion(__sys__);
	win_getversion(__sys__);
	win_getrelease(__sys__);
	win_getwinversion(__sys__);
	win_getprocessor(__sys__);
	gethostname(__sys__->nodename, 256);
	strcpy(__sys__->sysname, platform);
	__sys__->vt_supported = win_vtsupported();
	WSACleanup();
}
