#include "ch.h"
#include "hal.h"
#include "chprintf.h"

#include "debug.h"
#include "threads.h"
#include "ov5640.h"
#include "pi2c.h"
#include "ssdv.h"
#include "aprs.h"
#include "radio.h"
#include "base91.h"
#include <string.h>
#include "types.h"
#include "sleep.h"
#include "watchdog.h"
#include "flash.h"
#include "sd.h"
#include "collector.h"
#include "image.h"

const uint8_t noCameraFound[4071] = {
	0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01, 0x01, 0x01, 0x00, 0x48,
	0x00, 0x48, 0x00, 0x00, 0xFF, 0xDB, 0x00, 0x43, 0x00, 0x10, 0x0B, 0x0C, 0x0E, 0x0C, 0x0A, 0x10,
	0x0E, 0x0D, 0x0E, 0x12, 0x11, 0x10, 0x13, 0x18, 0x28, 0x1A, 0x18, 0x16, 0x16, 0x18, 0x31, 0x23,
	0x25, 0x1D, 0x28, 0x3A, 0x33, 0x3D, 0x3C, 0x39, 0x33, 0x38, 0x37, 0x40, 0x48, 0x5C, 0x4E, 0x40,
	0x44, 0x57, 0x45, 0x37, 0x38, 0x50, 0x6D, 0x51, 0x57, 0x5F, 0x62, 0x67, 0x68, 0x67, 0x3E, 0x4D,
	0x71, 0x79, 0x70, 0x64, 0x78, 0x5C, 0x65, 0x67, 0x63, 0xFF, 0xDB, 0x00, 0x43, 0x01, 0x11, 0x12,
	0x12, 0x18, 0x15, 0x18, 0x2F, 0x1A, 0x1A, 0x2F, 0x63, 0x42, 0x38, 0x42, 0x63, 0x63, 0x63, 0x63,
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63,
	0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0xFF, 0xC0,
	0x00, 0x11, 0x08, 0x00, 0x80, 0x00, 0xA0, 0x03, 0x01, 0x11, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11,
	0x01, 0xFF, 0xC4, 0x00, 0x1B, 0x00, 0x00, 0x02, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x04, 0x05, 0x07, 0x06, 0x01, 0xFF, 0xC4,
	0x00, 0x47, 0x10, 0x00, 0x00, 0x05, 0x02, 0x02, 0x05, 0x05, 0x0C, 0x06, 0x09, 0x05, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x11, 0x12, 0x21, 0x06, 0x13, 0x31, 0x41,
	0x81, 0x14, 0x15, 0x16, 0x42, 0x51, 0x22, 0x32, 0x53, 0x54, 0x61, 0x71, 0x91, 0x92, 0xA1, 0xC1,
	0xE1, 0xE2, 0x36, 0x55, 0x63, 0x93, 0xA3, 0xB3, 0x23, 0x24, 0x52, 0x62, 0x64, 0x65, 0xA2, 0xB1,
	0xD1, 0x07, 0x35, 0x43, 0x74, 0x82, 0xB2, 0xFF, 0xC4, 0x00, 0x1A, 0x01, 0x01, 0x01, 0x00, 0x03,
	0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x02, 0x03,
	0x04, 0x06, 0x01, 0xFF, 0xC4, 0x00, 0x39, 0x11, 0x01, 0x00, 0x00, 0x03, 0x04, 0x06, 0x07, 0x05,
	0x08, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x11, 0x05, 0x12, 0x51, 0xC1,
	0x03, 0x04, 0x13, 0x31, 0x42, 0x81, 0x14, 0x15, 0x21, 0x43, 0x52, 0xA1, 0xE1, 0x06, 0x32, 0x33,
	0x61, 0x62, 0x16, 0x22, 0x41, 0x63, 0x71, 0x82, 0xA2, 0xE2, 0x23, 0x24, 0xD1, 0x91, 0xFF, 0xDA,
	0x00, 0x0C, 0x03, 0x01, 0x00, 0x02, 0x11, 0x03, 0x11, 0x00, 0x3F, 0x00, 0xF4, 0x00, 0x00, 0x0A,
	0x7F, 0xAB, 0xC4, 0x49, 0xB4, 0xF8, 0x39, 0xE4, 0xD9, 0x21, 0x22, 0x43, 0x60, 0x01, 0x5E, 0x66,
	0xC4, 0x71, 0xF7, 0x0B, 0xD6, 0x2F, 0x79, 0xCB, 0x34, 0x5B, 0x5F, 0x83, 0x9E, 0x4A, 0xC2, 0xF2,
	0x20, 0x00, 0x97, 0xFA, 0xBC, 0x44, 0x9B, 0x4F, 0x74, 0x9C, 0xF2, 0x67, 0x21, 0x22, 0x43, 0x60,
	0x00, 0x89, 0x7D, 0xEA, 0x3C, 0xE6, 0x2F, 0x58, 0xBD, 0xE7, 0x2C, 0xDE, 0x9B, 0xD9, 0xFE, 0xF7,
	0xF6, 0xE6, 0xAA, 0x2F, 0x3D, 0x30, 0x00, 0xA7, 0xFA, 0xBC, 0x44, 0x9B, 0x4F, 0x83, 0x9E, 0x4C,
	0xE4, 0x24, 0x48, 0x6C, 0x00, 0x2B, 0xCC, 0xD8, 0x8E, 0x3E, 0xE1, 0x7A, 0xC5, 0xEF, 0x39, 0x66,
	0x8B, 0x6B, 0xF0, 0x73, 0xC9, 0x58, 0x5E, 0x44, 0x00, 0x3A, 0xA8, 0xF2, 0x4E, 0x80, 0x01, 0x4F,
	0xF5, 0x78, 0x89, 0x36, 0x9F, 0x07, 0x3C, 0x9B, 0x24, 0x24, 0x48, 0x6C, 0x00, 0x2B, 0xCC, 0xD8,
	0x8E, 0x3E, 0xE1, 0x7A, 0xC5, 0xEF, 0x39, 0x66, 0x8B, 0x6B, 0xF0, 0x73, 0xC9, 0x58, 0x5E, 0x44,
	0x00, 0x12, 0xFF, 0x00, 0x57, 0x88, 0x93, 0x69, 0xEE, 0x93, 0x9E, 0x4C, 0xE4, 0x24, 0x48, 0x6C,
	0x00, 0x11, 0x2F, 0xBD, 0x47, 0x9C, 0xC5, 0xEB, 0x17, 0xBC, 0xE5, 0x9B, 0xD3, 0x7B, 0x3F, 0xDE,
	0xFE, 0xDC, 0xD5, 0x45, 0xE7, 0xA6, 0x00, 0x14, 0xFF, 0x00, 0x57, 0x88, 0x93, 0x69, 0xF0, 0x73,
	0xC9, 0x9C, 0x84, 0x89, 0x0D, 0x80, 0x05, 0x79, 0x9B, 0x11, 0xC7, 0xDC, 0x2F, 0x58, 0xBD, 0xE7,
	0x2C, 0xD1, 0x6D, 0x7E, 0x0E, 0x79, 0x2B, 0x0B, 0xC8, 0x80, 0x07, 0x55, 0x1E, 0x49, 0xD0, 0x00,
	0x29, 0xFE, 0xAF, 0x11, 0x26, 0xD3, 0xE0, 0xE7, 0x93, 0x64, 0x84, 0x89, 0x0D, 0x80, 0x05, 0x79,
	0x9B, 0x11, 0xC7, 0xDC, 0x2F, 0x58, 0xBD, 0xE7, 0x2C, 0xD1, 0x6D, 0x7E, 0x0E, 0x79, 0x2B, 0x0B,
	0xC8, 0x80, 0x02, 0x5F, 0xEA, 0xF1, 0x12, 0x6D, 0x3D, 0xD2, 0x73, 0xC9, 0x9C, 0x84, 0x89, 0x0D,
	0x80, 0x02, 0x25, 0xF7, 0xA8, 0xF3, 0x98, 0xBD, 0x62, 0xF7, 0x9C, 0xB3, 0x7A, 0x6F, 0x67, 0xFB,
	0xDF, 0xDB, 0x9A, 0xA8, 0xBC, 0xF4, 0xC0, 0x02, 0x9F, 0xEA, 0xF1, 0x12, 0x6D, 0x3E, 0x0E, 0x79,
	0x33, 0x90, 0x91, 0x21, 0xB0, 0x00, 0xAF, 0x33, 0x62, 0x38, 0xFB, 0x85, 0xEB, 0x17, 0xBC, 0xE5,
	0x9A, 0x2D, 0xAF, 0xC1, 0xCF, 0x25, 0x61, 0x79, 0x10, 0x00, 0xEA, 0xA3, 0xC9, 0x3A, 0x00, 0x05,
	0x3F, 0xD5, 0xE2, 0x24, 0xDA, 0x7C, 0x1C, 0xF2, 0x6C, 0x90, 0x91, 0x21, 0xB0, 0x00, 0xAF, 0x33,
	0x62, 0x38, 0xFB, 0x85, 0xEB, 0x17, 0xBC, 0xE5, 0x9A, 0x2D, 0xAF, 0xC1, 0xCF, 0x25, 0x61, 0x79,
	0x10, 0x00, 0x4B, 0xFD, 0x5E, 0x22, 0x4D, 0xA7, 0xBA, 0x4E, 0x79, 0x33, 0x90, 0x91, 0x21, 0xB0,
	0x00, 0x44, 0xBE, 0xF5, 0x1E, 0x73, 0x17, 0xAC, 0x5E, 0xF3, 0x96, 0x6F, 0x4D, 0xEC, 0xFF, 0x00,
	0x7B, 0xFB, 0x73, 0x55, 0x17, 0x9E, 0x98, 0x00, 0x53, 0xFD, 0x5E, 0x22, 0x4D, 0xA7, 0xC1, 0xCF,
	0x26, 0x72, 0x12, 0x24, 0x36, 0x00, 0x15, 0xE6, 0x6C, 0x47, 0x1F, 0x70, 0xBD, 0x62, 0xF7, 0x9C,
	0xB3, 0x45, 0xB5, 0xF8, 0x39, 0xE4, 0xAC, 0x2F, 0x22, 0x00, 0x1D, 0x54, 0x79, 0x27, 0x43, 0x0A,
	0xAF, 0xA4, 0xCC, 0xD2, 0xA6, 0x9C, 0x67, 0x23, 0xB8, 0xE2, 0x89, 0x24, 0xAC, 0x49, 0x32, 0x22,
	0xCC, 0x06, 0x7B, 0x9A, 0x6B, 0x1D, 0x76, 0xB4, 0x47, 0x4A, 0xDF, 0xBC, 0x43, 0x8F, 0x5B, 0xD5,
	0xA3, 0xA7, 0xBB, 0x48, 0xD2, 0x95, 0x65, 0x2C, 0xD4, 0x43, 0xA6, 0x31, 0xFC, 0x51, 0xDF, 0x58,
	0x87, 0x17, 0x56, 0x4F, 0xE2, 0x65, 0x7C, 0x74, 0xC6, 0x3F, 0x8A, 0x3B, 0xEB, 0x10, 0x75, 0x64,
	0xFE, 0x22, 0xF9, 0x4F, 0xE9, 0x6B, 0x0E, 0x92, 0x6D, 0x15, 0xC2, 0xB7, 0xEF, 0x10, 0xA3, 0xA8,
	0x68, 0x63, 0xAA, 0xDE, 0xAC, 0x6B, 0x5A, 0x79, 0x55, 0xC3, 0xAE, 0xEA, 0xF1, 0xD6, 0x2E, 0xD2,
	0x34, 0xA5, 0x72, 0x2B, 0xA4, 0xEC, 0xF8, 0xB3, 0x9E, 0xB1, 0x0A, 0x1B, 0x68, 0x60, 0xE0, 0xEA,
	0xC9, 0xFC, 0x43, 0xA4, 0xEC, 0xF8, 0xB3, 0x9E, 0xB1, 0x06, 0xDA, 0x18, 0x1D, 0x59, 0x3F, 0x89,
	0x07, 0x34, 0x91, 0x95, 0xDA, 0xD1, 0xD6, 0x56, 0xF2, 0x90, 0xE3, 0xD6, 0xE4, 0xDB, 0xDD, 0xA7,
	0x65, 0x2A, 0xCA, 0x5B, 0x3A, 0x68, 0x71, 0x21, 0xD2, 0x16, 0xBC, 0x02, 0xFD, 0x24, 0x38, 0xBA,
	0x1C, 0xD8, 0xB2, 0xE8, 0x13, 0x62, 0x3A, 0x42, 0xD7, 0x80, 0x5F, 0xA4, 0x83, 0xA1, 0xCD, 0x89,
	0xD0, 0x26, 0xC4, 0xB7, 0xAB, 0xAD, 0x3A, 0x49, 0x22, 0x65, 0x65, 0x6F, 0x29, 0x0A, 0x3A, 0x87,
	0xFA, 0xB7, 0xAB, 0xDB, 0x5A, 0x79, 0x55, 0x56, 0xCD, 0xFF, 0x00, 0x4E, 0xFD, 0xEE, 0xDA, 0xD3,
	0xCA, 0xBF, 0xF4, 0x9E, 0x77, 0x6F, 0xC1, 0x2B, 0xD2, 0x28, 0xF4, 0xC9, 0x70, 0x55, 0xE9, 0xF2,
	0xE0, 0x39, 0xDD, 0xBF, 0x04, 0xAF, 0x48, 0x74, 0xC9, 0x70, 0x3A, 0x7C, 0xB8, 0x20, 0xE5, 0x51,
	0x0B, 0xB5, 0x9B, 0x51, 0x5B, 0xCA, 0x38, 0xF5, 0xB9, 0xF6, 0xF7, 0x69, 0xD9, 0x4A, 0xB2, 0x96,
	0xD1, 0x96, 0x1C, 0x28, 0x73, 0x8A, 0x3C, 0x1A, 0xBD, 0x23, 0x8B, 0x63, 0x1C, 0x59, 0x75, 0x9C,
	0x9E, 0x11, 0xCE, 0x28, 0xF0, 0x6A, 0xF4, 0x86, 0xC6, 0x38, 0x9D, 0x67, 0x27, 0x84, 0xA7, 0xE6,
	0x25, 0xD2, 0x4D, 0x90, 0x65, 0x61, 0x47, 0x50, 0xD3, 0x43, 0x55, 0xBD, 0x58, 0x56, 0xB4, 0xF2,
	0xAB, 0x87, 0x5D, 0xD6, 0x21, 0xAC, 0x5D, 0xA4, 0x29, 0x4A, 0xE4, 0x5A, 0x5D, 0x25, 0x28, 0x8A,
	0xDB, 0x45, 0x4D, 0x16, 0xBF, 0x2E, 0x92, 0x78, 0x49, 0x08, 0x6F, 0x4F, 0x8C, 0x94, 0x30, 0x50,
	0x60, 0xEA, 0xA3, 0xC9, 0x3A, 0x18, 0x6C, 0x9B, 0x85, 0xA5, 0x55, 0x63, 0x65, 0xC4, 0xB4, 0xE9,
	0x52, 0x97, 0x81, 0xC5, 0x2B, 0x09, 0x20, 0xEE, 0x9B, 0x19, 0x99, 0xEC, 0xB7, 0x68, 0x08, 0xD3,
	0xA4, 0xB4, 0xFE, 0x93, 0xE8, 0xFA, 0x1C, 0x92, 0xC4, 0xBA, 0x8B, 0x68, 0x7C, 0xA5, 0x3E, 0xC9,
	0x92, 0x89, 0x57, 0x4A, 0xB0, 0x16, 0x22, 0xC8, 0xCC, 0x8A, 0xE0, 0x30, 0xE5, 0x4B, 0x4D, 0x5A,
	0x4C, 0x08, 0x4E, 0xD7, 0x64, 0xCE, 0x69, 0xD9, 0x48, 0x4A, 0xD0, 0xEB, 0x1A, 0xB2, 0x41, 0x19,
	0xDB, 0x15, 0xEE, 0x79, 0xE6, 0x60, 0x17, 0xA4, 0x95, 0x99, 0xBC, 0xB6, 0xA1, 0x4A, 0x49, 0xA5,
	0xA8, 0x08, 0x70, 0xDA, 0x44, 0x74, 0xA0, 0x89, 0x29, 0x24, 0x2B, 0x23, 0x2C, 0xB6, 0xE5, 0x7E,
	0x20, 0x3D, 0x0D, 0x6E, 0x94, 0xFA, 0x34, 0x35, 0x50, 0x8D, 0x85, 0xA5, 0x10, 0x18, 0x61, 0xF4,
	0x9D, 0xB6, 0xB8, 0x66, 0xAD, 0x6E, 0x7D, 0x84, 0x4A, 0xB8, 0x06, 0x9B, 0x69, 0x98, 0x5A, 0x38,
	0xDA, 0x52, 0x5A, 0xF8, 0x4C, 0x44, 0x90, 0x5B, 0x6E, 0xA6, 0xD6, 0x78, 0x17, 0x97, 0x90, 0xC9,
	0x07, 0xC4, 0xC0, 0x78, 0x5A, 0xE7, 0xFB, 0xED, 0x43, 0xFE, 0xCB, 0x9F, 0xFD, 0x18, 0x0A, 0x20,
	0x3D, 0x66, 0x88, 0x2A, 0x42, 0x28, 0x55, 0xD5, 0x43, 0x92, 0xDC, 0x57, 0xCB, 0x93, 0xE1, 0x75,
	0xC7, 0x09, 0x09, 0x4F, 0x74, 0xAB, 0xDC, 0xCF, 0x22, 0xCA, 0xE5, 0xC4, 0x05, 0x6D, 0x23, 0xA9,
	0x13, 0x55, 0x98, 0x72, 0xA9, 0xF2, 0x9B, 0x54, 0xC6, 0xA2, 0xA1, 0x2F, 0xC8, 0x8E, 0x45, 0x65,
	0x3B, 0x63, 0x25, 0x1D, 0xED, 0x63, 0xB9, 0x1E, 0xD0, 0x1A, 0xF5, 0xA2, 0x3A, 0xA7, 0xFA, 0x90,
	0xCD, 0x3A, 0x6B, 0xA6, 0xA8, 0x8D, 0x38, 0x93, 0x43, 0x6A, 0xD8, 0x57, 0x6D, 0x2A, 0x32, 0xE2,
	0x65, 0x6E, 0x20, 0x31, 0xF9, 0xDA, 0x65, 0x66, 0xB5, 0x1A, 0x9F, 0x38, 0xCB, 0x91, 0xAE, 0x6A,
	0x0B, 0x93, 0xE1, 0x24, 0x93, 0x65, 0x8B, 0x0E, 0x12, 0xB6, 0x65, 0x91, 0x99, 0x00, 0xD3, 0xA9,
	0xC8, 0x72, 0xA1, 0x07, 0x49, 0x99, 0x95, 0x85, 0x6D, 0xD3, 0xDF, 0x41, 0x45, 0x2C, 0x04, 0x5A,
	0x92, 0xD6, 0x1A, 0x6C, 0x56, 0x2D, 0x96, 0x22, 0x20, 0x1E, 0x28, 0x00, 0x03, 0xDA, 0x68, 0xCB,
	0x8E, 0x15, 0x1A, 0x22, 0x29, 0x8F, 0xC7, 0x69, 0xF3, 0x9B, 0xFA, 0xE9, 0x38, 0xB4, 0xA5, 0x4A,
	0x6F, 0x2B, 0x77, 0xDB, 0x53, 0x6D, 0xC5, 0xBC, 0x05, 0xEA, 0x04, 0x15, 0x40, 0xAF, 0xD7, 0x25,
	0xC2, 0x8E, 0x6B, 0x26, 0x1F, 0x4B, 0x0D, 0xB6, 0x82, 0xCB, 0x0A, 0x9C, 0x23, 0x59, 0x17, 0x99,
	0x24, 0x03, 0xC8, 0xD7, 0xA1, 0x73, 0x7E, 0x91, 0x4C, 0x8A, 0x45, 0x64, 0xA1, 0xD3, 0x34, 0x97,
	0x62, 0x4F, 0x32, 0xF6, 0x19, 0x0E, 0x8D, 0x53, 0xE3, 0x4A, 0xF9, 0x36, 0xE5, 0x61, 0xE9, 0x5A,
	0x1D, 0x54, 0x79, 0x27, 0x43, 0xC4, 0xE9, 0x1D, 0x49, 0xEA, 0x75, 0x7A, 0x66, 0xA5, 0x2D, 0xAB,
	0x94, 0xC3, 0xE4, 0xEB, 0xC6, 0x46, 0x76, 0x4A, 0xB6, 0x99, 0x58, 0xF6, 0xE4, 0x03, 0x06, 0x93,
	0x52, 0x7A, 0x91, 0x52, 0x6A, 0x74, 0x74, 0xB6, 0xA7, 0x5A, 0xBE, 0x12, 0x70, 0x8C, 0xD3, 0x99,
	0x19, 0x67, 0x63, 0x2E, 0xD0, 0x0C, 0x9B, 0x53, 0x4C, 0xB6, 0x92, 0x86, 0xE9, 0xD0, 0xA2, 0xA9,
	0x2A, 0x25, 0x6B, 0x23, 0xA5, 0x49, 0x57, 0x9A, 0xE6, 0xA3, 0xCB, 0xFC, 0x00, 0xB3, 0x23, 0x49,
	0x1F, 0x94, 0x93, 0x39, 0x30, 0xA0, 0xBA, 0xF2, 0xB0, 0x6B, 0x1F, 0x36, 0x8C, 0x9C, 0x70, 0x92,
	0x64, 0x76, 0x33, 0x23, 0xDF, 0x62, 0x23, 0xB1, 0x16, 0x40, 0x23, 0xD2, 0x29, 0x87, 0x53, 0x9B,
	0x3D, 0x6D, 0xB2, 0xB7, 0x26, 0xB4, 0xA6, 0x5D, 0x42, 0x92, 0x78, 0x70, 0x99, 0x11, 0x58, 0xB3,
	0xBE, 0xE2, 0xDE, 0x02, 0x71, 0xF4, 0x9E, 0x7C, 0x7A, 0x84, 0x39, 0x8D, 0xA5, 0x92, 0x72, 0x24,
	0x62, 0x8A, 0x94, 0xD9, 0x58, 0x56, 0x82, 0x23, 0xB6, 0x2C, 0xF3, 0x3C, 0xEF, 0xBB, 0x61, 0x00,
	0xCA, 0x97, 0x21, 0x72, 0xE5, 0xBD, 0x25, 0xC2, 0x49, 0x2D, 0xE7, 0x14, 0xE2, 0x89, 0x3B, 0x08,
	0xCC, 0xEF, 0x90, 0x05, 0x00, 0xB9, 0x1A, 0xA4, 0xF4, 0x6A, 0x6C, 0xD8, 0x28, 0x4B, 0x66, 0xD4,
	0xCC, 0x1A, 0xC3, 0x51, 0x1E, 0x22, 0xC0, 0x77, 0x2B, 0x66, 0x01, 0x31, 0x1F, 0x28, 0xD2, 0x50,
	0xF2, 0x98, 0x69, 0xF2, 0x4D, 0xFF, 0x00, 0x46, 0xF1, 0x19, 0xA5, 0x59, 0x5B, 0x3B, 0x19, 0x18,
	0x0B, 0xF5, 0x5A, 0xFC, 0x8A, 0x9C, 0xF6, 0xA7, 0x29, 0x88, 0xF1, 0xE5, 0x36, 0xA2, 0x5E, 0xB5,
	0x84, 0x99, 0x1A, 0x8C, 0xAD, 0x63, 0x3B, 0x99, 0xEC, 0xC2, 0x56, 0x00, 0x4E, 0xAF, 0x3F, 0x34,
	0x89, 0x47, 0x16, 0x23, 0x12, 0x35, 0x84, 0xEA, 0xA4, 0x30, 0xDE, 0x07, 0x14, 0xA2, 0xDE, 0x67,
	0x7E, 0xD3, 0xBE, 0x56, 0xCC, 0x04, 0xAA, 0x1A, 0x45, 0x2E, 0x7C, 0x57, 0x58, 0x53, 0x31, 0x98,
	0x27, 0xD4, 0x4B, 0x7D, 0x6C, 0x37, 0x85, 0x4F, 0x99, 0x6C, 0x35, 0x67, 0x9E, 0x79, 0xEE, 0xCC,
	0x06, 0x40, 0x00, 0x06, 0x95, 0x32, 0xB2, 0xBA, 0x6B, 0x64, 0x4D, 0x43, 0x86, 0xEB, 0x89, 0x5E,
	0xB1, 0x0F, 0x3A, 0xD6, 0x25, 0xB6, 0x76, 0x2D, 0x87, 0x7D, 0xD6, 0xB9, 0x5C, 0x8F, 0x30, 0x10,
	0x7A, 0xB1, 0x25, 0xFA, 0x62, 0xA0, 0xB9, 0x80, 0xD0, 0xB9, 0x27, 0x25, 0x6E, 0x67, 0x8D, 0x6B,
	0x32, 0xB6, 0x67, 0x7B, 0x7B, 0x00, 0x15, 0x1A, 0x9B, 0xD5, 0x69, 0x8D, 0xC8, 0x90, 0x86, 0xD2,
	0xEA, 0x5B, 0x4B, 0x66, 0x68, 0x23, 0x2C, 0x76, 0xDE, 0x77, 0x33, 0xCC, 0x74, 0x6A, 0x9F, 0x1A,
	0x57, 0xC9, 0xB7, 0x16, 0x3D, 0x2B, 0x43, 0xAA, 0x8F, 0x24, 0xE8, 0x79, 0xEE, 0x4D, 0x06, 0x4E,
	0x97, 0x4F, 0x3A, 0x93, 0x24, 0xEC, 0x76, 0x69, 0xE6, 0xE9, 0x91, 0xEE, 0xB1, 0xA7, 0x32, 0xF2,
	0xDA, 0xE0, 0x15, 0x0F, 0x46, 0x22, 0x25, 0xB8, 0xCC, 0x4C, 0x67, 0x12, 0x9A, 0x96, 0xF9, 0x3A,
	0xB4, 0x9D, 0x8D, 0xC4, 0x21, 0x37, 0x49, 0x5F, 0xB0, 0xF2, 0xE0, 0x60, 0x30, 0xEA, 0x4C, 0xC5,
	0x99, 0xA3, 0x6D, 0x55, 0x98, 0x88, 0xDC, 0x47, 0x53, 0x2C, 0xE3, 0x2D, 0x0D, 0x19, 0xE1, 0x51,
	0x61, 0xC4, 0x47, 0x63, 0x33, 0xB1, 0xEE, 0x01, 0x82, 0x03, 0xD2, 0x56, 0xCA, 0x05, 0x11, 0xE5,
	0xD2, 0x13, 0x4E, 0x65, 0xF7, 0x50, 0xC1, 0x13, 0xD2, 0x16, 0xA5, 0x63, 0xD6, 0xA9, 0x37, 0xBA,
	0x73, 0xB1, 0x11, 0x5C, 0xB2, 0xB6, 0xE0, 0x16, 0xE1, 0x51, 0xA2, 0x39, 0xA2, 0x26, 0x4A, 0x65,
	0x27, 0x3D, 0xF8, 0xEF, 0x4C, 0x43, 0xA7, 0xDF, 0x21, 0x2D, 0xA9, 0x25, 0x84, 0x8B, 0xCA, 0x57,
	0x01, 0xA7, 0x40, 0xD1, 0xEA, 0x74, 0xEA, 0x35, 0x16, 0x4B, 0xB1, 0x9B, 0x52, 0xCD, 0xC5, 0x9C,
	0x83, 0x33, 0xB6, 0x34, 0xF7, 0x64, 0x57, 0xED, 0xEE, 0xB0, 0x17, 0x10, 0x15, 0x91, 0x4E, 0x88,
	0xCC, 0x0A, 0x93, 0xAD, 0x44, 0xA6, 0x1B, 0x8D, 0xD5, 0x9D, 0x61, 0x27, 0x39, 0x78, 0x10, 0x96,
	0xC8, 0xAE, 0x49, 0x23, 0xB9, 0x67, 0xF1, 0x00, 0x8A, 0x5C, 0x02, 0x90, 0x8A, 0xE3, 0xAD, 0xD3,
	0x29, 0xF3, 0x25, 0xB1, 0xC9, 0xF5, 0x2C, 0xB0, 0x66, 0xB6, 0x73, 0xBE, 0x2C, 0x36, 0x57, 0x66,
	0x67, 0x9E, 0xD2, 0x01, 0x17, 0x68, 0xB0, 0xDD, 0xD3, 0x1A, 0x64, 0x24, 0xB2, 0xDB, 0x04, 0xB6,
	0x10, 0xF4, 0xC6, 0x09, 0x46, 0xA4, 0xB6, 0xB2, 0x23, 0x52, 0x93, 0xB4, 0xEC, 0x56, 0x22, 0x2D,
	0xBB, 0xC0, 0x64, 0xE9, 0x4C, 0x56, 0x19, 0x9B, 0x1A, 0x4C, 0x46, 0x92, 0xCC, 0x79, 0xB1, 0x9B,
	0x7D, 0x2D, 0xA7, 0x63, 0x66, 0x65, 0x63, 0x4D, 0xF7, 0xE6, 0x5E, 0xD0, 0x18, 0xA0, 0x3D, 0xAE,
	0x95, 0x45, 0x8F, 0x4E, 0x7A, 0x6B, 0x51, 0xA1, 0x51, 0x52, 0xCA, 0x52, 0x44, 0x92, 0x37, 0x3F,
	0x58, 0x2C, 0x49, 0x2C, 0xC9, 0x38, 0xB6, 0xDC, 0xEE, 0x59, 0x6C, 0x00, 0x4B, 0xA5, 0x53, 0x8A,
	0x75, 0x4A, 0x88, 0x88, 0x48, 0x4A, 0xA1, 0x42, 0xD7, 0x22, 0x51, 0x29, 0x5A, 0xC5, 0x38, 0x49,
	0x4A, 0x8E, 0xF9, 0xDA, 0xC7, 0x7B, 0x5A, 0xC0, 0x28, 0xBD, 0x4F, 0x88, 0x9A, 0x8E, 0x8C, 0x36,
	0x4C, 0x20, 0x91, 0x29, 0xB6, 0x4D, 0xE2, 0xFD, 0xB3, 0x35, 0xD8, 0xEF, 0xC0, 0x06, 0x8D, 0x56,
	0x89, 0x01, 0x54, 0x29, 0xBC, 0x9E, 0x3A, 0x1B, 0x96, 0xDC, 0x99, 0x0B, 0x6D, 0x49, 0xCA, 0xE8,
	0x6D, 0xC3, 0x23, 0x4F, 0x04, 0x9D, 0xFF, 0x00, 0xF2, 0x01, 0x95, 0xB8, 0x74, 0xEA, 0x52, 0x6A,
	0xEF, 0x35, 0x4C, 0x8A, 0xEE, 0xA6, 0x4B, 0x2D, 0xA1, 0x0E, 0x11, 0xD9, 0x29, 0x53, 0x69, 0x33,
	0xB5, 0x8C, 0xB7, 0x80, 0xF3, 0x9A, 0x41, 0x02, 0x3C, 0x2A, 0x84, 0x65, 0xC3, 0x4A, 0x91, 0x1E,
	0x5C, 0x66, 0xE4, 0xA1, 0xB5, 0x2A, 0xE6, 0x82, 0x51, 0x1F, 0x73, 0x7D, 0xFB, 0x07, 0x46, 0xA9,
	0xF1, 0xA5, 0x7C, 0x9B, 0x72, 0x88, 0xF4, 0xAD, 0x0E, 0xAA, 0x3C, 0x93, 0xA1, 0xE4, 0xEA, 0xF2,
	0xD7, 0x06, 0xB9, 0x51, 0x57, 0x26, 0x79, 0xE4, 0xCA, 0xA7, 0xAA, 0x32, 0x4D, 0x09, 0xC8, 0x8D,
	0x56, 0xCF, 0xD8, 0x01, 0x68, 0xD2, 0x89, 0x4D, 0xC4, 0xA5, 0xA4, 0xE9, 0xCE, 0xAD, 0xE8, 0x86,
	0x64, 0xE9, 0xA8, 0x8C, 0x89, 0xE4, 0xE1, 0xC1, 0xD9, 0x7B, 0xE1, 0xB6, 0x7E, 0x41, 0xF2, 0x33,
	0x42, 0x1B, 0xE2, 0x32, 0xEA, 0x52, 0xC9, 0xCA, 0x63, 0x54, 0xDA, 0x74, 0x09, 0x2C, 0xC5, 0x43,
	0xA6, 0xFA, 0x8D, 0xD3, 0xC4, 0xA5, 0xAC, 0xCA, 0xDB, 0x88, 0xAC, 0x44, 0x59, 0x0C, 0x76, 0x92,
	0x62, 0x51, 0x91, 0xC9, 0x64, 0x78, 0x07, 0x7D, 0x43, 0x0D, 0xA4, 0x98, 0x94, 0x6E, 0x4D, 0x9F,
	0x1E, 0xA2, 0xC9, 0x3D, 0x3E, 0x97, 0x28, 0xEA, 0x09, 0x63, 0x55, 0xAC, 0x42, 0xEC, 0x87, 0x0C,
	0x8A, 0xC9, 0x5A, 0x8A, 0xD7, 0xB9, 0x65, 0xB0, 0xF3, 0xB0, 0xCA, 0x58, 0xC2, 0x6D, 0xDD, 0xAF,
	0x91, 0x9A, 0x12, 0xEF, 0x68, 0x33, 0xA5, 0x92, 0xA3, 0xD4, 0x21, 0x6A, 0x63, 0xC9, 0xE6, 0xD8,
	0xEC, 0x25, 0x95, 0x46, 0x3F, 0xF9, 0x2C, 0x93, 0x2B, 0x9E, 0x5B, 0x73, 0x2F, 0x40, 0xCA, 0x91,
	0x63, 0xB4, 0x93, 0x15, 0x04, 0xD5, 0xD4, 0xDA, 0x28, 0xAD, 0xB5, 0x1A, 0x49, 0x22, 0x9C, 0xEA,
	0x96, 0xBF, 0xB4, 0x49, 0xB8, 0x4B, 0x22, 0xF6, 0x05, 0x22, 0x6D, 0x24, 0xC4, 0xE7, 0x2A, 0xD0,
	0xE5, 0xC2, 0x99, 0x16, 0x74, 0x19, 0xD8, 0x1F, 0xA8, 0x2E, 0x6A, 0x4D, 0x93, 0x24, 0x99, 0x5C,
	0xAC, 0x44, 0x77, 0x23, 0xED, 0x31, 0xF2, 0x31, 0xA6, 0xF2, 0xFC, 0xB8, 0xA9, 0xA2, 0x6B, 0x31,
	0xA9, 0xD5, 0x68, 0x31, 0x22, 0xCA, 0xD5, 0x4D, 0xD4, 0xEA, 0xCD, 0xD3, 0x23, 0x52, 0x30, 0x1D,
	0xCE, 0xF6, 0x22, 0xBD, 0xFC, 0x83, 0x1B, 0xD2, 0xE2, 0xFB, 0x7E, 0x5C, 0x5A, 0x48, 0xD2, 0x6C,
	0x3F, 0xAE, 0x14, 0x39, 0x25, 0x53, 0x38, 0x27, 0x11, 0x4F, 0x15, 0xAC, 0xA3, 0xCA, 0xCE, 0x1E,
	0x57, 0xC5, 0x90, 0x5E, 0x97, 0x12, 0xFC, 0xB8, 0xB3, 0x6A, 0xF5, 0x77, 0xEA, 0xD4, 0xA8, 0x2C,
	0xCB, 0x6D, 0xF5, 0xCB, 0x8C, 0xA5, 0xDD, 0xE5, 0x16, 0x4B, 0x4A, 0x8E, 0xFE, 0x92, 0xB1, 0x10,
	0xCA, 0x5F, 0xBD, 0xBB, 0xB5, 0x94, 0xBF, 0x7B, 0xDD, 0xED, 0x62, 0xEA, 0x9C, 0xFD, 0x85, 0x7A,
	0x06, 0x57, 0x66, 0xC1, 0x95, 0xC9, 0xB0, 0x69, 0x69, 0x14, 0xE3, 0xAC, 0x56, 0xE4, 0x4F, 0x6D,
	0x87, 0x1B, 0x4B, 0xB8, 0x6C, 0x95, 0x15, 0xCC, 0xAC, 0x92, 0x2F, 0x70, 0x5D, 0x9B, 0x02, 0xE4,
	0xD8, 0x34, 0xE4, 0x69, 0x1B, 0x6E, 0x72, 0xA9, 0x69, 0x80, 0xEA, 0x6A, 0x72, 0xE3, 0x72, 0x67,
	0x5C, 0x35, 0xFE, 0x8E, 0xD6, 0x22, 0x35, 0x12, 0x6D, 0x7B, 0x99, 0x11, 0x6F, 0xCB, 0xFB, 0xFC,
	0x8C, 0x29, 0xBD, 0xF6, 0xE4, 0xD8, 0x21, 0x12, 0xB9, 0x10, 0x9B, 0xA7, 0x39, 0x3A, 0x9F, 0x21,
	0xE9, 0x54, 0xD2, 0xB3, 0x2A, 0x6D, 0xDC, 0x29, 0x59, 0x11, 0xDD, 0x24, 0xA2, 0xB1, 0xEC, 0xF2,
	0x0C, 0x6B, 0x03, 0x67, 0x3E, 0x00, 0xB4, 0x95, 0xC3, 0x28, 0x2B, 0x54, 0x75, 0x1B, 0xCC, 0x4A,
	0x75, 0xF7, 0x72, 0xEE, 0x5C, 0x27, 0x0F, 0xBA, 0x49, 0x17, 0x94, 0x8C, 0xCB, 0x88, 0x56, 0x06,
	0xCE, 0x7C, 0x16, 0x6A, 0x1A, 0x49, 0x02, 0xA2, 0x75, 0x04, 0xCB, 0x85, 0x2C, 0x9A, 0x94, 0xF3,
	0x6E, 0xA4, 0x9B, 0x5A, 0x52, 0x65, 0x85, 0x04, 0x9B, 0x19, 0x99, 0x1F, 0x60, 0xCA, 0x58, 0x46,
	0x6D, 0xDD, 0xAC, 0x63, 0x2C, 0x65, 0xDF, 0x06, 0x25, 0x56, 0xA4, 0xBA, 0xAD, 0x41, 0x0F, 0x6A,
	0x49, 0x96, 0x9B, 0x42, 0x5A, 0x69, 0xA2, 0x3B, 0x93, 0x68, 0x49, 0x64, 0x57, 0xDE, 0x3A, 0xB5,
	0x59, 0x26, 0x86, 0x9A, 0x58, 0xC6, 0x0C, 0x26, 0x8F, 0x61, 0x03, 0xD1, 0x34, 0xBA, 0xA8, 0xF2,
	0x4E, 0x80, 0x01, 0x4F, 0xF5, 0x78, 0x89, 0x36, 0x9F, 0x07, 0x3C, 0x9B, 0x24, 0x24, 0x48, 0x6C,
	0x00, 0x2B, 0xCC, 0xD8, 0x8E, 0x3E, 0xE1, 0x7A, 0xC5, 0xEF, 0x39, 0x66, 0x8B, 0x6B, 0xF0, 0x73,
	0xC9, 0x58, 0x5E, 0x44, 0x00, 0x12, 0xFF, 0x00, 0x57, 0x88, 0x93, 0x69, 0xEE, 0x93, 0x9E, 0x4C,
	0xE4, 0x24, 0x48, 0x6C, 0x00, 0x11, 0x2F, 0xBD, 0x47, 0x9C, 0xC5, 0xEB, 0x17, 0xBC, 0xE5, 0x9B,
	0xD3, 0x7B, 0x3F, 0xDE, 0xFE, 0xDC, 0xD5, 0x45, 0xE7, 0xA6, 0x00, 0x14, 0xFF, 0x00, 0x57, 0x88,
	0x93, 0x69, 0xF0, 0x73, 0xC9, 0x9C, 0x84, 0x89, 0x0D, 0x80, 0x05, 0x79, 0x9B, 0x11, 0xC7, 0xDC,
	0x2F, 0x58, 0xBD, 0xE7, 0x2C, 0xD1, 0x6D, 0x7E, 0x0E, 0x79, 0x2B, 0x0B, 0xC8, 0x80, 0x07, 0xAC,
	0xE9, 0xA2, 0x7E, 0xAF, 0x3F, 0xBF, 0xF9, 0x44, 0x8E, 0xAC, 0x8F, 0x8F, 0xCB, 0xD5, 0xB2, 0xF8,
	0xE9, 0xA2, 0x7E, 0xAF, 0x3F, 0xBF, 0xF9, 0x43, 0xAB, 0x23, 0xE3, 0xF2, 0xF5, 0x2F, 0x96, 0xEE,
	0x98, 0x92, 0xAD, 0x6A, 0x7E, 0xCF, 0xB7, 0xF9, 0x44, 0x9B, 0x4E, 0xCC, 0xF7, 0x3E, 0xFE, 0x3F,
	0x87, 0xE9, 0xF3, 0x6C, 0x92, 0x72, 0xFA, 0x5D, 0xFC, 0xBF, 0xF1, 0xFE, 0x51, 0x23, 0xAA, 0xFE,
	0xBF, 0x2F, 0x56, 0xCB, 0xE3, 0xA5, 0xDF, 0xCB, 0xFF, 0x00, 0x1F, 0xE5, 0x0E, 0xAB, 0xFA, 0xFC,
	0xBD, 0x4B, 0xE6, 0x35, 0xA4, 0x05, 0x34, 0x8E, 0xF1, 0x4D, 0xBC, 0x1F, 0x6B, 0x7B, 0xDF, 0x81,
	0x76, 0x0A, 0xF6, 0x66, 0xA5, 0xB2, 0xBF, 0xF7, 0xAB, 0x5A, 0x7E, 0x1F, 0xAF, 0xCD, 0xAE, 0x7B,
	0x3F, 0xA7, 0xF1, 0x5D, 0xBB, 0xF2, 0xAE, 0xFE, 0x70, 0xC0, 0xCE, 0x71, 0x4F, 0x82, 0x3F, 0x5B,
	0xE0, 0x2B, 0x6C, 0x63, 0x8B, 0x5F, 0xD9, 0xBF, 0xCD, 0xFE, 0x3F, 0xD8, 0x73, 0x8A, 0x7C, 0x11,
	0xFA, 0xDF, 0x00, 0xD8, 0xC7, 0x13, 0xEC, 0xDF, 0xE6, 0xFF, 0x00, 0x1F, 0xEC, 0xA9, 0x3A, 0xAE,
	0x4D, 0x6A, 0xED, 0x1F, 0x15, 0xEF, 0xD7, 0xB7, 0x67, 0x90, 0x4D, 0xB4, 0x35, 0x5B, 0xF7, 0x7B,
	0x71, 0xC9, 0xA3, 0x4F, 0x62, 0xF4, 0x7A, 0x7F, 0x92, 0xB5, 0xF9, 0x7A, 0xAA, 0x73, 0xEF, 0xF0,
	0xBF, 0x89, 0xF0, 0x13, 0x3A, 0x17, 0xD5, 0xE5, 0xEA, 0xE7, 0xEA, 0xFF, 0x00, 0xAB, 0xCB, 0xD4,
	0x73, 0xEF, 0xF0, 0xBF, 0x89, 0xF0, 0x0E, 0x85, 0xF5, 0x79, 0x7A, 0x9D, 0x5F, 0xF5, 0x79, 0x7A,
	0xAD, 0xC1, 0x7C, 0xAA, 0xA4, 0xE1, 0x1A, 0x4D, 0x9D, 0x55, 0xB7, 0xE2, 0xBD, 0xEF, 0xE6, 0xEC,
	0x14, 0xAC, 0xF9, 0x3A, 0x3D, 0xEE, 0xDA, 0xD6, 0x99, 0xBA, 0x34, 0x3A, 0xD7, 0x55, 0xD7, 0xB2,
	0xF5, 0xEE, 0x54, 0xA7, 0xFE, 0xE2, 0xB7, 0xCD, 0xC9, 0xF0, 0xA7, 0xEA, 0xFC, 0x45, 0x3D, 0xB4,
	0x70, 0x6F, 0xFB, 0x49, 0xF9, 0x5F, 0xCB, 0xFA, 0x8E, 0x6E, 0x4F, 0x85, 0x3F, 0x57, 0xE2, 0x1B,
	0x68, 0xE0, 0x7D, 0xA4, 0xFC, 0xAF, 0xE5, 0xFD, 0x59, 0xB5, 0x84, 0x14, 0x2D, 0x4D, 0x8C, 0xDC,
	0xC7, 0x8B, 0xC9, 0x6B, 0x5B, 0xFC, 0x8E, 0x1D, 0x77, 0xFC, 0xB7, 0x7F, 0x0A, 0x57, 0x27, 0x6E,
	0xA9, 0x6E, 0x6D, 0xAF, 0x7F, 0x8E, 0x94, 0xA7, 0xE3, 0xFA, 0xFC, 0x99, 0xBC, 0xB3, 0xEC, 0xFF,
	0x00, 0xAB, 0xE0, 0x27, 0xEC, 0x3E, 0x6E, 0xEE, 0xB4, 0xFA, 0x3C, 0xFD, 0x07, 0x2C, 0xFB, 0x3F,
	0xEA, 0xF8, 0x06, 0xC3, 0xE6, 0x75, 0xA7, 0xD1, 0xE7, 0xE8, 0xD2, 0xA3, 0xD2, 0xCA, 0xBA, 0x4F,
	0x5D, 0xE3, 0x8F, 0xA8, 0xC3, 0xD4, 0xC7, 0x8B, 0x15, 0xFC, 0xA5, 0x6E, 0xF7, 0xDA, 0x3B, 0xF5,
	0x2D, 0x3F, 0x45, 0xBD, 0xD9, 0x5A, 0xD3, 0xE5, 0xBA, 0xBF, 0xAE, 0x2E, 0x2D, 0x6F, 0x58, 0xE9,
	0x37, 0x7B, 0x29, 0x4A, 0xFC, 0xFF, 0x00, 0xE3, 0x4B, 0xA1, 0x69, 0xFA, 0xC0, 0xFE, 0xE3, 0xE6,
	0x1D, 0xFD, 0x67, 0x1F, 0x07, 0x9F, 0xA3, 0x86, 0xE0, 0xE8, 0x5A, 0x7E, 0xB0, 0x3F, 0xB8, 0xF9,
	0x83, 0xAC, 0xE3, 0xE0, 0xF3, 0xF4, 0x2E, 0x3C, 0xA5, 0x8C, 0x51, 0xE9, 0x1A, 0x2F, 0x13, 0x58,
	0xB1, 0x87, 0x48, 0xD1, 0x78, 0x87, 0xC3, 0x23, 0xEC, 0x13, 0x6D, 0x09, 0xA1, 0xA4, 0xBB, 0x73,
	0xB6, 0x95, 0xC9, 0x9C, 0x91, 0x84, 0x17, 0xE2, 0xD0, 0xAA, 0x93, 0x23, 0xA5, 0xF8, 0xD0, 0x9D,
	0x75, 0xA5, 0xDF, 0x0A, 0xD2, 0x59, 0x1D, 0x8E, 0xC7, 0xED, 0x21, 0x2E, 0x30, 0xA6, 0xF6, 0xD3,
	0x7A, 0x33, 0x5A, 0xFA, 0xB5, 0xFF, 0x00, 0x40, 0xF8, 0x2E, 0xD3, 0xB4, 0x7A, 0xAE, 0xDE, 0xB3,
	0x1C, 0x07, 0x93, 0x7B, 0x5A, 0xE5, 0xE7, 0x1D, 0x9A, 0xA6, 0x92, 0x59, 0x2F, 0x5E, 0x8E, 0x19,
	0xBB, 0xF5, 0x2D, 0x24, 0x92, 0x5E, 0xBD, 0x1A, 0x6E, 0xCD, 0x73, 0x98, 0xAA, 0x9E, 0x24, 0xEF,
	0xA0, 0x76, 0x6D, 0xF4, 0x78, 0xA8, 0x74, 0x9D, 0x17, 0x89, 0x95, 0xCA, 0x59, 0xF0, 0x84, 0x36,
	0x5E, 0x83, 0xEE, 0xDF, 0x47, 0x8A, 0x9D, 0x41, 0xC4, 0x3B, 0xAB, 0xC0, 0xA2, 0x55, 0xAF, 0x7B,
	0x70, 0x1C, 0x7A, 0xDC, 0x23, 0x3D, 0xDB, 0xBF, 0x3C, 0x9C, 0x1A, 0xEC, 0xF2, 0xCF, 0x76, 0xEC,
	0x6B, 0xBF, 0x25, 0x2B, 0x1F, 0x60, 0xE3, 0xD9, 0x4F, 0x82, 0x7D, 0xD8, 0x8B, 0x18, 0xF9, 0x1D,
	0x1C, 0xD0, 0x85, 0x63, 0x02, 0x91, 0x83, 0x5F, 0x47, 0xE5, 0x33, 0x1B, 0x94, 0x6B, 0x9C, 0x4A,
	0x31, 0x60, 0xB5, 0xF7, 0xDB, 0x17, 0xF9, 0x19, 0xE8, 0xA6, 0x84, 0x2B, 0x54, 0xCB, 0x43, 0x45,
	0x3E, 0x92, 0xED, 0xC8, 0x56, 0x95, 0xC9, 0xB1, 0xCE, 0x70, 0xBC, 0x61, 0x03, 0x76, 0xD2, 0x5C,
	0x53, 0x3A, 0x2E, 0x9B, 0xC2, 0x39, 0xCE, 0x17, 0x8C, 0x20, 0x36, 0x92, 0xE2, 0x74, 0x5D, 0x37,
	0x85, 0x8F, 0xA4, 0x12, 0x98, 0x91, 0xC9, 0xB5, 0x2E, 0xA5, 0x78, 0x71, 0xDE, 0xDB, 0xBB, 0xD1,
	0xA7, 0x4B, 0x34, 0x23, 0x4A, 0x29, 0xD9, 0xFA, 0x29, 0xF4, 0x77, 0xAF, 0xC2, 0x95, 0xA6, 0x6C,
	0x72, 0x32, 0x3D, 0x83, 0x4A, 0x93, 0xE9, 0x11, 0x9E, 0xC0, 0x1E, 0x8F, 0x44, 0x66, 0xC6, 0x83,
	0xCB, 0x39, 0x53, 0xC9, 0x6B, 0x1E, 0xAF, 0x0E, 0x2D, 0xF6, 0xC5, 0x7F, 0xEE, 0x40, 0x3D, 0x1F,
	0x3E, 0x53, 0x3C, 0x75, 0xAF, 0x48, 0x03, 0x9F, 0x29, 0x9E, 0x3A, 0xD7, 0xA4, 0x07, 0x3D, 0x1D,
	0xAE, 0x60, 0x00, 0x01, 0xD5, 0x34, 0x23, 0xE8, 0xA4, 0x2F, 0x3B, 0x9F, 0x98, 0xA1, 0xCB, 0xA4,
	0xF7, 0xA2, 0xDF, 0x2E, 0xE6, 0xF0, 0xC1, 0x90, 0x00, 0x00, 0xE1, 0x85, 0xB0, 0x85, 0x56, 0xF8,
	0x00, 0x7D, 0x00, 0x05, 0x77, 0xA6, 0x35, 0x69, 0xFE, 0x1C, 0x58, 0xCD, 0xB8, 0xB1, 0x3D, 0xA4,
	0x00, 0x00, 0x45, 0x7B, 0x80, 0x08, 0xDE, 0x01, 0xA8, 0xDE, 0x00, 0x5E, 0xE0, 0x10, 0x00, 0x00,
	0x68, 0xED, 0x73, 0x00, 0x00, 0x0E, 0xA9, 0xA1, 0x1F, 0x45, 0x21, 0x79, 0xDC, 0xFC, 0xC5, 0x0E,
	0x5D, 0x27, 0xBD, 0x16, 0xF9, 0x77, 0x37, 0x86, 0x0C, 0x80, 0x00, 0x07, 0x0C, 0x2D, 0x84, 0x2A,
	0xB7, 0xC0, 0x03, 0xE8, 0x00, 0x2B, 0xBD, 0x31, 0xAB, 0x4F, 0xF0, 0xE2, 0xC6, 0x6D, 0xC5, 0x89,
	0xED, 0x20, 0x00, 0x02, 0x2B, 0xDC, 0x00, 0x46, 0xF0, 0x0D, 0x46, 0xF0, 0x02, 0xF7, 0x00, 0x80,
	0x00, 0x03, 0x47, 0x6B, 0x98, 0x00, 0x00, 0x75, 0x4D, 0x08, 0xFA, 0x29, 0x0B, 0xCE, 0xE7, 0xE6,
	0x28, 0x72, 0xE9, 0x3D, 0xE8, 0xB7, 0xCB, 0xB9, 0xBC, 0x30, 0x64, 0x00, 0x00, 0x38, 0x61, 0x6C,
	0x21, 0x55, 0xBE, 0x00, 0x1F, 0x40, 0x01, 0x5D, 0xE9, 0x8D, 0x5A, 0x7F, 0x87, 0x16, 0x33, 0x6E,
	0x2C, 0x4F, 0x69, 0x00, 0x00, 0x11, 0x5E, 0xE0, 0x02, 0x37, 0x80, 0x6A, 0x37, 0x80, 0x17, 0xB8,
	0x04, 0x00, 0x00, 0x1A, 0x3B, 0x5C, 0xC0, 0x00, 0x03, 0xAA, 0x68, 0x47, 0xD1, 0x48, 0x5E, 0x77,
	0x3F, 0x31, 0x43, 0x97, 0x49, 0xEF, 0x45, 0xBE, 0x5D, 0xCD, 0xE1, 0x83, 0x20, 0x00, 0x01, 0xC3,
	0x0B, 0x61, 0x0A, 0xAD, 0xF0, 0x00, 0xFA, 0x00, 0x0A, 0xEF, 0x4C, 0x6A, 0xD3, 0xFC, 0x38, 0xB1,
	0x9B, 0x71, 0x62, 0x7B, 0x48, 0x00, 0x00, 0x8A, 0xF7, 0x00, 0x11, 0xBC, 0x03, 0x51, 0xBC, 0x00,
	0xBD, 0xC0, 0x20, 0x00, 0x01, 0xFF, 0xD9
};

