//-------------------------------------------------------------------------------
//  TinyCircuits TinyScreen/ST BLE ANCS Smartwatch Example
//  Last Updated 26 October 2017
//
//  This demo sets up the ST BLE for the Apple Notification Center Service as well
//  as the Current Time service. Now with TinyScreen+ compatibity.
//  Requires updated STBLE library or will not compile!
//
//  2.0.0 26 Oct 2017 Initial update release
//
//  Written by Ben Rose, TinyCircuits http://TinyCircuits.com
//
//-------------------------------------------------------------------------------

// Color definitions
#define  BLACK           0x00
#define BLUE            0xE0
#define RED             0x03
#define GREEN           0x1C
#define DGREEN           0x0C
#define YELLOW          0x1F
#define WHITE           0xFF
#define ALPHA           0xFE
#define BROWN           0x32

#include <SPI.h>
#include <Wire.h>
#include <TinyScreen.h>
#include <STBLE.h>
#include "BLEtypes.h"

#include "BMA250.h"       // For interfacing with the accel. sensor

#define BLE_DEBUG true
#define menu_debug_print true

uint32_t doVibrate = 0;

// Accelerometer sensor variables for the sensor and its values
BMA250 accel_sensor;
int x, y, z;
double temp, acc;
bool temperature_noti = false;
int outOfTempSelection = 1;
int outOfFlappyBirdSelection = 1;
int tempCode = 100;

#if defined (ARDUINO_ARCH_AVR)
TinyScreen display = TinyScreen(0);
#define SerialMonitorInterface Serial
#include <TimeLib.h>

#elif defined(ARDUINO_ARCH_SAMD)
TinyScreen display = TinyScreen(0);
#define SerialMonitorInterface SerialUSB
#include <RTCZero.h>
#include <time.h>
RTCZero RTCZ;
uint32_t startTime = 0;
uint32_t sleepTime = 0;
unsigned long millisOffsetCount = 0;


unsigned char flappyBird[]={
  ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,
  ALPHA,ALPHA,ALPHA,ALPHA,BLACK,BLACK,WHITE,WHITE,WHITE,BLACK,WHITE,WHITE,BLACK,ALPHA,ALPHA,ALPHA,ALPHA,
  ALPHA,ALPHA,ALPHA,BLACK,WHITE,WHITE,YELLOW,YELLOW,BLACK,WHITE,WHITE,WHITE,WHITE,BLACK,ALPHA,ALPHA,ALPHA,
  ALPHA,ALPHA,BLACK,WHITE,YELLOW,YELLOW,YELLOW,YELLOW,BLACK,WHITE,WHITE,WHITE,BLACK,WHITE,BLACK,ALPHA,ALPHA,
  ALPHA,BLACK,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,BLACK,WHITE,WHITE,WHITE,BLACK,WHITE,BLACK,ALPHA,ALPHA,
  ALPHA,BLACK,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,BLACK,WHITE,WHITE,WHITE,WHITE,BLACK,ALPHA,ALPHA,
  ALPHA,BLACK,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,ALPHA,
  ALPHA,BLACK,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,BLACK,RED,RED,RED,RED,RED,RED,BLACK,
  ALPHA,ALPHA,BLACK,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,BLACK,RED,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
  ALPHA,ALPHA,BLACK,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,BLACK,RED,RED,RED,RED,RED,BLACK,ALPHA,
  ALPHA,ALPHA,ALPHA,BLACK,BLACK,YELLOW,YELLOW,YELLOW,YELLOW,YELLOW,BLACK,BLACK,BLACK,BLACK,BLACK,ALPHA,ALPHA,
  ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,BLACK,BLACK,BLACK,BLACK,BLACK,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA
};
//7x8
unsigned char wingUp[]={
  ALPHA,BLACK,BLACK,BLACK,BLACK,ALPHA,ALPHA,
  BLACK,WHITE,WHITE,WHITE,WHITE,BLACK,ALPHA,
  BLACK,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,
  BLACK,YELLOW,WHITE,WHITE,WHITE,YELLOW,BLACK,
  ALPHA,BLACK,YELLOW,YELLOW,YELLOW,BLACK,ALPHA,
  ALPHA,ALPHA,BLACK,BLACK,BLACK,ALPHA,ALPHA,
  ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,
  ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,
};
unsigned char wingMid[]={
  ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,
  ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,
  ALPHA,BLACK,BLACK,BLACK,BLACK,BLACK,ALPHA,
  BLACK,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,
  BLACK,YELLOW,WHITE,WHITE,WHITE,YELLOW,BLACK,
  ALPHA,BLACK,BLACK,BLACK,BLACK,BLACK,ALPHA,
  ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,
  ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,
};
unsigned char wingDown[]={
  ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,
  ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,
  ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,ALPHA,
  ALPHA,BLACK,BLACK,BLACK,BLACK,BLACK,ALPHA,
  BLACK,YELLOW,WHITE,WHITE,WHITE,YELLOW,BLACK,
  BLACK,WHITE,WHITE,WHITE,WHITE,BLACK,ALPHA,
  BLACK,WHITE,WHITE,YELLOW,BLACK,ALPHA,ALPHA,
  ALPHA,BLACK,BLACK,BLACK,BLACK,ALPHA,ALPHA,
};

