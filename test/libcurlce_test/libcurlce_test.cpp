// libcurlce_test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <curl/curl.h>


struct MemoryStruct 
{
	char *memory;
	size_t size;
};


static size_t WriteMemoryCallback (void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if(mem->memory == NULL) 
	{
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}


/*** for big data ***/
void flushed_printf_to_stdout (char * string, long size)
{
#define MAX_CHARS_STDOUT 128
	long i=0, j=0;
	char *p=string;
	while (i < size)
	{
		fputc (*p, stdout);
		++p;
		++i;
		++j;
		if (j == MAX_CHARS_STDOUT)
		{
			fflush (stdout);
			j=0;
		}
	}
}

int _tmain(void)
{
	CURL *curl_handle;
	CURLcode res;

	struct MemoryStruct chunk;

	curl_global_init(CURL_GLOBAL_ALL);

	// HTTP GET w/ two parameters
	curl_handle = curl_easy_init();
	chunk.memory = (char *)malloc(1); 
	chunk.size = 0;
	curl_easy_setopt(curl_handle, CURLOPT_URL, "https://httpbin.org/get?param1=a&param2=b");
	curl_easy_setopt(curl_handle, CURLOPT_HTTPGET, 1L);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);

	res = curl_easy_perform(curl_handle);
	if(res != CURLE_OK) 
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}
	else 
	{
		fprintf(stdout, "HTTP GET Result:\n");
		fprintf(stdout, "%lu bytes retrieved\n", (long)chunk.size);
		flushed_printf_to_stdout (chunk.memory, (long)chunk.size);
	}

	curl_easy_cleanup(curl_handle);
	if(chunk.memory)
		free(chunk.memory);


	// HTTP POST w/ two parameters
	curl_handle = curl_easy_init();
	chunk.memory = (char *)malloc(1); 
	chunk.size = 0;
	const char * post_data = "param1=a&param2=b";
	curl_easy_setopt(curl_handle, CURLOPT_URL, "https://httpbin.org/post");
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post_data);
	curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, strlen (post_data));
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0L);

	res = curl_easy_perform(curl_handle);
	if(res != CURLE_OK) 
	{
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}
	else 
	{
		fprintf(stdout, "HTTP POST Result:\n");
		fprintf(stdout, "%lu bytes retrieved\n", (long)chunk.size);
		flushed_printf_to_stdout (chunk.memory, (long)chunk.size);
	}

	curl_easy_cleanup(curl_handle);
	if(chunk.memory)
		free(chunk.memory);



	curl_global_cleanup();
	return 0;
}