uint32_t gimage_id; // Global image ID (for all image threads)
mutex_t camera_mtx;
bool camera_mtx_init = false;

ssdv_packet_t packetRepeats[16];
bool reject_pri;
bool reject_sec;

static bool transmit_image_packet(const uint8_t *image,
                                  uint32_t image_len,
                                  thd_img_conf_t* conf,
                                  uint8_t image_id,
                                  uint16_t packet_id) {
	ssdv_t ssdv;
	uint8_t pkt[SSDV_PKT_SIZE];
	uint8_t pkt_base91[256] = {0};
	const uint8_t *b;
	uint32_t bi = 0;
	uint8_t c = SSDV_OK;
	uint16_t i = 0;

	// Init SSDV (FEC at 2FSK, non FEC at APRS)
	bi = 0;
	ssdv_enc_init(&ssdv, SSDV_TYPE_PADDING, "N0CALL", image_id, conf->quality);
	ssdv_enc_set_buffer(&ssdv, pkt);

	while(true)
	{
		while((c = ssdv_enc_get_packet(&ssdv)) == SSDV_FEED_ME)
		{
			b = &image[bi];
			uint8_t r = bi < image_len-128 ? 128 : image_len - bi;
			bi += r;

			if(r <= 0)
			{
				TRACE_ERROR("SSDV > Premature end of file");
				return false;
			}
			ssdv_enc_feed(&ssdv, b, r);
		}

		if(c == SSDV_EOI) {
			return true;
		} else if(c != SSDV_OK) {
			return false;
		}

		if(i == packet_id) {
			// Sync byte, CRC and FEC of SSDV not transmitted (because its not necessary inside an APRS packet)
			base91_encode(&pkt[6], pkt_base91, 174);
			packet_t packet = aprs_encode_data_packet(conf->call, conf->path, 'I', pkt_base91);
            if(packet == NULL) {
              TRACE_WARN("IMG  > No free packet objects for transmission");
              return false;
            }
            if(!transmitOnRadio(packet,
                                conf->radio_conf.freq,
                                0,
                                0,
                                conf->radio_conf.pwr,
                                conf->radio_conf.mod,
                                conf->radio_conf.rssi)) {

              TRACE_ERROR("IMG  > Unable to send image packet TX on radio");
              return false;
            }
		}

		chThdSleep(TIME_MS2I(10)); // Leave other threads some time

		i++;
	}
}