unsigned char codeNormalEmoji[] = {
  0x24,0x49,0x24,0x29,0x48,0x04,0x05,0x0d,0x0d,0x11,0x29,0x04,0x29,0x29,0x48,0x49,0x48,
  0x24,0x49,0x28,0x25,0x28,0x52,0x76,0x7b,0x7b,0x7b,0x76,0x52,0x25,0x01,0x24,0x49,0x48,
  0x44,0x49,0x28,0x29,0x7b,0x7b,0x7f,0x5b,0x5b,0x57,0x57,0x5b,0x77,0x4e,0x04,0x25,0x44,
  0x44,0x24,0x2d,0x9b,0x7b,0x7f,0x5f,0x5b,0x57,0x5b,0x37,0x37,0x57,0x7b,0x52,0x24,0x25,
  0x24,0x05,0x7b,0x7b,0x7b,0x7f,0x57,0x57,0x57,0x5b,0x5b,0x5b,0x7b,0x57,0x5b,0x0d,0x04,
  0x00,0x4e,0x7b,0x5f,0x7b,0x32,0x0e,0x5b,0x57,0x5b,0x33,0x0e,0x57,0x57,0x5b,0x53,0x04,
  0x01,0x76,0x77,0x5b,0x77,0x09,0x09,0x33,0x7b,0x5b,0x0e,0x05,0x2e,0x5b,0x57,0x5b,0x2e,
  0x09,0x7b,0x57,0x5b,0x5b,0x0e,0x05,0x57,0x37,0x5b,0x32,0x05,0x57,0x5b,0x37,0x5b,0x57,
  0x09,0x77,0x5b,0x57,0x5b,0x5b,0x37,0x5b,0x57,0x5b,0x57,0x37,0x57,0x5b,0x57,0x57,0x57,
  0x09,0x7b,0x57,0x5b,0x5b,0x5b,0x57,0x37,0x57,0x3b,0x57,0x5b,0x7b,0x57,0x57,0x5b,0x57,
  0x05,0x77,0x7b,0x37,0x5b,0x57,0x57,0x7b,0x7b,0x5b,0x57,0x57,0x37,0x5b,0x5b,0x57,0x32,
  0x00,0x4e,0x7b,0x5b,0x5b,0x37,0x09,0x12,0x33,0x0e,0x0e,0x0e,0x57,0x57,0x5b,0x77,0x05,
  0x24,0x29,0x77,0x57,0x5f,0x5f,0x32,0x0a,0x09,0x0a,0x0e,0x57,0x5b,0x57,0x7b,0x4e,0x00,
  0x45,0x04,0x2e,0x9b,0x5b,0x5b,0x7b,0x57,0x57,0x57,0x5b,0x5b,0x57,0x7b,0x53,0x04,0x25,
  0x24,0x25,0x24,0x52,0x7b,0x7b,0x7b,0x5b,0x57,0x57,0x57,0x5b,0x7b,0x77,0x04,0x25,0x24,
  0x24,0x25,0x48,0x29,0x2d,0x56,0x9b,0x7b,0x5b,0x5b,0x5b,0x77,0x2d,0x05,0x24,0x25,0x24,
  0x48,0x49,0x48,0x49,0x24,0x04,0x09,0x2e,0x52,0x2e,0x29,0x05,0x04,0x25,0x48,0x25,0x44,
};

unsigned char codeGreenEmoji[] = {
  0x44,0x49,0x24,0x49,0x28,0x04,0x09,0x2d,0x2e,0x11,0x09,0x04,0x25,0x28,0x45,0x48,0x45,
  0x24,0x49,0x48,0x25,0x28,0x72,0x77,0x7b,0x9b,0x7b,0x76,0x4e,0x05,0x20,0x25,0x48,0x25,
  0x48,0x49,0x24,0x2d,0x7b,0x7b,0x7f,0x5b,0x5b,0x77,0x57,0x7b,0x77,0x09,0x24,0x25,0x44,
  0x24,0x29,0x29,0x9b,0x5f,0x5b,0x5b,0x57,0x57,0x37,0x5b,0x5b,0x7b,0x77,0x29,0x24,0x25,
  0x24,0x25,0x76,0x7f,0x5b,0x5b,0x37,0x5b,0x57,0x5b,0x37,0x57,0x57,0x7b,0x72,0x00,0x25,
  0x00,0x4d,0x9f,0x5b,0x5b,0x12,0x09,0x5b,0x7b,0x57,0x09,0x0e,0x5b,0x37,0x9b,0x29,0x24,
  0x00,0x4e,0x7b,0x5b,0x5b,0x0e,0x05,0x57,0x7b,0x57,0x05,0x0a,0x5b,0x57,0x7b,0x4e,0x00,
  0x00,0x76,0x7b,0x5b,0x7b,0x37,0x33,0x5b,0x57,0x77,0x33,0x52,0x57,0x57,0x77,0x52,0x04,
  0x04,0x72,0x7b,0x5b,0x5b,0x5f,0x5b,0x57,0x5b,0x5b,0x5b,0x5b,0x5b,0x57,0x5b,0x52,0x00,
  0x04,0x4e,0x7b,0x5b,0x5b,0x5b,0x5b,0x57,0x37,0x57,0x5b,0x5b,0x37,0x57,0x77,0x2e,0x04,
  0x24,0x29,0x7b,0x5b,0x7b,0x5f,0x0e,0x05,0x09,0x09,0x0a,0x5b,0x5b,0x5b,0x9b,0x09,0x24,
  0x24,0x05,0x52,0x7f,0x7b,0x37,0x32,0x57,0x57,0x57,0x0e,0x33,0x77,0x7b,0x52,0x00,0x45,
  0x24,0x25,0x09,0x7b,0x7f,0x5b,0x5b,0x57,0x57,0x57,0x5b,0x57,0x5b,0x77,0x04,0x25,0x44,
  0x24,0x45,0x29,0x29,0x56,0x7f,0x77,0x5b,0x7b,0x5b,0x7b,0x7b,0x32,0x05,0x24,0x25,0x24,
  0x44,0x49,0x48,0x49,0x28,0x2d,0x76,0x77,0x7b,0x7a,0x56,0x29,0x01,0x24,0x29,0x44,0x49,
  0x48,0x49,0x48,0x49,0x44,0x25,0x04,0x05,0x05,0x04,0x05,0x04,0x25,0x48,0x24,0x45,0x44,
};

