# Win-Utsname
Clone of utsname.h library for linux. Made specifically for windows can be used to get various information like virtualization, windows version, etc.
# Examples

* Check basic info
```c
#include <stdio.h>
#include "win_utsname.h"
int main() {
	struct win_utsname sys;
	win_uname(&sys);
	printf("Operating System: %s\n", sys.sysname);
	printf("Hostname: %s\n", sys.nodename);
	printf("Processor Architecture: %s\n", sys.machine);
	printf("Virtualization is Enabled: %d\n", sys.vt_supported);
	printf("Version: %s\n", sys.version);
	printf("Release: %s\n", sys.release);
	printf("full version: %s\n", sys.full_version);
	system("pause");
	return 0;
}
```
* Executing and running
```
gcc <filename> -lregex -lws2_32 -o <output>
./<output>
```
![image](https://user-images.githubusercontent.com/54384337/119207922-b40ac500-ba65-11eb-9862-e2f6dd2c2750.png)