/*
 * Transmit image packets.
 * Return true if no SSDV encoding error or false on encoding error.
 */
static bool transmit_image_packets(const uint8_t *image,
                                   uint32_t image_len,
                                   thd_img_conf_t* conf,
                                   uint8_t image_id) {

  uint8_t pkt[SSDV_PKT_SIZE];
  uint8_t pkt_base91[256] = {0};


  /* FIXME: This doesn't work with 'en bloc' packet sends. */
  // Process redundant transmission from last cycle
  if(strlen((char*)pkt_base91)
      && conf->radio_conf.redundantTx) {
    packet_t packet = aprs_encode_data_packet(conf->call, conf->path,
                                              'I', pkt_base91);
    if(packet == NULL) {
      TRACE_ERROR("IMG  > No available packet for redundant"
          " image transmission");
    } else {
      if(!transmitOnRadio(packet,
                          conf->radio_conf.freq,
                          0,
                          0,
                          conf->radio_conf.pwr,
                          conf->radio_conf.mod,
                          conf->radio_conf.rssi)) {
        /* Packet has been released by transmit. */
        TRACE_ERROR("IMG  > Unable to send redundant image on radio");
      }
    }
    chThdSleep(TIME_MS2I(10)); // Leave other threads some time
  }

  // Init SSDV (FEC at 2FSK, non FEC at APRS)
  //bi = 0;


  /* Prepare for image encode and send. */
  ssdv_t ssdv;
  const uint8_t *b;
  uint32_t bi = 0;
  uint8_t c = SSDV_OK;

  /* Initialize SSDV, output buffer and input buffer. */
  ssdv_enc_init(&ssdv, SSDV_TYPE_PADDING, "N0CALL", image_id, conf->quality);
  ssdv_enc_set_buffer(&ssdv, pkt);
  ssdv_enc_feed(&ssdv, image, 0);

  while(c != SSDV_EOI) {
    // Encode packet(s)
    /* Packet burst send is available if redundant TX is not requested. */
    uint8_t chain = (conf->radio_conf.mod == MOD_2FSK
        && !conf->radio_conf.redundantTx) ?
        (NUMBER_COMMON_PKT_BUFFERS / 2) : 1;
    TRACE_INFO("IMG  > Encode APRS/SSDV packet(s) %i", chain);

    /* Packet linking control. */
    packet_t head = NULL;
    packet_t previous = NULL;

    while(chain-- > 0) {
      /* TODO: re-implement chunk sized SSDV conversions? */
      while((c = ssdv_enc_get_packet(&ssdv)) == SSDV_FEED_ME) {
        b = &image[bi++];
        if(bi > image_len) {
          TRACE_ERROR("SSDV > Premature end of file");
          if(head != NULL) {
            pktReleaseBufferChain(head);
          }
          return false;
        }
        ssdv_enc_feed(&ssdv, b, 1);
      }

      if(c == SSDV_EOI) {
        TRACE_INFO("SSDV > ssdv_enc_get_packet said EOI");
        break;
      } else if(c != SSDV_OK) {
        TRACE_ERROR("SSDV > ssdv_enc_get_packet failed: %i", c);
        if(head != NULL) {
          pktReleaseBufferChain(head);
        }
        return false;
      }

      /*
       * Sync byte, CRC and FEC of SSDV not transmitted.
       * Not necessary inside an APRS packet.
       */
      base91_encode(&pkt[6], pkt_base91, 174);

      packet_t packet = aprs_encode_data_packet(conf->call, conf->path,
                                                'I', pkt_base91);
      if(packet == NULL) {
        TRACE_ERROR("IMG  > No available packet for image transmission");
        /* Error so release any linked packets. */
        if(head != NULL) {
          pktReleaseBufferChain(head);
        }
        return false;
      }
      if(head == NULL)
        head = packet;
      if(previous != NULL)
        /* Link the next packet into the chain. */
        previous->nextp = packet;
      /* Now set previous as current. */
      previous = packet;
    } /* End while(chain-- > 0) */

    /* If we have some image packet(s) to transmit then do it. */
    if(head != NULL) {
      if(!transmitOnRadio(head,
                          conf->radio_conf.freq,
                          0,
                          0,
                          conf->radio_conf.pwr,
                          conf->radio_conf.mod,
                          conf->radio_conf.rssi)) {
        TRACE_ERROR("IMG  > Unable to send image on radio");
        /* Transmit on radio will release the packet chain. */
      }
    }
    chThdSleep(TIME_MS2I(10)); // Leave other threads some time
  } /* End while(c!= SSDV_EOI) */

  // Repeat packets
  for(uint8_t i=0; i<16; i++) {
    if(packetRepeats[i].n_done && image_id == packetRepeats[i].image_id) {
      if(!transmit_image_packet(image, image_len, conf,
                                image_id, packetRepeats[i].packet_id)) {
        TRACE_ERROR("IMG  > Failed re-send of image %i", image_id);
      } else {
        packetRepeats[i].n_done = false; // Set done
      }
    }
    chThdSleep(TIME_MS2I(100)); // Leave other threads some time
  }

  // Packet spacing (delay)
  if(conf->thread_conf.send_spacing)
    chThdSleep(conf->thread_conf.send_spacing);

  // Handle image rejection flag
  if((conf == &conf_sram.img_pri) && reject_pri) { // Image rejected
    reject_pri = false;
  }
  if((conf == &conf_sram.img_sec) && reject_sec) { // Image rejected
    reject_sec = false;
  }
  return true;
}