unsigned char codeYellowEmoji[] = {
  0x44,0x25,0x48,0x29,0x48,0x25,0x04,0x09,0x09,0x08,0x25,0x24,0x24,0x49,0x28,0x25,0x4c,
  0x48,0x49,0x28,0x25,0x28,0x2d,0x52,0x77,0x7a,0x7b,0x72,0x4d,0x05,0x04,0x25,0x48,0x25,
  0x24,0x49,0x28,0x09,0x76,0x9b,0x7f,0x7b,0x5b,0x7b,0x7b,0x7b,0x73,0x09,0x04,0x45,0x24,
  0x44,0x29,0x04,0x77,0x7f,0x7b,0x5b,0x5b,0x57,0x37,0x57,0x57,0x7b,0x77,0x25,0x24,0x25,
  0x44,0x21,0x52,0x9f,0x7b,0x5b,0x09,0x5b,0x57,0x57,0x0e,0x32,0x5b,0x9b,0x77,0x24,0x29,
  0x24,0x29,0x9b,0x37,0x2e,0x11,0x37,0x57,0x57,0x5b,0x57,0x0a,0x0e,0x0e,0x9b,0x29,0x24,
  0x00,0x52,0x7b,0x5b,0x57,0x2e,0x12,0x57,0x37,0x57,0x57,0x0e,0x33,0x37,0x57,0x52,0x00,
  0x00,0x4e,0x7b,0x7b,0x7b,0x09,0x05,0x5b,0x57,0x5f,0x0e,0x05,0x57,0x5b,0x5b,0x73,0x04,
  0x00,0x52,0x7b,0x77,0x7b,0x0e,0x0a,0x5b,0x57,0x57,0x2e,0x09,0x57,0x57,0x7b,0x7b,0x04,
  0x00,0x72,0x9b,0x76,0x9b,0x7b,0x57,0x57,0x5b,0x57,0x57,0x57,0x5b,0x7b,0x57,0x77,0x04,
  0x04,0x6d,0xb6,0x92,0x7a,0x7f,0x7b,0x33,0x2e,0x32,0x5b,0x57,0x57,0x57,0x7f,0x2e,0x04,
  0x24,0x69,0xd5,0xd6,0x96,0x9b,0x36,0x09,0x05,0x05,0x0e,0x5f,0x5b,0x7f,0x9b,0x09,0x24,
  0x24,0x8d,0xd5,0xd6,0xb6,0x9a,0x57,0x37,0x57,0x37,0x33,0x57,0x57,0x77,0x29,0x20,0x25,
  0x20,0xb1,0xfa,0xd5,0xb6,0xbb,0x7b,0x7b,0x5b,0x57,0x77,0x7b,0x7b,0x2e,0x04,0x25,0x24,
  0x44,0x48,0xd2,0xd6,0x91,0x76,0x9b,0x77,0x7b,0x7b,0x77,0x73,0x2d,0x05,0x24,0x41,0x44,
  0x24,0x48,0x48,0x44,0x28,0x29,0x0d,0x31,0x52,0x2d,0x29,0x05,0x04,0x25,0x48,0x45,0x48,
  0xb6,0x92,0xb6,0x92,0x92,0x92,0x92,0x71,0x6e,0x72,0x92,0x92,0x92,0x92,0x92,0x92,0x92,
};

unsigned char codeRedEmoji[] = {
  0x24,0x48,0x45,0x24,0x49,0x48,0x24,0x24,0x24,0x28,0x28,0x4c,0x49,0x28,0x49,0x28,0x49,
  0x48,0x48,0x45,0x48,0x24,0x28,0x6d,0x91,0x96,0x91,0x95,0x49,0x24,0x44,0x29,0x44,0x49,
  0x28,0x49,0x48,0x45,0x91,0xda,0xdb,0xfe,0xff,0xff,0xfa,0xb6,0xb2,0x45,0x24,0x25,0x24,
  0x24,0x49,0x44,0xb2,0xff,0xff,0xfe,0xfa,0xfb,0xd6,0xdb,0xff,0xfb,0xb6,0x44,0x21,0x24,
  0x44,0x28,0x92,0xff,0xff,0x9a,0x8d,0xfb,0xd6,0xdb,0xb6,0x6d,0xdb,0xfb,0xb6,0x48,0x25,
  0x40,0x25,0xde,0xba,0x71,0x6d,0x96,0xbb,0xbf,0xbe,0xbf,0x6e,0x49,0x72,0xdb,0x6d,0x24,
  0x24,0x6d,0xfb,0x9a,0x92,0x7b,0x7b,0x7b,0x7b,0x77,0x77,0x73,0x72,0x72,0xd7,0xd6,0x20,
  0x28,0xb6,0xbb,0xbb,0x7b,0x09,0x0e,0x5b,0x57,0x77,0x53,0x05,0x0e,0xbb,0xb7,0xda,0x45,
  0x44,0xb7,0x9b,0x7b,0x56,0x05,0x0a,0x5b,0x57,0x7b,0x53,0x05,0x0e,0x7b,0x97,0xbb,0x45,
  0x24,0xb7,0x97,0x77,0x7f,0x0e,0x0e,0x57,0x57,0x57,0x77,0x0e,0x37,0x5b,0x7b,0x9b,0x25,
  0x00,0x92,0xb6,0x92,0x9b,0x7b,0x5b,0x5b,0x57,0x77,0x5b,0x57,0x5b,0x57,0x5b,0x77,0x24,
  0x04,0x49,0xb2,0xb6,0x96,0x9b,0x57,0x0a,0x09,0x0a,0x12,0x57,0x5b,0x5b,0x77,0x56,0x04,
  0x24,0x6d,0xd5,0xb6,0x96,0xbb,0x36,0x0a,0x0a,0x09,0x0e,0x37,0x57,0x5b,0x9b,0x29,0x24,
  0x24,0xb1,0xd6,0xd9,0xb6,0x9a,0x7b,0x5b,0x5b,0x7b,0x5b,0x5b,0x5b,0x5b,0x4e,0x04,0x25,
  0x24,0xad,0xf6,0xd6,0xba,0xbf,0x7b,0x57,0x57,0x57,0x57,0x7b,0x7b,0x52,0x04,0x25,0x24,
  0x24,0x48,0x8d,0x91,0x4d,0x55,0x7b,0x7b,0x5b,0x7b,0x57,0x2e,0x29,0x05,0x24,0x25,0x44,
  0x44,0x49,0x24,0x44,0x24,0x24,0x05,0x29,0x29,0x29,0x29,0x04,0x24,0x25,0x48,0x25,0x44,
  
};

