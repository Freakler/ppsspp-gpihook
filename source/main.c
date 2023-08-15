#include <pspsdk.h>
#include <stdio.h>
#include <string.h>

#include "minIni.h"

#define LOG

PSP_MODULE_INFO("gpihook", 0x1007, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

//////////////////////////////////////

#ifdef LOG
int debugPrintf(const char *text, ...) { // if USE_PSPSDK_LIBC
  va_list list;
  char string[256];

  va_start(list, text);
  vsprintf(string, text, list);
  va_end(list);

  SceUID fd = sceIoOpen("ms0:/PSP/PLUGINS/gpihook/log.txt", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_APPEND, 0777);
  if( fd >= 0 ) {
    sceIoWrite(fd, string, strlen(string));
    sceIoWrite(fd, "\n", 1);
    sceIoClose(fd);
  }
   
  return 0;
}
#endif  

//////////////////////////////////////

#define MAKE_JUMP(a, f) _sw(0x08000000 | (((u32)(f) & 0x0FFFFFFC) >> 2), a);

#define HIJACK_FUNCTION(a, f, ptr) { \
  u32 _func_ = a; \
  static u32 patch_buffer[3]; \
  _sw(_lw(_func_), (u32)patch_buffer); \
  _sw(_lw(_func_ + 4), (u32)patch_buffer + 8);\
  MAKE_JUMP((u32)patch_buffer + 4, _func_ + 8); \
  _sw(0x08000000 | (((u32)(f) >> 2) & 0x03FFFFFF), _func_); \
  _sw(0, _func_ + 4); \
  ptr = (void *)patch_buffer; \
}

//////////////////////////////////////

int ValidUserAddress(void *addr) {
  return ((u32)addr >= 0x08800000 && (u32)addr < 0x0A000000);
}

u32 FindImport(char *libname, u32 nid) {
  #ifdef LOG
  debugPrintf("FindImport()");
  #endif  
  
  u32 lower = 0x08800000;
  u32 higher = 0x09000000;
  
  u32 i;

  for( i = lower; i < higher; i += 4 ) {
    SceLibraryStubTable *stub = (SceLibraryStubTable *)i;

    if( ValidUserAddress((void *)stub->libname) && ValidUserAddress(stub->nidtable) && ValidUserAddress(stub->stubtable) ) {
      #ifdef LOG
      //debugPrintf("found possible stub table '%s' at 0x%08X", stub->libname, i);
      #endif  
      if( strcmp(libname, stub->libname) == 0 ) {
        #ifdef LOG
        debugPrintf("> found %s at 0x%08X", stub->libname, i);
        #endif  
        u32 *table = stub->nidtable;
        int j;
        #ifdef LOG
        debugPrintf("> stubcount = %i", stub->stubcount);
        #endif  
        for( j = 0; j < stub->stubcount; j++ ) {
          #ifdef LOG
          debugPrintf("> found nid 0x%08X", table[j]);
          #endif  
          if( table[j] == nid ) {
            #ifdef LOG
            debugPrintf("> match at index %i!", j);
            #endif  
            return ((u32)stub->stubtable + (j * 8));
          }
        } 
      }
    }
  }

  return 0;
}

//////////////////////////////////////

unsigned char (*_sceKernelGetGPI)(void) = NULL;

unsigned char sceKernelGetGPI_Patched(void) {
  static int time = 0;
  static unsigned char val = 0;
  
  /// only re-check ini every 1 second(s)
  if( sceKernelGetSystemTimeLow() >= time + 1000000 ) { // 1 second
    #ifdef LOG
    debugPrintf("[%i] reading config.ini", sceKernelGetSystemTimeLow());
    #endif
    int i;
    char str[8];
    val = 0b00000000; // 
    for( i = 0; i < 8; i++ ) {
      sprintf(str, "%i", i);
      if( ini_getbool("GPI", str, 0, "ms0:/PSP/PLUGINS/gpihook/config.ini") ) { // pin is ON
        val |= 1 << i; // set bit at position i
      }
    } time = sceKernelGetSystemTimeLow();
  }
  
  #ifdef LOG
  debugPrintf("[%i] sceKernelGetGPI() = 0x%02X", sceKernelGetSystemTimeLow(), val);
  #endif
  
  return val; // 0xFF for all pins ON
}

//////////////////////////////////////

int module_start(SceSize args, void *argp) {
  #ifdef LOG
  sceIoRemove("ms0:/PSP/PLUGINS/gpihook/log.txt");
  debugPrintf("module_start()");
  #endif
  
  // sceKernelDelayThread(1000000); // 1s

  /// work
  _sceKernelGetGPI = (void *)FindImport("UtilsForUser", 0x37fb5c42);
  #ifdef LOG
  debugPrintf("_sceKernelGetGPI at 0x%08X", (u32)_sceKernelGetGPI);
  #endif
  if( _sceKernelGetGPI != 0 ) {
    HIJACK_FUNCTION((u32)_sceKernelGetGPI, sceKernelGetGPI_Patched, _sceKernelGetGPI);
    #ifdef LOG
    debugPrintf("hooked");
    #endif
  }
  
  #ifdef LOG
  debugPrintf("all done");
  #endif
  
  sceKernelDcacheWritebackAll();
  sceKernelIcacheClearAll();
  return 0;
}