/**
  * Analyzes the image for JPEG errors. Returns true if the image is error free.
  */
static bool analyze_image(uint8_t *image, uint32_t image_len)
{
	return true;
	#if !OV5640_USE_DMA_DBM
	if(image_len >= 65535)
	{
		TRACE_ERROR("CAM  > Camera has %d bytes allocated but DMA DBM not activated", image_len);
		TRACE_ERROR("CAM  > DMA can only use 65535 bytes only");
		image_len = 65535;
	}
	#endif

	ssdv_t ssdv;
	uint8_t pkt[SSDV_PKT_SIZE];
	uint8_t *b;
	uint32_t bi = 0;
	uint16_t i = 0;
	uint8_t c = SSDV_OK;

	ssdv_enc_init(&ssdv, SSDV_TYPE_NOFEC, "", 0, 7);
	ssdv_enc_set_buffer(&ssdv, pkt);

	while(true) // FIXME: I get caught in these loops occasionally and never return
	{
		while((c = ssdv_enc_get_packet(&ssdv)) == SSDV_FEED_ME)
		{
			b = &image[bi];
			uint8_t r = bi < image_len-128 ? 128 : image_len - bi;
			bi += r;
			if(r <= 0)
			{
				TRACE_ERROR("CAM  > Error in image (Premature end of file %d)", i);
				return false;
			}
			ssdv_enc_feed(&ssdv, b, r);
		}

		if(c == SSDV_EOI) // End of image
			return true;

		if(c != SSDV_OK) // Error in JPEG image
		{
			TRACE_ERROR("CAM  > Error in image (ssdv_enc_get_packet failed: %d %d)", c, i);
			return false;
		}

		i++;
		chThdSleep(TIME_MS2I(5));
	}
}