unsigned char codeBlackEmoji[] = {
  0x48,0x49,0x44,0x25,0x49,0x28,0x29,0x48,0x25,0x25,0x28,0x29,0x49,0x48,0x49,0x28,0x69,
  0x48,0x49,0x4c,0x6d,0x49,0x45,0x4d,0x6e,0x6e,0x49,0x6a,0x45,0x25,0x49,0x48,0x45,0x48,
  0x48,0x48,0x91,0xb6,0x69,0x72,0x93,0x8f,0x6e,0x8f,0x8f,0x6a,0x69,0x69,0x69,0x24,0x49,
  0x48,0x4d,0xb5,0xb5,0xb2,0x8f,0x8e,0x6f,0x6f,0x6b,0x6a,0x8f,0xae,0xae,0xb1,0x49,0x44,
  0x24,0x6d,0xb5,0xb6,0x92,0xb2,0x25,0x46,0x8b,0x6b,0x25,0x8e,0xb1,0xd2,0xb1,0x8d,0x24,
  0x24,0x4d,0xb2,0xb6,0x8e,0x49,0x46,0x8a,0x6b,0x8b,0x8e,0x45,0xb1,0xcd,0xb1,0xb1,0x49,
  0x29,0x4d,0x92,0x8e,0x8a,0x6a,0x6e,0x8e,0x8f,0x8f,0x6e,0x8e,0xb1,0xad,0xb1,0xb2,0x48,
  0x29,0x6d,0x8f,0x8f,0x8f,0x45,0x21,0x6a,0x8b,0x8b,0x49,0x20,0x8e,0xae,0xb1,0xb2,0x48,
  0x25,0x6e,0x8f,0x8f,0x8e,0x21,0x01,0x66,0x8b,0x93,0x45,0x20,0x8a,0x8b,0x8e,0x92,0x49,
  0x44,0x6a,0x6f,0x6f,0x8e,0x6e,0x46,0x8a,0x8f,0x8f,0x8e,0x45,0x8f,0x6b,0x8b,0x6e,0x45,
  0x40,0x46,0x8f,0x6b,0x93,0x8f,0x8a,0x66,0x66,0x66,0x6a,0x8f,0x8f,0x6b,0x8f,0x6a,0x41,
  0x41,0x41,0x6f,0x6b,0x6b,0x8f,0x66,0xaf,0xaf,0xab,0x8a,0x66,0x6b,0x6b,0x8b,0x6a,0x24,
  0x45,0x21,0x66,0x93,0x6b,0x21,0x8e,0xd7,0xaf,0xd3,0xd7,0x26,0x66,0x8f,0x6a,0x45,0x45,
  0x24,0x45,0x29,0x6a,0x93,0x6e,0x6e,0xb7,0xb3,0xd3,0x93,0x66,0x6b,0x8e,0x45,0x45,0x45,
  0x45,0x28,0x49,0x45,0x6a,0x93,0x93,0x8e,0xb3,0x8b,0x6a,0x8f,0x6e,0x45,0x45,0x49,0x44,
  0x49,0x48,0x45,0x49,0x45,0x49,0x8e,0x8f,0x8a,0x8e,0x6e,0x6a,0x45,0x45,0x49,0x48,0x49,
  0x49,0x44,0x49,0x29,0x49,0x24,0x25,0x45,0x49,0x45,0x25,0x25,0x48,0x25,0x49,0x44,0x29,
};

void wakeHandler() {
  if (sleepTime) {
    millisOffsetCount += (RTCZ.getEpoch() - sleepTime);
    sleepTime = 0;
  }
}

void RTCwakeHandler() {
  //not used
}

void watchSleep() {
  if (doVibrate || ANCSRequestStayAwake())
    return;
  sleepTime = RTCZ.getEpoch();
  RTCZ.standbyMode();
}
#endif

BLEConn phoneConnection;
BLEServ timeService;
BLEServ ANCSService;
BLEChar currentTimeChar;
BLEChar NSchar;
BLEChar CPchar;
BLEChar DSchar;
int ANCSInitStep = -1;
unsigned long ANCSInitRetry = 0;

uint8_t ble_connection_state = false;
uint8_t ble_connection_displayed_state = true;
uint8_t TimeData[20];
uint32_t newtime = 0;


uint8_t defaultFontColor = TS_8b_White;
uint8_t defaultFontBG = TS_8b_Black;

uint8_t codeGreenFontColor = TS_8b_Green;
uint8_t codeYellowFontColor = TS_8b_Yellow;
uint8_t codeRedFontColor = TS_8b_Red;
uint8_t codeBlackFontColor = TS_8b_Black;

uint8_t inactiveFontColor = TS_8b_Gray;
uint8_t inactiveFontBG = TS_8b_Black;

uint8_t topBarHeight = 10;
uint8_t timeY = 14;
uint8_t menuTextY[4] = {12, 25, 38, 51};

