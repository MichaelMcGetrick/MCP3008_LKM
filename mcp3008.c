/*   MODULE: mcp33008.c ------------------------------------------------- 
 *   Source file for building a shared library to be used by JNI for
 *   communicationg with the LKM (device driver) for the MCP3008 8-channel
 *   analog to digital converter.   
 ----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>               //Communication facility between kernel and user space
#include <signal.h>
#include <stdbool.h>
#include "mcp3008.h"
									

//Define approrpiate logging function protoptye
//logf will be the alias for both Android App and normal Linux builds
#ifdef ANDROID_APP
	#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "** NATIVE C++ **: ", __VA_ARGS__))
#else
    #define LOGI(...) printf(__VA_ARGS__)
#endif 



#define BUFFER_LENGTH 256               ///< The buffer length for messages 
static char receive[BUFFER_LENGTH] = {0};     ///< The receive buffer from the LKM

int res;
  
//File pointer for file commands  
int fd;

//Flag for logging:
bool logging = true;
  
  
void mcp3008_test(void)
{
	int num = 5;
	char *str = "Hello from mcp3008!";
	
	LOGI("Integer number: %d, String %s\n", num, str);


}//mcp3008_test

char* jni_test(void)
{
	//Test to check we can interface with Java space
	return "This is an updated jni test on 5 April from keypad!\n";
	 	

}//jni_test



//Open the device driver
int init_dev(void)
{
	
   LOGI("Opening the ds3231 device driver\n");
		
   //Open driver:
   fd = open("/dev/ds3231", O_RDWR);             // Open the device with read/write access
   if (fd < 0)
   {
		LOGI("Failed to open the device driver...");
		LOGI("The error number: %d",errno);
		LOGI("Error description: %s",strerror(errno));
				
		return errno;
   }
   LOGI("Opened device driver OK\n");
  	
	
   return 0;
    	
	
}//init_dev


int32_t read_byte(char addr)
{
   //Read value back from kernel driver
   //Define address to read from:
   receive[0] = addr;   
   res = read(fd, receive,BUFFER_LENGTH);
   if (res < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   printf("The received message is: %s\n", receive);
     
}  


int32_t read_data(char* elem)
{
   //Read value back from kernel driver
   //Define address to read from:
   strcpy(receive,elem);
   //res = read(fd, receive,BUFFER_LENGTH);
   res = read(fd, receive,strlen(receive));
   if (res < 0){
      perror("Failed to read the message from the device.");
      return errno;
   }
   //printf("The received message is: %s\n", receive);
   
}  


int32_t write_byte(char elem, char data)
{
  //Read value back from kernel driver
   //Define address to read from:
   char buf[3];
   buf[0] = elem;
   buf[1] = data;
   res = write(fd, buf,strlen(buf));
   if (res < 0){
      perror("Failed to write to the device.");
      return errno;
   }
          
}  



	
//Close the device driver	
void close_dev(void)
{
    LOGI("Closing the ds3231 device driver\n");
	close(fd);
	
}//close_dev
	
	


//---------------------------------------------------------------------
//DEFINE METHODS FOR JAVA CALLBACK HERE (If Android App realisation):
#ifdef ANDROID_APP
void init_callbacks(char *f_name, char *f_sig)
{
	//strcpy(callback_name,f_name);
	callback_name = f_name;
	callback_sig = f_sig;
	LOGI("Set callback strings OK!");
	LOGI("Callback name: %s",callback_name);
	LOGI("Callback signature: %s",callback_sig);
}
	
void sendToJava(char *c)
{
	
	//LOGI("JNIEnv pointer 0x%p",glo_env);
	//LOGI("JVM pointer 0x%p",JVM);
	//LOGI("Global class 0x%p",&glo_clazz);
	//LOGI("Global object 0x%p",&glo_obj);
    
	    
    jstring js = (*glo_env)->NewStringUTF(glo_env,c);
    jclass clazz = (*glo_env)->GetObjectClass(glo_env,glo_obj);
	jmethodID mid_cb = (*glo_env)->GetMethodID(glo_env,clazz,callback_name, callback_sig);
    (*glo_env)->CallVoidMethod(glo_env, glo_obj,mid_cb,js); //OK with this
      
    
	LOGI("Data sent to Java!");
	
	
}	

#endif	
//---------------------------------------------------------------------
