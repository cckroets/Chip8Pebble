#pragma once

#include <pebble.h>

#define VERBOSE APP_LOG_LEVEL_DEBUG_VERBOSE
#define DEBUG 	APP_LOG_LEVEL_DEBUG
#define INFO 	  APP_LOG_LEVEL_INFO
#define WARN	  APP_LOG_LEVEL_WARNING
#define ERROR	  APP_LOG_LEVEL_ERROR

void log_name(const char* s) {
	APP_LOG(VERBOSE, s);
}

void log_instrAddr(const char* s, int addr) {
	APP_LOG(VERBOSE, "%s %d", s, addr);
}

void log_instrVRegAddr(const char* s, int reg, int addr) {
	APP_LOG(VERBOSE, "%s V%d, %d", s, reg, addr);
}

void log_instrRegAddr(const char* s, const char* reg, int addr) {
	APP_LOG(VERBOSE, "%s %s, %d", s, reg, addr);
}

void log_instrVRegVReg(const char* s, int r1, int r2) {
	APP_LOG(VERBOSE, "%s V%d, V%d", s, r1, r2);
}

void log_instrVReg(const char* s, int r1) {
	APP_LOG(VERBOSE, "%s V%d", s, r1);
}

void log_instrVRegReg(const char* s, int r1, const char* r2) {
	APP_LOG(VERBOSE, "%s V%d, %s", s, r1, r2);
}

void log_instrRegVReg(const char* s, const char* r1, int r2) {
	APP_LOG(VERBOSE, "%s %s, V%d", s, r1, r2);
}

void log_instrVRegVRegAddr(const char* s, int r1, int r2, int n) {
	APP_LOG(VERBOSE, "%s V%d, V%d, %d", s, r1, r2, n);
}