unsigned long lastReceivedTime = 0;

unsigned long batteryUpdateInterval = 10000;
unsigned long lastBatteryUpdate = 0;

unsigned long sleepTimer = 0;
int sleepTimeout = 3600; // The timeout in second (s) for the smartwatch

uint8_t rewriteDateBTBatt = false;
uint8_t rewriteTime = true;

uint8_t displayOn = 0;
uint8_t buttonReleased = 1;
uint8_t rewriteMenu = false;
uint8_t amtNotifications = 0;
uint8_t lastAmtNotificationsShown = -1;
unsigned long mainDisplayUpdateInterval = 300;
unsigned long lastMainDisplayUpdate = 0;

uint8_t vibratePin = 6;
uint8_t vibratePinActive = HIGH;
uint8_t vibratePinInactive = LOW;


int brightness = 3;
uint8_t lastSetBrightness = 100;

const FONT_INFO& font10pt = thinPixel7_10ptFontInfo;
const FONT_INFO& font22pt = liberationSansNarrow_22ptFontInfo;

char wingHeight=8;
char wingWidth=7;
uint8_t highScore=0;
char flappyBirdX=25;
char flappyBirdY=30;
char flappyBirdHeight=12;
char flappyBirdWidth=17;
char amtPipes=4;
char PipeHeight[]={10,15,20,25,25,10,10};
char pipeOffsetX=0;
const char pipeWidth=10;
const char pipeSpacingX=26;
const char pipeSpacingY=26;
unsigned char lineBuffer[16+96+(pipeSpacingX+pipeWidth)];
unsigned char BGcolor=BLUE;
unsigned char pipeColor=GREEN;
char wingPos=1;
char cloudY[]={5,15,30,25,5,0};
char cloudOffsetX=0;
const char cloudWidth=15;
const char cloudSpacingX=30;
unsigned char cloudColor=WHITE;
char groundY=52;
char groundOffsetX=0;
char birdYmod=1;
unsigned long framecount=0;
char brightnessChanged=0;
char movePipe=4;
char movePipeMod=1;
uint8_t score=0;
int initial_y;