static bool camInitialized = false;

uint32_t takePicture(uint8_t* buffer, uint32_t size,
                     resolution_t res, bool enableJpegValidation) {
	uint32_t size_sampled = 0;

	// Initialize mutex
	if(!camera_mtx_init)
		chMtxObjectInit(&camera_mtx);
	camera_mtx_init = true;

	// Lock camera
	TRACE_INFO("IMG  > Lock camera");
	chMtxLock(&camera_mtx);

	// Detect camera
	if(camInitialized || OV5640_isAvailable()) { // OV5640 available

		TRACE_INFO("IMG  > OV5640 found");

		uint8_t cntr = 5;
		bool jpegValid;
		do {
			// Init camera
			if(!camInitialized) {
				OV5640_init();
				camInitialized = true;
			}

			// Sample data from pseudo DCMI through DMA into RAM
			size_sampled = OV5640_Snapshot2RAM(buffer, size, res);

			// Switch off camera
			if(!conf_sram.keep_cam_switched_on) {
				OV5640_deinit();
				camInitialized = false;
			}

			// Validate JPEG image
			if(enableJpegValidation)
			{
				TRACE_INFO("CAM  > Validate integrity of JPEG");
				jpegValid = analyze_image(buffer, size);
				TRACE_INFO("CAM  > JPEG image %s", jpegValid ? "valid" : "invalid");
			} else {
				jpegValid = true;
			}
		} while(!jpegValid && cntr--);

	} else { // Camera not found

		camInitialized = false;
		TRACE_ERROR("IMG  > No camera found");

	}

	// Unlock camera
	TRACE_INFO("IMG  > Unlock camera");
	chMtxUnlock(&camera_mtx);

	return size_sampled;
}

