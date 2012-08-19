#include <stdio.h>
#include <stdarg.h>
#include <memory.h>

typedef unsigned int uint32;
typedef signed char int8;

#define JAKE_FOURCC(a,b,c,d) ( (uint32)(((d)<<24) | ((c)<<16) | ((b)<<8) | (a)) )
#define JAKE_FCC(x) \
    ((uint32)((((uint32)(x) & 0x000000FF) << 24) | (((uint32)(x) & 0x0000FF00) << 8) | \
    (((uint32)(x) & 0x00FF0000) >> 8) | (((uint32)(x) & 0xFF000000) >> 24)))

struct FilterInfo
{
	enum VerbosityLocation { 
		TTY = 0,
		FILE = 1,
		SCREEN = 2,
		RESERVED = 3
	};

	uint32 filter;
	int8 verbosity[4];
};

#define MAX_NUM_FILTERS (8)
static FilterInfo s_filters[MAX_NUM_FILTERS];
static uint32 s_numFilters = 0;
#define DEFAULT_FILTER (0)

FilterInfo* LogFindFilter(const uint32 filter)
{
	FilterInfo* pFilter = s_filters;
	const uint32 numFilters = s_numFilters;
	for (uint32 i = 0; i < numFilters; i++)
	{
		if (pFilter->filter == filter)
		{
			return pFilter;
		}
		pFilter++;
	}
	return NULL;
}

static void LogSetFilterVerbosity(const uint32 filter, const int8 verbosity, const int index)
{
	FilterInfo* pFilterInfo = LogFindFilter(filter);
	if (pFilterInfo == NULL)
	{
		pFilterInfo = &(s_filters[s_numFilters]);
		s_numFilters++;
		pFilterInfo->filter = filter;
		//printf("New Filter '%.4s'\n", (char*)&filter);
	}
	pFilterInfo->verbosity[index] = verbosity;
}

void LogSetFilterTTYVerbosity(const uint32 filter, const int8 verbosity)
{
	LogSetFilterVerbosity(filter, verbosity, FilterInfo::TTY);
}

void LogSetFilterFileVerbosity(const uint32 filter, const int8 verbosity)
{
	LogSetFilterVerbosity(filter, verbosity, FilterInfo::FILE);
}
void LogSetFilterScreenVerbosity(const uint32 filter, const int8 verbosity)
{
	LogSetFilterVerbosity(filter, verbosity, FilterInfo::SCREEN);
}

void LogInit()
{
	memset(s_filters, sizeof(s_filters), 0);
	s_numFilters = 0;
	LogSetFilterTTYVerbosity(DEFAULT_FILTER, 0);
	LogSetFilterFileVerbosity(DEFAULT_FILTER, 0);
	LogSetFilterScreenVerbosity(DEFAULT_FILTER, 0);
}

static void LogInternal(const uint32 filter, const char* const message)
{
	const FilterInfo* pFilter = s_filters;
	const FilterInfo* pFoundFilter = pFilter;
	const uint32 numFilters = s_numFilters;
	for (uint32 i = 0; i < numFilters; i++)
	{
		if (pFilter->filter == filter)
		{
			pFoundFilter = pFilter;
			break;
		}
		pFilter++;
	}

	//printf("Found Filter '%.4s' 0x%X TTY:%d\n", (char*)&(pFoundFilter->filter), pFoundFilter->filter, pFoundFilter->verbosity[FilterInfo::TTY]);
	if (pFoundFilter->verbosity[FilterInfo::TTY] > 0)
	{
		printf("'%.4s' %s\n", (char*)&filter, message);
	}
}

void Log(const uint32 filter, const char* const fmt, ...)
{
	char outputBuffer[2048];
	va_list argPtr;
	va_start(argPtr, fmt);
	vsnprintf(outputBuffer, sizeof(outputBuffer), fmt, argPtr);
	va_end(argPtr);

	LogInternal(filter, outputBuffer);
}

void Log(const char* const fmt, ...)
{
	char outputBuffer[2048];
	va_list argPtr;
	va_start(argPtr, fmt);
	vsnprintf(outputBuffer, sizeof(outputBuffer), fmt, argPtr);
	va_end(argPtr);

	uint32 filter = 0;
	if ((outputBuffer[0] == '[') && (outputBuffer[5] == ']'))
	{
		filter = (outputBuffer[1]) | (outputBuffer[2] << 8) | (outputBuffer[3] <<16) | (outputBuffer[4] << 24);
	}
	LogInternal(filter, outputBuffer);
}

int main(int argc, char* argv[])
{
	printf("argc:%d\n", argc);
	for (int i = 0; i < argc; i++)
	{
		printf("argv[%d] '%s'\n", i, argv[i]);
	}
	LogInit();

	LogSetFilterTTYVerbosity(JAKE_FCC('JAKE'), 1);
	Log(JAKE_FOURCC('J', 'A', 'K', 'E'), "argc:%d J A K E", argc);
	Log(JAKE_FCC('JAKE'), "argc:%d JAKE", argc);
	Log(JAKE_FCC('GAME'), "argc:%d NO GAME", argc);
	Log("[JAKE] argc:%d [JAKE] JAKE", argc);
	LogSetFilterTTYVerbosity(JAKE_FCC('GAME'), 1);
	LogSetFilterTTYVerbosity(JAKE_FCC('JAKE'), 0);
	Log(JAKE_FCC('JAKE'), "argc:%d NO JAKE", argc);
	Log(JAKE_FCC('GAME'), "argc:%d GAME", argc);
	Log("[GAME] argc:%d [GAME] GAME", argc);
	return 0;
}