void setup(void)
{
#if defined (ARDUINO_ARCH_AVR)
  for (int i = 0; i < 20; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  setTime(1, 1, 1, 16, 2, 2016);
#elif defined(ARDUINO_ARCH_SAMD)
  /*for (int i = 0; i < 27; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  pinMode(28, INPUT_PULLUP);
  pinMode(29, INPUT_PULLUP);
  pinMode(42, INPUT_PULLUP);
  pinMode(44, INPUT_PULLUP);
  pinMode(45, INPUT_PULLUP);
  pinMode(A4, INPUT);
  pinMode(2, INPUT);*/
  RTCZ.begin();
  RTCZ.setTime(16, 15, 1);//h,m,s
  RTCZ.setDate(25, 7, 16);//d,m,y
  //RTCZ.attachInterrupt(RTCwakeHandler);
  //RTCZ.enableAlarm(RTCZ.MATCH_HHMMSS);
  //RTCZ.setAlarmEpoch(RTCZ.getEpoch() + 1);
  //attachInterrupt(TSP_PIN_BT1, wakeHandler, FALLING);
  //attachInterrupt(TSP_PIN_BT2, wakeHandler, FALLING);
  //attachInterrupt(TSP_PIN_BT3, wakeHandler, FALLING);
  //attachInterrupt(TSP_PIN_BT4, wakeHandler, FALLING);
#endif
  Wire.begin();
  SerialMonitorInterface.begin(9600);
  SerialMonitorInterface.print("Initializing BMA...");
  accel_sensor.begin(BMA250_range_2g, BMA250_update_time_32ms);
  
  //while(!SerialMonitorInterface);
  display.begin();
  display.setFlip(true);
  pinMode(vibratePin, OUTPUT);
  digitalWrite(vibratePin, vibratePinInactive);
  initHomeScreen();
  requestScreenOn();
  delay(100);
  BLEsetup(&phoneConnection, "ICT1003-P8B", BLEConnect, BLEDisconnect);
  useSecurity(BLEBond);
  advertise("ICT1003-P8B", "7905F431-B5CE-4E99-A40F-4B1E122D00D0");

  //SerialUSB.begin(9600);
  Serial.begin(9600);
  //display.begin();
  //display.setFlip(1);
  uint16_t time = millis();
  accel_sensor.read();
  initial_y = accel_sensor.Y;
}

uint32_t millisOffset() {
#if defined (ARDUINO_ARCH_AVR)
  return millis();
#elif defined(ARDUINO_ARCH_SAMD)
  return (millisOffsetCount * 1000ul) + millis();
#endif
}

byte flip=0;
void loop() {
//
//  SerialMonitorInterface.print("outOfTempSelection: ");
//  SerialMonitorInterface.println(outOfTempSelection);
  if (!outOfTempSelection) {
    
  setTemperature();
    if (temp <= 29.9 && tempCode != 0) {
      drawBitmap(codeNormalEmoji);
      display.setCursor(10, menuTextY[3]);
      display.clearWindow(10, menuTextY[3], 80, 8);
      display.fontColor(defaultFontColor, defaultFontBG);
      display.print("Code Normal");
      tempCode = 0;
      // White
    } else if (temp > 29.9 && temp <= 30.9 && tempCode != 1) {
      drawBitmap(codeGreenEmoji);
      display.setCursor(10, menuTextY[3]);
      display.clearWindow(10, menuTextY[3], 80, 8);
      display.fontColor(codeGreenFontColor, defaultFontBG);
      display.print("Code Green");
      tempCode = 1;
      // Green
    } else if (temp > 30.9 && temp <= 31.9 && tempCode != 2) {
      drawBitmap(codeYellowEmoji);
      display.setCursor(10, menuTextY[3]);
      display.clearWindow(10, menuTextY[3], 80, 8);
      display.fontColor(codeYellowFontColor, defaultFontBG);
      display.print("Code Yellow");
      tempCode = 2;
      // Yellow
    } else if (temp > 31.9 && temp <= 32.9 && tempCode != 3) {
      drawBitmap(codeRedEmoji);
      display.setCursor(10, menuTextY[3]);
      display.clearWindow(10, menuTextY[3], 80, 8);
      display.fontColor(codeRedFontColor, defaultFontBG);
      display.print("Code Red");
      tempCode = 3;
      // Red
    } else if (temp > 32.9 && tempCode != 4) {
      drawBitmap(codeBlackEmoji);
      display.setCursor(10, menuTextY[3]);
      display.clearWindow(10, menuTextY[3], 80, 8);
      display.fontColor(codeBlackFontColor, inactiveFontColor);
      display.print("Code Black");
      tempCode = 4;
      // Black
    }
     display.setCursor(10, menuTextY[2]);
     display.print(temp);
  }

  if (!outOfFlappyBirdSelection) {
    unsigned long timer=micros();
  updateDisplay();
  timer=micros()-timer;
  Serial.println(timer);

  accel_sensor.read();//This function gets new data from the acccelerometer

  // Get the acceleration values from the sensor and store them into global variables
  // (Makes reading the rest of the program easier)
  //x = accel_sensor.X;
  static int y = 0;
  y = accel_sensor.X;

  static int count = 0;

  if (count < 10)
  {
    SerialUSB.print(y-initial_y);
    SerialUSB.print(" ");
    count++;
  }
  else
  {
    SerialUSB.println(y-initial_y);
    count = 0;
  }
  
  if(pipeOffsetX<18 && pipeOffsetX>6){
    if(flappyBirdY<(PipeHeight[1]) || flappyBirdY>(PipeHeight[1]+pipeSpacingY-13)){
      pipeColor=RED;
      if(score>highScore)
        highScore=score;
      score=0;
    }else{
      pipeColor=GREEN;
      if(pipeOffsetX==7)
        score++;
    }
  }else{
    pipeColor=GREEN;
  }
  
  
  framecount++;

  int temp = -1*y/3+19;
     
  flappyBirdY = temp;
  pipeOffsetX-=1;
  
  if(movePipe && movePipe<5){
    PipeHeight[movePipe-1]+=movePipeMod;
    if(PipeHeight[movePipe-1]<1)
      movePipeMod=abs(movePipeMod);
    if(PipeHeight[movePipe-1]>groundY-3-pipeSpacingY)
      movePipeMod=-1*abs(movePipeMod);
  }
  if(pipeOffsetX<=0){
    pipeOffsetX=(pipeSpacingX+pipeWidth);
    PipeHeight[0]=PipeHeight[1];
    PipeHeight[1]=PipeHeight[2];
    PipeHeight[2]=PipeHeight[3];
    PipeHeight[3]=PipeHeight[4];
    PipeHeight[4]=3+micros()%(groundY-8-pipeSpacingY);
    if(movePipe)
      movePipe-=1;
    else
      movePipe=3+micros()%5;
  }
  
  groundOffsetX+=1;
  if(groundOffsetX>=5)
    groundOffsetX=0;
  
  if(!(framecount%2))
    cloudOffsetX--;
  if(cloudOffsetX<=0){
    cloudOffsetX=cloudSpacingX+cloudWidth;
    cloudY[0]=cloudY[1];
    cloudY[1]=cloudY[2];
    cloudY[2]=cloudY[3];
    cloudY[3]=cloudY[4];
    cloudY[4]=cloudY[5];
    cloudY[5]=micros()%(30);
  }
  delay (32);
  }
  
  BLEProcess();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  if (!ANCSInitStep) {
    ANCSInit();
  } else if (ANCSInitRetry && millisOffset() - ANCSInitRetry > 1000) {
    ANCSInit();
  }
  ANCSProcess();

  if (ANCSIsBusy()) {
    return;
  }

  amtNotifications = ANCSNotificationCount();

  if (newtime) {
    newtime = 0;
    newTimeData();
  }

  if (ANCSNewNotification()) {
    requestScreenOn();
    rewriteMenu = true;
    updateMainDisplay();
    doVibrate = millisOffset();
  }
  if (doVibrate) {
    uint32_t td = millisOffset() - doVibrate;
    if (td > 0 && td < 100) {
      digitalWrite(vibratePin, vibratePinActive);
    } else if (td > 200 && td < 300) {
      digitalWrite(vibratePin, vibratePinActive);
    } else {
      digitalWrite(vibratePin, vibratePinInactive);
      if (td > 300)doVibrate = 0;
    }
  }
  if (displayOn && (millisOffset() > mainDisplayUpdateInterval + lastMainDisplayUpdate)) {
    updateMainDisplay();
  }
  if (millisOffset() > sleepTimer + ((unsigned long)sleepTimeout * 1000ul)) {
    if (displayOn) {
      displayOn = 0;
      display.off();
    }
#if defined(ARDUINO_ARCH_SAMD)
    BLEProcess();
    //watchSleep();
#endif
  }
  checkButtons();
}

int requestScreenOn() {
  sleepTimer = millisOffset();
  if (!displayOn) {
    displayOn = 1;
    updateMainDisplay();
    display.on();
    return 1;
  }
  return 0;
}

void checkButtons() {
  byte buttons = display.getButtons();
  if (buttonReleased && buttons) {
    if (displayOn)
      buttonPress(buttons);
    requestScreenOn();
    buttonReleased = 0;
  }
  if (!buttonReleased && !(buttons & 0x0F)) {
    buttonReleased = 1;
  }
}

void newTimeData() {
  int y, M, d, k, m, s;
  y = (TimeData[1] << 8) | TimeData[0];
  M = TimeData[2];
  d = TimeData[3];
  k = TimeData[4];
  m = TimeData[5];
  s = TimeData[6];
  //dayOfWeek = timeData[7];
  //fractionOfSecond = timeData[8];
#if defined (ARDUINO_ARCH_AVR)
  setTime(k, m, s, d, M, y);
#elif defined(ARDUINO_ARCH_SAMD)
  RTCZ.setTime(k, m, s);
  RTCZ.setDate(d, M, y - 2000);
#endif
}

void timeCharUpdate(uint8_t * newData, uint8_t length) {
  memcpy(TimeData, newData, length);
  newtime = millisOffset();
  //SerialMonitorInterface.println("Time Update Data RX");
}

void DSCharUpdate(byte * newData, byte length) {
  newDSdata(newData, length);
}

void NSCharUpdate(byte * newData, byte length) {
  newNSdata(newData, length);
}

void BLEConnect() {
  //SerialMonitorInterface.println("---------Connect");
  requestSecurity();
}

void BLEBond() {
  //SerialMonitorInterface.println("---------Bond");
  ANCSInitStep = 0;
}

void BLEDisconnect() {
  //SerialMonitorInterface.println("---------Disconnect");
  ANCSReset();
  ble_connection_state = false;
  ANCSInitStep = -1;
  advertise("ICT1003-P8B", "7905F431-B5CE-4E99-A40F-4B1E122D00D0");
}

void ANCSInit() {
  if (ANCSInitStep == 0)if (!discoverService(&timeService, "1805"))ANCSInitStep++;
  if (ANCSInitStep == 1)if (!discoverService(&ANCSService, "7905F431-B5CE-4E99-A40F-4B1E122D00D0"))ANCSInitStep++;
  if (ANCSInitStep == 2)if (!discoverCharacteristic(&timeService, &currentTimeChar, "2A2B"))ANCSInitStep++;
  if (ANCSInitStep == 3)if (!discoverCharacteristic(&ANCSService, &NSchar, "9FBF120D-6301-42D9-8C58-25E699A21DBD"))ANCSInitStep++;
  if (ANCSInitStep == 4)if (!discoverCharacteristic(&ANCSService, &CPchar, "69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9"))ANCSInitStep++;
  if (ANCSInitStep == 5)if (!discoverCharacteristic(&ANCSService, &DSchar, "22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB"))ANCSInitStep++;
  if (ANCSInitStep == 6)if (!enableNotifications(&currentTimeChar, timeCharUpdate))ANCSInitStep++;
  if (ANCSInitStep == 7)if (!readCharacteristic(&currentTimeChar, TimeData, sizeof(TimeData)))ANCSInitStep++;
  if (ANCSInitStep == 8)if (!enableNotifications(&DSchar, DSCharUpdate))ANCSInitStep++;
  if (ANCSInitStep == 9)if (!enableNotifications(&NSchar, NSCharUpdate))ANCSInitStep++;
  if (ANCSInitStep == 10) {
    //SerialMonitorInterface.println("Connected!");
    newtime = millisOffset();
    ble_connection_state = true;
  } else {
    ANCSInitRetry = millisOffset();
  }
}

void setTemperature() {
  accel_sensor.read();//This function gets new data from the acccelerometer
  x = accel_sensor.X;
  y = accel_sensor.Y;
  z = accel_sensor.Z;
  temp = ((accel_sensor.rawTemp * 0.5) + 20);

  // If the BME280 is not found, nor connected correctly, these values will be produced
  // by the sensor 
  if (x == -1 && y == -1 && z == -1) {
    // Print error message to Serial Monitor
    Serial.print("ERROR! NO BMA250 DETECTED!");
  }
  
  else { // if we have correct sensor readings: 
    //showSerial();                 //Print to Serial Monitor 
    //showGraph(x, y, z);           //Show on Serial Plotter
  }

  // The BMA250 can only poll new sensor values every 64ms, so this delay
  // will ensure that we can continue to read values
  delay(250);
  // ***Without the delay, there would not be any sensor output*** 

  if (temp < 33.0) {
    temperature_noti = false;
  }

  if (!temperature_noti) {
    if (temp > 32.9) {
      Write_UART_TX("Code Black Alert", sizeof("Code Black Alert"));
      Write_UART_TX("Stop Training Now!", sizeof("Stop Training Now!"));
      temperature_noti = true;
    }
  }
}

void showSerial() {
  SerialMonitorInterface.print("X = ");
  SerialMonitorInterface.print(x);
  
  SerialMonitorInterface.print("  Y = ");
  SerialMonitorInterface.print(y);
  
  SerialMonitorInterface.print("  Z = ");
  SerialMonitorInterface.print(z);
  
  SerialMonitorInterface.print("  Temperature(C) = ");
  SerialMonitorInterface.println(temp);
}

//Plots X, Y and Z data on Serial Plotter (found under 'Tools')
void showGraph(int x, int y, int z) {
  SerialMonitorInterface.print(x);
  SerialMonitorInterface.print(",");

  SerialMonitorInterface.print(y);
  SerialMonitorInterface.print(",");

  SerialMonitorInterface.print(z);
  SerialMonitorInterface.print(",");
}

void drawBitmap(unsigned char imgCode[]){
  //set a background that matches
  //let's set up for a bitmap at (40,30) that is 17 pixels wide and 12 pixels tall:
  //setX(x start, x end);//set OLED RAM to x start, wrap around at x end
  display.setX(60,60+17-1);
  //setY(y start, y end);//set OLED RAM to y start, wrap around at y end
  display.setY(34,34+17-1);
  //now start a data transfer
  display.startData();
  //writeBuffer(buffer,count);//optimized write of a large buffer of 8 bit data.
  display.writeBuffer(imgCode,17*17);
  display.endTransfer();
  delay(1000);
  
}

void updateDisplay(){
  display.goTo(0,0);
  display.startData();
  for(unsigned char y=0;y<groundY;y++){
    
    for(unsigned char i=16;i<112;i++)
      lineBuffer[i]=BGcolor;
      
    int x=cloudOffsetX;
    char cloud=0;
    while(x<16+96){
      if(y>cloudY[cloud] && y<cloudY[cloud]+8){
        if(y<cloudY[cloud]+2 || y>cloudY[cloud]+6)
          setBuffer(x+1, cloudWidth-2, cloudColor);
        else
          setBuffer(x, cloudWidth, cloudColor);
      }
      cloud++;
      x+=(cloudSpacingX+cloudWidth);
    }
    
    x=pipeOffsetX;
    char pipe=0;
    while(x<16+96){
      if(y<PipeHeight[pipe] || y>PipeHeight[pipe]+pipeSpacingY){
        if(y<PipeHeight[pipe]-4 || y>PipeHeight[pipe]+pipeSpacingY+4)
          setBuffer(x, pipeWidth, pipeColor);
        else
          setBuffer(x-1, pipeWidth+2, pipeColor);
      }
      pipe++;
      x+=(pipeSpacingX+pipeWidth);
    }
    
    if(y>=flappyBirdY && y<flappyBirdY+flappyBirdHeight){
      int offset=(y-flappyBirdY)*flappyBirdWidth;
      for(int i=0;i<flappyBirdWidth;i++){
        unsigned char color=flappyBird[offset+i];
        if(color!=ALPHA)
          lineBuffer[16+flappyBirdX+i]=color;
      }
    }
    char wingY=flappyBirdY+3;
    if(y>=wingY && y<wingY+wingHeight){
      int offset=(y-wingY)*wingWidth;
      for(int i=0;i<wingWidth;i++){
        unsigned char color;
        if(wingPos==0)
          color=wingUp[offset+i];
        if(wingPos==1)
          color=wingMid[offset+i];
        if(wingPos==2)
          color=wingDown[offset+i];
        if(color!=ALPHA)
          lineBuffer[16+flappyBirdX+i]=color;
      }
    }
    
    
    char highScoreString[3];
    highScoreString[0]=(highScore/10)+'0';
    highScoreString[1]=(highScore%10)+'0';
    highScoreString[2]='\0';
    
    putString(y,80,1,highScoreString,liberationSans_10ptFontInfo);
    
    
    char scoreString[3];
    scoreString[0]=(score/10)+'0';
    scoreString[1]=(score%10)+'0';
    scoreString[2]='\0';
    
    
    putString(y,60,15,scoreString,liberationSans_16ptFontInfo);
    
    
    display.writeBuffer(lineBuffer+16,96);
  }
  for(int z=0;z<21;z++){
    lineBuffer[16+(z*5)]=GREEN;
    lineBuffer[17+(z*5)]=GREEN;
    lineBuffer[18+(z*5)]=GREEN;
    lineBuffer[19+(z*5)]=DGREEN;
    lineBuffer[20+(z*5)]=DGREEN;
  }
  display.writeBuffer(lineBuffer+16+groundOffsetX,96);
  display.writeBuffer(lineBuffer+17+groundOffsetX,96);
  display.writeBuffer(lineBuffer+18+groundOffsetX,96);
  display.writeBuffer(lineBuffer+19+groundOffsetX,96);
  display.writeBuffer(lineBuffer+20+groundOffsetX,96);
  
  
  for(unsigned char i=16;i<112;i++)
    lineBuffer[i]=BROWN;
  for(unsigned char i=0;i<7;i++)
    display.writeBuffer(lineBuffer+16,96);
  
  display.endTransfer();
}

void putString(uint8_t y, uint8_t fontX, uint8_t fontY, char * string, const FONT_INFO& fontInfo){
  const FONT_CHAR_INFO* fontDescriptor=fontInfo.charDesc;
  uint8_t fontHeight=fontInfo.height;
  if(y>=fontY && y<fontY+fontHeight){
      const unsigned char* fontBitmap=fontInfo.bitmap;
      uint8_t fontFirstCh=fontInfo.startCh;
      uint8_t fontLastCh=fontInfo.endCh;
      //if(!_fontFirstCh)return 1;
      //if(ch<_fontFirstCh || ch>_fontLastCh)return 1;
      //if(_cursorX>xMax || _cursorY>yMax)return 1;
      uint8_t stringChar=0;
      uint8_t ch=string[stringChar++];
      while(ch){
        uint8_t chWidth=pgm_read_byte(&fontDescriptor[ch-fontFirstCh].width);
        uint8_t bytesPerRow=chWidth/8;
        if(chWidth>bytesPerRow*8)
          bytesPerRow++;
        uint16_t offset=pgm_read_word(&fontDescriptor[ch-fontFirstCh].offset)+(bytesPerRow*fontHeight)-1;
      
        for(uint8_t byte=0; byte<bytesPerRow; byte++){
          uint8_t data=pgm_read_byte(fontBitmap+offset-(y-fontY)-((bytesPerRow-byte-1)*fontHeight));
          uint8_t bits=byte*8;
          for(uint8_t i=0; i<8 && (bits+i)<chWidth; i++){
            if(data&(0x80>>i)){
              lineBuffer[16+fontX]=0;
             }else{
              //SPDR=_fontBGcolor;
             }
             fontX++;
           }
         }
        fontX+=2;
        ch=string[stringChar++];
      }
  }
}

void setBuffer(char i, char amt, unsigned char color){
  char endbyte=i+amt;
  while(i<endbyte)
    lineBuffer[i++]=color;
}