THD_FUNCTION(imgThread, arg) {
  thd_img_conf_t* conf = (thd_img_conf_t*)arg;

  if(conf->thread_conf.init_delay) chThdSleep(conf->thread_conf.init_delay);
  TRACE_INFO("IMG  > Startup image thread");

  // Create buffer
  //uint8_t buffer[conf->buf_size] __attribute__((aligned(DMA_FIFO_BURST_ALIGN)));

  sysinterval_t time = chVTGetSystemTime();
  while(true) {
    TRACE_INFO("IMG  > Do module IMAGE cycle");
    if(!p_sleep(&conf->thread_conf.sleep_conf)) {
      // Take picture

      /* Create image capture buffer. */
      uint8_t *buffer = chHeapAllocAligned(NULL, conf->buf_size,
                                           DMA_FIFO_BURST_ALIGN);
      if(buffer != NULL) {
        uint32_t size_sampled = takePicture(buffer, conf->buf_size,
                                            conf->res, true);
        gimage_id++; // Increase SSDV image counter

        // Radio transmission
        if(size_sampled) {
          // Write picture to SD card
          TRACE_INFO("IMG  > Save image to SD card");
          if(initSD())
          {
            char filename[32];
            chsnprintf(filename, sizeof(filename), "r%02xi%04x.jpg",
                       getLastDataPoint()->reset % 0xFF,
                       (gimage_id-1) % 0xFFFF);

            // Find SOI
            uint32_t soi;
            for(soi=0; soi<conf->buf_size; soi++) {
              if(buffer[soi] == 0xFF && buffer[soi+1] == 0xD8)
                break;
            }
            TRACE_DEBUG("IMG  > SOI at byte %d");
            writeBufferToFile(filename, &buffer[soi], size_sampled-soi);
          }

          if(conf->radio_conf.mod == MOD_2FSK && conf->radio_conf.redundantTx) {
            TRACE_WARN("IMG  > Redundant TX disables 2FSK burst send mode");
          }
          // Encode and transmit picture
          TRACE_INFO("IMG  > Encode/Transmit SSDV ID=%d", gimage_id-1);
          if(!transmit_image_packets(buffer, size_sampled, conf,
                                     (uint8_t)(gimage_id-1))) {
            TRACE_ERROR("IMG  > Error in encoding snapshot image"
                " %i - discarded", gimage_id-1);
            /* Allow time for output queue to be processed. */
            chThdSleep(TIME_S2I(10));
          }
        } else { // No camera found
          TRACE_INFO("IMG  > Encode/Transmit SSDV (camera error) ID=%d",
                     gimage_id-1);
          if(!transmit_image_packets(noCameraFound, sizeof(noCameraFound),
                                     conf, (uint8_t)(gimage_id-1))) {
            TRACE_ERROR("IMG  > Error in encoding dummy image %i"
                " - discarded", gimage_id-1);
            /* Allow time for output queue to be processed. */
            chThdSleep(TIME_S2I(10));
          }
        }
        /* Return the buffer to the heap. */
        chHeapFree(buffer);
      } else {
        /* Could not get a capture buffer. */
        TRACE_WARN("IMG  > Unable to get capture buffer for image %i",
                   gimage_id-1);
      }
    }
    chThdSleep(TIME_MS2I(10));
    time = waitForTrigger(time, conf->thread_conf.cycle);
  }
}

/*
 *
 */
void start_image_thread(thd_img_conf_t *conf)
{
	thread_t *th = chThdCreateFromHeap(NULL,
	                                   THD_WORKING_AREA_SIZE(40 * 1024),
	                                   "IMG", LOWPRIO, imgThread, conf);
	if(!th) {
		// Print startup error, do not start watchdog for this thread
		TRACE_ERROR("IMG  > Could not startup thread"
		    " (not enough memory available)");
	}
}

