/*! \file oled_display_test.c
*
*   \brief Functions that setup and controls oled display module for
*          displaying text on it.
*
*/

#include "oled_display_test.h"

Int8 totalcols;
Int8 row;
Int8 dir;
Int8 col[2];
Int8 rolling0;
Int8 rolling1;

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// OLED constructor is called).
//
//  Font data for Arial 8pt
//

/** \brief  Character bitmaps for Arial 8pt
*/
static const Uint8 arial_8ptBitmaps[] =
{
	// @0 ' ' (2 pixels wide)
	//
	//
	//
	//
	//
	//
	//
	//
	//
	//
	//
	//
	//
	//
	0x00, 0x00,
	0x00, 0x00,

	// @4 '!' (1 pixels wide)
	// #
	//
	// #
	// #
	// #
	// #
	// #
	// #
	0xFD,

	// @5 '"' (3 pixels wide)
	// # #
	// # #
	// # #
	0x70, 0x00, 0x70,

	// @8 '#' (5 pixels wide)
	// # #
	// # #
	// #####
	//  # #
	//  # #
	// #####
	//   # #
	//   # #
	0x27, 0x3C, 0xE7, 0x3C, 0xE4,

	// @14 '$' (5 pixels wide)
	//   #
	//  ###
	// # # #
	//   ##
	//  ##
	// # # #
	//  ###
	//   #
	0x24, 0x52, 0xFF, 0x4A, 0x24,
	0x00, 0x00, 0x00, 0x00, 0x00,

	// @23 '%' (9 pixels wide)
	//   #   ##
	//    # #  #
	//    # #  #
	//     # ##
	//  ## #
	// #  # #
	// #  # #
	//  ##   #
	0x60, 0x90, 0x91, 0x66, 0x18, 0x66, 0x89, 0x09, 0x06,

	// @32 '&' (6 pixels wide)
	//  ### #
	// #   #
	// #   ##
	//  # #
	//   ##
	//  #  #
	//  #  #
	//   ##
	0x06, 0x69, 0x91, 0x99, 0x66, 0x05,

	// @38 ''' (1 pixels wide)
	//
	//
	//
	//
	//
	// #
	// #
	// #
	0xE0,

	// @39 '(' (3 pixels wide)
    //   #
    //  #
    // #
    // #
    // #
    // #
    //  #
    //   #
	0x3C, 0x42, 0x81,
	0x00, 0x00, 0x00,

	// @45 ')' (3 pixels wide)
	// @39 '(' (3 pixels wide)
	// #
	//  #
	//   #
	//   #
	//   #
	//   #
	//  #
	// #
	0x81, 0x42, 0x3C,
	0x00, 0x00, 0x00,

	// @51 '*' (3 pixels wide)
	// # #
	//  #
	// ###
	//  #
	0x05, 0x0E, 0x05,

	// @54 '+' (5 pixels wide)
	//   #
	//   #
	// #####
	//   #
	//   #
	0x04, 0x04, 0x1F, 0x04, 0x04,

	// @59 ',' (1 pixels wide)
	// #
	// #
	// #
	0x07,

	// @60 '-' (3 pixels wide)
	// ###
	0x01, 0x01, 0x01,

	// @63 '.' (1 pixels wide)
	// #
	0x01,

	// @64 '/' (3 pixels wide)
	// #
	// #
	//  #
	//  #
	//  #
	//  #
	//   #
	//   #
	0x03, 0x3C, 0xC0,

	// @67 '0' (5 pixels wide)
	//  ###
	// #   #
	// #   #
	// #   #
	// #   #
	// #   #
	// #   #
	//  ###
	0x7E, 0x81, 0x81, 0x81, 0x7E,

	// @72 '1' (3 pixels wide)
	//   #
	//   #
	//   #
	//   #
	//   #
	// # #
	//  ##
	//   #
	0x20, 0x40, 0xFF,

	// @75 '2' (5 pixels wide)
	// #####
	//  #
	//   #
	//    #
	//     #
	//     #
	// #   #
	//  ###
	0x41, 0x83, 0x85, 0x89, 0x71,

	// @80 '3' (5 pixels wide)
	//  ###
	// #   #
	//     #
	//     #
	//   ##
	//     #
	// #   #
	//  ###
	0x42, 0x81, 0x91, 0x91, 0x6E,

	// @85 '4' (5 pixels wide)
	//    #
	//    #
	// #####
	// #  #
	//  # #
	//  # #
	//   ##
	//    #
	0x0C, 0x34, 0x44, 0xFF, 0x04,

	// @90 '5' (5 pixels wide)
	//  ###
	// #   #
	//     #
	//     #
	// ####
	// #
	//  #
	//  ####
	0x32, 0xD1, 0x91, 0x91, 0x8E,

	// @95 '6' (5 pixels wide)
	//  ###
	// #   #
	// #   #
	// #   #
	// ####
	// #
	// #   #
	//  ###
	0x7E, 0x91, 0x91, 0x91, 0x4E,

	// @100 '7' (5 pixels wide)
	//  #
	//  #
	//  #
	//   #
	//   #
	//    #
	//    #
	// #####
	0x80, 0x87, 0x98, 0xE0, 0x80,

	// @105 '8' (5 pixels wide)
	//  ###
	// #   #
	// #   #
	// #   #
	//  ###
	// #   #
	// #   #
	//  ###
	0x6E, 0x91, 0x91, 0x91, 0x6E,

	// @110 '9' (5 pixels wide)
	//  ###
	// #   #
	//     #
	//  ####
	// #   #
	// #   #
	// #   #
	//  ###
	0x72, 0x89, 0x89, 0x89, 0x7E,

	// @115 ':' (1 pixels wide)
	// #
	//
	//
	//
	//
	// #
	0x21,

	// @116 ';' (1 pixels wide)
	// #
	// #
	// #
	//
	//
	//
	//
	// #
	0x87,

	// @117 '<' (5 pixels wide)
	//    ##
	//  ##
	// #
	//  ##
	//    ##
	0x04, 0x0A, 0x0A, 0x11, 0x11,

	// @122 '=' (5 pixels wide)
	// #####
	//
	// #####
	0x05, 0x05, 0x05, 0x05, 0x05,

	// @127 '>' (5 pixels wide)
	// ##
	//   ##
	//     #
	//   ##
	// ##
	0x11, 0x11, 0x0A, 0x0A, 0x04,

	// @132 '?' (5 pixels wide)
	//   #
	//
	//   #
	//   #
	//    #
	//     #
	// #   #
	//  ###
	0x40, 0x80, 0x8D, 0x90, 0x60,

	// @137 '@' (10 pixels wide)
	//   #######
	//  #       #
	// #  ## ##
	// # #  ## #
	// #  #  ## #
	// #   ## # #
	//  #       #
	//   #######
	0x3C, 0x42, 0x89, 0x95, 0xA5, 0xA9, 0x9D, 0xB5, 0x89, 0x72,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	// @157 'A' (7 pixels wide)
	// #     #
	// #     #
	//  #####
	//  #   #
	//   # #
	//   # #
	//   # #
	//    #
	0x03, 0x0C, 0x74, 0x84, 0x74, 0x0C, 0x03,

	// @164 'B' (6 pixels wide)
	// #####
	// #    #
	// #    #
	// #    #
	// ######
	// #    #
	// #    #
	// #####
	0xFF, 0x91, 0x91, 0x91, 0x91, 0x7E,

	// @170 'C' (6 pixels wide)
	//   ###
	//  #   #
	// #
	// #
	// #
	// #
	//  #   #
	//   ###
	0x3C, 0x42, 0x81, 0x81, 0x81, 0x42,

	// @176 'D' (6 pixels wide)
	// ####
	// #   #
	// #    #
	// #    #
	// #    #
	// #    #
	// #   #
	// ####
	0xFF, 0x81, 0x81, 0x81, 0x42, 0x3C,

	// @182 'E' (5 pixels wide)
	// #####
	// #
	// #
	// #
	// #####
	// #
	// #
	// #####
	0xFF, 0x91, 0x91, 0x91, 0x91,

	// @187 'F' (5 pixels wide)
	// #
	// #
	// #
	// #
	// ####
	// #
	// #
	// #####
	0xFF, 0x90, 0x90, 0x90, 0x80,

	// @192 'G' (7 pixels wide)
	//   ###
	//  #   #
	// #     #
	// #   ###
	// #
	// #     #
	//  #   #
	//   ###
	0x3C, 0x42, 0x81, 0x81, 0x89, 0x4A, 0x2C,

	// @199 'H' (6 pixels wide)
	// #    #
	// #    #
	// #    #
	// #    #
	// ######
	// #    #
	// #    #
	// #    #
	0xFF, 0x10, 0x10, 0x10, 0x10, 0xFF,

	// @205 'I' (1 pixels wide)
	// #
	// #
	// #
	// #
	// #
	// #
	// #
	// #
	0xFF,

	// @206 'J' (4 pixels wide)
	//  ##
	// #  #
	// #  #
	//    #
	//    #
	//    #
	//    #
	//    #
	0x06, 0x01, 0x01, 0xFE,

	// @210 'K' (6 pixels wide)
	// #    #
	// #   #
	// #   #
	// ## #
	// # ##
	// #  #
	// #   #
	// #    #
	0xFF, 0x08, 0x10, 0x38, 0x46, 0x81,

	// @216 'L' (5 pixels wide)
	// #####
	// #
	// #
	// #
	// #
	// #
	// #
	// #
	0xFF, 0x01, 0x01, 0x01, 0x01,

	// @221 'M' (7 pixels wide)
	// #  #  #
	// #  #  #
	// # # # #
	// # # # #
	// # # # #
	// ##   ##
	// ##   ##
	// #     #
	0xFF, 0x60, 0x1C, 0x03, 0x1C, 0x60, 0xFF,

	// @228 'N' (6 pixels wide)
	// #    #
	// #   ##
	// #  # #
	// #  # #
	// # #  #
	// # #  #
	// ##   #
	// #    #
	0xFF, 0x40, 0x30, 0x0C, 0x02, 0xFF,

	// @234 'O' (7 pixels wide)
	//   ###
	//  #   #
	// #     #
	// #     #
	// #     #
	// #     #
	//  #   #
	//   ###
	0x3C, 0x42, 0x81, 0x81, 0x81, 0x42, 0x3C,

	// @241 'P' (5 pixels wide)
	// #
	// #
	// #
	// ####
	// #   #
	// #   #
	// #   #
	// ####
	0xFF, 0x88, 0x88, 0x88, 0x70,

	// @246 'Q' (7 pixels wide)
	//   ### #
	//  #   #
	// #  ## #
	// #     #
	// #     #
	// #     #
	//  #   #
	//   ###
	0x3C, 0x42, 0x81, 0x85, 0x85, 0x42, 0x3D,

	// @253 'R' (6 pixels wide)
	// #    #
	// #   #
	// #   #
	// #  #
	// #####
	// #    #
	// #    #
	// #####
	0xFF, 0x90, 0x90, 0x98, 0x96, 0x61,

	// @259 'S' (6 pixels wide)
	//  ####
	// #    #
	//      #
	//    ##
	//  ##
	// #
	// #    #
	//  ####
	0x62, 0x91, 0x91, 0x89, 0x89, 0x46,

	// @265 'T' (5 pixels wide)
	//   #
	//   #
	//   #
	//   #
	//   #
	//   #
	//   #
	// #####
	0x80, 0x80, 0xFF, 0x80, 0x80,

	// @270 'U' (6 pixels wide)
	//  ####
	// #    #
	// #    #
	// #    #
	// #    #
	// #    #
	// #    #
	// #    #
	0xFE, 0x01, 0x01, 0x01, 0x01, 0xFE,

	// @276 'V' (7 pixels wide)
	//    #
	//    #
	//   # #
	//   # #
	//  #   #
	//  #   #
	// #     #
	// #     #
	0xC0, 0x30, 0x0C, 0x03, 0x0C, 0x30, 0xC0,

	// @283 'W' (11 pixels wide)
	//   #     #
	//   #     #
	//  # #   # #
	//  # #   # #
	//  #  # #  #
	//  #  # #  #
	// #   # #   #
	// #    #    #
	0xC0, 0x3C, 0x03, 0x0C, 0x70, 0x80, 0x70, 0x0C, 0x03, 0x3C, 0xC0,

	// @294 'X' (6 pixels wide)
	// #    #
	//  #  #
	//  #  #
	//   ##
	//   ##
	//  #  #
	//  #  #
	// #    #
	0x81, 0x66, 0x18, 0x18, 0x66, 0x81,

	// @300 'Y' (7 pixels wide)
	//    #
	//    #
	//    #
	//    #
	//   # #
	//  #   #
	//  #   #
	// #     #
	0x80, 0x60, 0x10, 0x0F, 0x10, 0x60, 0x80,

	// @307 'Z' (6 pixels wide)
	// ######
	//  #
	//   #
	//   #
	//    #
	//    #
	//     #
	//  #####
	0x01, 0x83, 0x8D, 0xB1, 0xC1, 0x81,

	// @320 ']' (2 pixels wide)
	// ##
	// #
	// #
	// #
	// #
	// #
	// #
	// ##
	0xFF, 0x81,
	0x00, 0x00,

	// @317 '\' (3 pixels wide)
	//   #
	//   #
	//  #
	//  #
	//  #
	//  #
	// #
	// #
	0xC0, 0x3C, 0x03,

	// @313 '[' (2 pixels wide)
	// ##
	//  #
	//  #
	//  #
	//  #
	//  #
	//  #
	//  #
	//  #
	// ##
	0x81, 0xFF,
	0x00, 0x00,

	// @324 '^' (5 pixels wide)
	// #   #
	//  # #
	//  # #
	//   #
	0x01, 0x06, 0x08, 0x06, 0x01,

	// @329 '_' (6 pixels wide)
	// ######
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01,

	// @335 '`' (2 pixels wide)
	//
	//
	//
	//
	//
	//
	//  #
	// #
	0x80, 0x40,

	// @337 'a' (5 pixels wide)
	//  ## #
	// #  ##
	// #   #
	//  ####
	// #   #
	//  ###
	0x16, 0x29, 0x29, 0x2A, 0x1F,

	// @342 'b' (5 pixels wide)
	// # ##
	// ##  #
	// #   #
	// #   #
	// ##  #
	// # ##
	// #
	// #
	0xFF, 0x12, 0x21, 0x21, 0x1E,

	// @347 'c' (5 pixels wide)
	//  ###
	// #   #
	// #
	// #
	// #   #
	//  ###
	0x1E, 0x21, 0x21, 0x21, 0x12,

	// @352 'd' (5 pixels wide)
	//  ## #
	// #  ##
	// #   #
	// #   #
	// #  ##
	//  ## #
	//     #
	//     #
	0x1E, 0x21, 0x21, 0x12, 0xFF,

	// @357 'e' (5 pixels wide)
	//  ###
	// #   #
	// #
	// #####
	// #   #
	//  ###
	0x1E, 0x29, 0x29, 0x29, 0x1A,

	// @362 'f' (3 pixels wide)
	//  #
	//  #
	//  #
	//  #
	//  #
	// ###
	//  #
	//   #
	0x20, 0x7F, 0xA0,

	// @365 'g' (5 pixels wide)
	// ####
	//     #
	//  ## #
	// #  ##
	// #   #
	// #   #
	// #  ##
	//  ## #
	0x79, 0x85, 0x85, 0x49, 0xFE,

	// @370 'h' (5 pixels wide)
	// #   #
	// #   #
	// #   #
	// #   #
	// ##  #
	// # ##
	// #
	// #
	0xFF, 0x10, 0x20, 0x20, 0x1F,

	// @375 'i' (1 pixels wide)
	// #
	// #
	// #
	// #
	// #
	// #
	//
	// #
	0xBF,

	// @376 'j' (2 pixels wide)
	// #
	//  #
	//  #
	//  #
	//  #
	//  #
	//  #
	//  #
	//
	//  #
	0x01, 0xFE,
	0x00, 0x02,

	// @380 'k' (4 pixels wide)
	// #  #
	// # #
	// # #
	// ##
	// # #
	// #  #
	// #
	// #
	0xFF, 0x08, 0x16, 0x21,

	// @384 'l' (1 pixels wide)
	// #
	// #
	// #
	// #
	// #
	// #
	// #
	// #
	0xFF,

	// @385 'm' (7 pixels wide)
	// #  #  #
	// #  #  #
	// #  #  #
	// #  #  #
	// ## #  #
	// # ####
	0x3F, 0x10, 0x20, 0x3F, 0x20, 0x20, 0x1F,

	// @392 'n' (5 pixels wide)
	// #   #
	// #   #
	// #   #
	// #   #
	// #   #
	// ####
	0x3F, 0x20, 0x20, 0x20, 0x1F,

	// @397 'o' (5 pixels wide)
	//  ###
	// #   #
	// #   #
	// #   #
	// #   #
	//  ###
	0x1E, 0x21, 0x21, 0x21, 0x1E,

	// @402 'p' (5 pixels wide)
	// #
	// #
	// # ##
	// ##  #
	// #   #
	// #   #
	// ##  #
	// # ##
	0xFF, 0x48, 0x84, 0x84, 0x78,

	// @407 'q' (5 pixels wide)
	//     #
	//     #
	//  ## #
	// #  ##
	// #   #
	// #   #
	// #  ##
	//  ## #
	0x78, 0x84, 0x84, 0x48, 0xFF,

	// @412 'r' (3 pixels wide)
	// #
	// #
	// #
	// #
	// ##
	// # #
	0x3F, 0x10, 0x20,

	// @415 's' (5 pixels wide)
	//  ###
	// #   #
	//    #
	//  ##
	// #   #
	//  ###
	0x12, 0x29, 0x29, 0x25, 0x12,

	// @420 't' (3 pixels wide)
	//  ##
	//  #
	//  #
	//  #
	//  #
	// ###
	//  #
	//  #
	0x20, 0xFF, 0x21,

	// @423 'u' (5 pixels wide)
	//  ## #
	// #  ##
	// #   #
	// #   #
	// #   #
	// #   #
	0x3E, 0x01, 0x01, 0x02, 0x3F,

	// @428 'v' (5 pixels wide)
	//   #
	//   #
	//  # #
	//  # #
	// #   #
	// #   #
	0x30, 0x0C, 0x03, 0x0C, 0x30,

	// @433 'w' (9 pixels wide)
	//   #   #
	//   #   #
	//  # # # #
	//  # # # #
	// #  # #  #
	// #   #   #
	0x30, 0x0C, 0x03, 0x1C, 0x20, 0x1C, 0x03, 0x0C, 0x30,

	// @442 'x' (5 pixels wide)
	// #   #
	//  # #
	//   #
	//   #
	//  # #
	// #   #
	0x21, 0x12, 0x0C, 0x12, 0x21,

	// @447 'y' (5 pixels wide)
	//  #
	//   #
	//   #
	//   #
	//  # #
	//  # #
	// #   #
	// #   #
	0xC0, 0x31, 0x0E, 0x30, 0xC0,

	// @452 'z' (5 pixels wide)
	// #####
	//  #
	//   #
	//   #
	//    #
	// #####
	0x21, 0x23, 0x2D, 0x31, 0x21,

	// @457 '{' (3 pixels wide)
	//   #
	//  #
	//  #
	// #
	//  #
	//  #
	//  #
	//   #
	0x10, 0x6E, 0x81,
	0x00, 0x00, 0x00,


	// @463 '|' (1 pixels wide)
	// #
	// #
	// #
	// #
	// #
	// #
	// #
	// #
	// #
	// #
	0xFF,
	0x00,

	// @465 '}' (3 pixels wide)
	//   #
	//  #
	//  #
	// #
	//  #
	//  #
	//  #
	//   #
	0x81, 0x6E, 0x10,
	0x00, 0x00, 0x00,

	// @471 '~' (5 pixels wide)
	// # ##
	// ### #
	0x03, 0x02, 0x03, 0x01, 0x02,
};

/** \brief  Character descriptors for Arial 8pt
*/
static const FONT_CHAR_INFO arial_8ptDescriptors[] =
{
	{2, 14, 0}, 		//
	{1, 8, 4}, 		// !
	{3, 3, 5}, 		// "
	{5, 8, 8}, 		// #
	{5, 9, 13}, 		// $
	{9, 8, 23}, 		// %
	{6, 8, 32}, 		// &
	{1, 8, 38}, 		// '
	{3, 10, 39}, 		// (
	{3, 10, 45}, 		// )
	{3, 4, 51}, 		// *
	{5, 5, 54}, 		// +
	{1, 3, 59}, 		// ,
	{3, 1, 60}, 		// -
	{1, 1, 63}, 		// .
	{3, 8, 64}, 		// /
	{5, 8, 67}, 		// 0
	{3, 8, 72}, 		// 1
	{5, 8, 75}, 		// 2
	{5, 8, 80}, 		// 3
	{5, 8, 85}, 		// 4
	{5, 8, 90}, 		// 5
	{5, 8, 95}, 		// 6
	{5, 8, 100}, 		// 7
	{5, 8, 105}, 		// 8
	{5, 8, 110}, 		// 9
	{1, 6, 115}, 		// :
	{1, 8, 116}, 		// ;
	{5, 5, 117}, 		// <
	{5, 3, 122}, 		// =
	{5, 5, 127}, 		// >
	{5, 8, 132}, 		// ?
	{10, 10, 137}, 		// @
	{7, 8, 157}, 		// A
	{6, 8, 164}, 		// B
	{6, 8, 170}, 		// C
	{6, 8, 176}, 		// D
	{5, 8, 182}, 		// E
	{5, 8, 187}, 		// F
	{7, 8, 192}, 		// G
	{6, 8, 199}, 		// H
	{1, 8, 205}, 		// I
	{4, 8, 206}, 		// J
	{6, 8, 210}, 		// K
	{5, 8, 216}, 		// L
	{7, 8, 221}, 		// M
	{6, 8, 228}, 		// N
	{7, 8, 234}, 		// O
	{5, 8, 241}, 		// P
	{7, 8, 246}, 		// Q
	{6, 8, 253}, 		// R
	{6, 8, 259}, 		// S
	{5, 8, 265}, 		// T
	{6, 8, 270}, 		// U
	{7, 8, 276}, 		// V
	{11, 8, 283}, 		// W
	{6, 8, 294}, 		// X
	{7, 8, 300}, 		// Y
	{6, 8, 307}, 		// Z
	{2, 10, 313}, 		// [
	{3, 8, 317}, 		// '\'
	{2, 10, 320}, 		// ]
	{5, 4, 324}, 		// ^
	{6, 1, 329}, 		// _
	{2, 8, 335}, 		// `
	{5, 6, 337}, 		// a
	{5, 8, 342}, 		// b
	{5, 6, 347}, 		// c
	{5, 8, 352}, 		// d
	{5, 6, 357}, 		// e
	{3, 8, 362}, 		// f
	{5, 8, 365}, 		// g
	{5, 8, 370}, 		// h
	{1, 8, 375}, 		// i
	{2, 10, 376}, 		// j
	{4, 8, 380}, 		// k
	{1, 8, 384}, 		// l
	{7, 6, 385}, 		// m
	{5, 6, 392}, 		// n
	{5, 6, 397}, 		// o
	{5, 8, 402}, 		// p
	{5, 8, 407}, 		// q
	{3, 6, 412}, 		// r
	{5, 6, 415}, 		// s
	{3, 8, 420}, 		// t
	{5, 6, 423}, 		// u
	{5, 6, 428}, 		// v
	{9, 6, 433}, 		// w
	{5, 6, 442}, 		// x
	{5, 8, 447}, 		// y
	{5, 6, 452}, 		// z
	{3, 10, 457}, 		// {
	{1, 10, 463}, 		// |
	{3, 10, 465}, 		// }
	{5, 2, 471}, 		// ~
};

/** \brief  This structure describes font information for Arial 8pt
*/
static const FONT_INFO arial_8ptFontInfo =
{
	2, //  Character height
	' ', //  Start character
	'~', //  End character
	arial_8ptDescriptors, //  Character descriptor array
	arial_8ptBitmaps, //  Character bitmap array
};

const Uint8 * FontBitmap;
const FONT_CHAR_INFO * FontDescriptors;
const FONT_INFO * FontInfo;

/*
 *  C55x_wait( delay )
 *
 *      C55x_wait in a software loop for 'delay' loops.
 *
 *      Uint32 delay  <- Number of delay loops
 */
void C55x_wait( Uint32 delay )
{
    volatile Uint32 i;

    for ( i = 0 ; i < delay ; i++ ){ };
}


/** ===========================================================================
 *   @n@b writeI2C
 *
 *   @b Description
 *   @n Function to perform I2C writes.
 *
 *   @b Arguments
 *   @verbatim
 *      address    - Address of the I2C slave module
 *      data       - pointer to the data to be written
 *      quantity   - Number of data bytes written
     @endverbatim
 *
 *   <b> Return Value </b>  CSL_Status
 *   @li                    CSL_SOK             - I2C writes are successful
 *   @li                    CSL_ESYS_FAIL       - I2C writes are not successful
 *   @li                    CSL_ESYS_INVPARAMS  - if the input parameters
 *                                                are invalid
 *
 *  ===========================================================================
 */
static Int8 oledwrite(Uint16* data, Uint16 quantity)
{
		Uint16 ret;
	    Uint16 startStop            = ((CSL_I2C_START) | (CSL_I2C_STOP));
		ret = I2C_write(data, quantity, I2C_ADDR,
                TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
		if (ret) {
			C55x_msgWrite("oledWrite: i2cWrite error : ret = %d\n\r", ret);
			return (OLED_ERR);
		}

		return (OLED_SUCCESS);
}

/**
 *  Int16 send(Uint16 comdat, Uint16 data)
 *
 *      Sends 2 bytes of data to the OSD9616
 *
 *      comdat :  0x00  => Command
 *                0x40  => Data
 *      data   :  Data to be sent
 *
 *  Returns
 *      0 => Success
 *     -1 => Fail
 */
static Int8 send(Uint8 comdat, Uint8 data)
{
    Uint16 cmd[2];
    Int8 ret;

    cmd[0] = comdat & 0x00FF;     // Specifies whether data is Command or Data
    cmd[1] = data;                // Command / Data
    C55x_wait( 1000 );

    ret = oledwrite(cmd, 2);
    C55x_wait( 1000 );
	if(ret == 0)
	{
        return (OLED_SUCCESS);
	}
	else
	{
		return (OLED_ERR);
	}

}

/*
 *  EZDSP5535_I2C_init( )
 *
 *      Enable and initalize the I2C module.
 *      The I2C clk is set to run at 100 KHz.
 */
Int16 oled_display_I2C_init( )
{
    CSL_Status        status;
    CSL_I2cConfig    i2cConfig;

    status = I2C_init(CSL_I2C0);

    /* Configure I2C module for write */
    i2cConfig.icoar  = CSL_I2C_ICOAR_DEFVAL;
    i2cConfig.icimr  = CSL_I2C_ICIMR_DEFVAL;
    i2cConfig.icclkl = 20;
    i2cConfig.icclkh = 20;
    i2cConfig.icsar  = CSL_I2C_ICSAR_DEFVAL;
    i2cConfig.icmdr  = CSL_I2C_ICMDR_WRITE_DEFVAL;
    i2cConfig.icemdr = CSL_I2C_ICEMDR_DEFVAL;
    i2cConfig.icpsc  = 20;

    status |= I2C_config(&i2cConfig);

    return 0;

}

/**
 *  Int16 multiSend(Uint16* data, Uint16 len)
 *
 *      Sends multiple bytes of data to the OSD9616
 *
 *      *data :  Pointer to start of I2C transfer
 *       len  :  Length of I2C transaction
 *
 *  Returns
 *      0 => Success
 *      1 => Fail
 */
static Int8 multiSend(Uint8 *data, Uint32 len)
{
    Uint8 x;
    Uint16 cmd[10];
    Int8 ret;

    for(x=0;x<len;x++)               // Command / Data
    {
    	cmd[x] = data[x];
    }

    C55x_wait( 1000 );

    ret = oledwrite(cmd, len);
    C55x_wait( 1000 );
    if(ret == 0)
    {
    	return (OLED_SUCCESS);
    }
    else
    {
    	return (OLED_ERR);
    }
}

/** ===========================================================================
 *   @n@b oledInit()
 *
 *   @b Description
 *   @n Function initializes I2C Module and also LCD.
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
I2C_RET oledInit(void)
{
	/*
	 *  Font data for Century Gothic 8pt
	 */

	/* Character bitmaps for Century Gothic 8pt */
	I2C_RET     returnValue;

	row = 0;
	totalcols = 0;
	returnValue = init();
	if (returnValue != I2C_RET_OK)
	{
		C55x_msgWrite("init: init Failed\n");
	    return (returnValue);
	}
	FontBitmap = &arial_8ptBitmaps[0];
    FontDescriptors = &arial_8ptDescriptors[0];
    FontInfo = &arial_8ptFontInfo;

    return I2C_RET_OK;

}

/** ===========================================================================
 *   @n@b init()
 *
 *   @b Description
 *   @n Function initializes I2C Module and also LCD.
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
I2C_RET init()
{
    Uint8 cmd[10];

    /* Note: Wire initialization will be done in main.cpp */
    /* Initialize OSD9616 display */
    send(0x00,0x00); // Set low column address
    send(0x00,0x10); // Set high column address

    send(0x00,0xae); // Turn off oled panel

    send(0x00,0xd5); // Set display clock divide ratio/oscillator frequency
    send(0x00,0x80); // Set divide ratio

    cmd[0] = 0x00;   // Set multiplex ratio(1 to 16)
    cmd[1] = 0xa8;
    cmd[2] = 0x0f;
    multiSend(cmd, 3);

    send(0x00,0xd3); // Set display offset
    send(0x00,0x00); // Not offset

    send(0x00,0x40); // Set Display start line address

    cmd[0] = 0x00;  //--set DC-DC enable
    cmd[1] = 0x8d; // Set Charge Pump
    cmd[2] = 0x14;
    multiSend(cmd, 3);

    send(0x00,0xa1); // Set segment re-map 95 to 0
    send(0x00,0xc8); // Set COM Output Scan Direction

    cmd[0] = 0x00;  // Set com pins hardware configuration
    cmd[1] = 0xda;
    cmd[2] = 0x02;
    multiSend(cmd, 3);

    cmd[0] = 0x00;  // Set contrast control register
    cmd[1] = 0x81;
    cmd[2] = 0xaf;
    multiSend(cmd, 3);

    cmd[0] = 0x00;  // Set pre-charge period
    cmd[1] = 0xd9;
    cmd[2] = 0xf1;
    multiSend(cmd, 3);

    send(0x00,0xdb); // Set vcomh
    send(0x00,0x20); // 0.83*vref

    send(0x00,0xa4); // Set entire display on/off
    send(0x00,0xa6); // Set normal display

    send(0x00,0xaf); // Turn on oled panel

	dir = 0;

	return I2C_RET_OK;
}

/** ===========================================================================
 *   @n@b setOrientation()
 *
 *   @b Description
 *   @n Function to set the orientation of LCD.
 *
 *   @b Arguments
 *   @verbatim
 *     int newDir      <- Direction of orientation
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
I2C_RET setOrientation(Int8 newDir)
{
	if(newDir == 0)
	{	/* Set divide ratio */
		send(0x00,0xC8);
		dir = 1;
	}
	else if(newDir == 1)
	{	/* Set divide ratio */
		send(0x00,0xC0);
		dir = 0;
	}

	return I2C_RET_OK;
}

/** ===========================================================================
 *   @n@b flip()
 *
 *   @b Description
 *   @n Flips the screen vertically
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void flip()
{
	if(dir == 0)
	{   /* Set divide ratios */
		send(0x00,0xC0);
		send(0x00,0xA1);
		dir = 1;
	}
	else if(dir == 1)
	{   /* Set divide ratios */
		send(0x00,0xC8);
		send(0x00,0xA1);
		dir = 0;
	}
}

/** ===========================================================================
 *   @n@b setline(line)
 *
 *   @b Description
 *   @n Sets the start line for the display
 *
 *   @b Arguments
 *   @verbatim
 *     int line      <- Line number
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
I2C_RET setline(Int8 line)
{
    if(line == 0)
	{
		/* Write to page 1 */
		send(0x00,0x00);   // Set low column address
		send(0x00,0x10);   // Set high column address
		send(0x00,0xb0+1); // Set page for page 1
    }
	else if(line == 1)
	{
	    /* Write to page 0 */
		send(0x00,0x00);   // Set low column address
		send(0x00,0x10);   // Set high column address
		send(0x00,0xb0+0); // Set page for page 0

	}
	totalcols = 0;
	row = line;

	return I2C_RET_OK;
}

/** ===========================================================================
 *   @n@b setRolling(int row, int status)
 *
 *   @b Description
 *   @n Sets the rolling parameters
 *
 *   @b Arguments
 *   @verbatim
 *     int row      <- Row number
 *     int status   <- Status of rolling
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void setRolling(Int8 row, Int8 status)
{
	if(row == 0)
	{
		if(status == 0)
		{
			rolling0 = 0;
		}
		else
		{
			rolling0 = 1;
		}
	}
	else if(row == 1)
	{
		if(status == 0)
		{
			rolling1 = 0;
		}
		else
		{
			rolling1 = 1;
		}
	}
}

/** ===========================================================================
 *   @n@b print(string[])
 *
 *   @b Description
 *   @n Prints a string to the LCD
 *
 *   @b Arguments
 *   @verbatim
 *      char *printString      <- Pointer to the string to be printed
     @endverbatim
 *
 *   <b> Return Value </b>  Returns the number of bytes written to LCD
 *   @li                    strLen   - Length of the string written to LCD
 *
 *  ===========================================================================
 */
I2C_RET printstr(Int8 string[])
{
    Int8 i = 0;

	while(string[i] != '\0')
    {
      if(printchar(string[i]) == 0) {break;}
	  i++;
    }

    return I2C_RET_OK;
}

/** ===========================================================================
 *   @n@b printchar(character)
 *
 *   @b Description
 *   @n Prints a character to the LCD
 *
 *   @b Arguments
 *   @verbatim
 *     unsigned char character      <- Character to be printed
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
Int8 printchar(Uint8 a)
{
	Int16 i, n;

	n = (Int8) a - 0x20;

	for(i=0;i<(FontDescriptors[n].widthBits);i++)
    {
		if(totalcols > 127)
		{
			return 0;
		}
		else
		{
			totalcols++;
		}
		send(0x40,FontBitmap[FontDescriptors[n].offset + i]);
    }
	if(totalcols > 127)
	{
		return 0;
	}
	else
	{
		totalcols++;
	}
	send(0x40,0x00);
	if(totalcols > 127)
	{
		return 0;
	}
	else
	{
		totalcols++;
	}
	send(0x40,0x00);  // 2 Spaces
    send(0x00,0x2e);  // Deactivate Scrolling
	return 1;

}


/** ===========================================================================
 *   @n@b clear()
 *
 *   @b Description
 *   @n Clears the LCD screen and positions the cursor in the upper-left corner
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
I2C_RET clear(void)
{
	Int16 i;
	totalcols = 0;
	//This part is to initialize and blank the LCD.
	/* Initialize OLED display */
	send(0x00,0x2e);  // Deactivate Scrolling

    /* Write to page 0 */
    send(0x00,0x00);   // Set low column address
    send(0x00,0x10);   // Set high column address
    send(0x00,0xb0+0); // Set page for page 0 to page 5

    for(i=0;i<128;i++)
    {
    	send(0x40,0x00);
    }

    /* Write to page 1*/
    send(0x00,0x00);   // Set low column address
    send(0x00,0x10);   // Set high column address
    send(0x00,0xb0+1); // Set page for page 0 to page 5

    for(i=0;i<128;i++)
    {
    	send(0x40,0x00);
    }

	/* Set cursor to upper left corner */
    send(0x00,0x00);   // Set low column address
    send(0x00,0x10);   // Set high column address
    send(0x00,0xb0+1); // Set page for page 0 to page 5
	row = 0;

	return I2C_RET_OK;
}

/** ===========================================================================
 *   @n@b scrollDisplayLeft()
 *
 *   @b Description
 *   @n Scrolls the contents of the display (text and cursor) to the left
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
I2C_RET scrollDisplayLeft(void)
{

	Uint8 cmd[10]; // For multibyte commands

    // These commands scroll the display without changing the RAM
    /* Set vertical and horizontal scrolling */
    cmd[0] = 0x00;
    cmd[1] = 0x27;  // Vertical and Right Horizontal Scroll 26 = left, 27 = right, 29 = vl, 2A = vr
    cmd[2] = 0x00;  // Dummy byte
    cmd[3] = 0x00;  // Define start page address
    cmd[4] = 0x00;  // Set time interval between each scroll step as 5 frames
    cmd[5] = 0x01;  // Define end page address
    cmd[6] = 0x00;  // Dummy byte
    cmd[7] = 0xFF;  // Dummy byte
    multiSend(cmd, 8);
    send(0x00,0x2f);

    return I2C_RET_OK;
}

/** ===========================================================================
 *   @n@b scrollDisplayLeft()
 *
 *   @b Description
 *   @n Scrolls the contents of a particular line, of the display (text and
 *      cursor) to the left
 *
 *   @b Arguments
 *   @verbatim
 *     int line      <- Line number
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void scrollDisplayLeftLine(Int8 line)
{

	Uint8 cmd[10]; // For multibyte commands

    if ((0 == line) || (1 == line))
    {
        // These commands scroll the display without changing the RAM
        /* Set vertical and horizontal scrolling */
        cmd[0] = 0x00;
        cmd[1] = 0x27;  // Vertical and Right Horizontal Scroll 26 = left, 27 = right, 29 = vl, 2A = vr
        cmd[2] = 0x00;  // Dummy byte
        cmd[3] = !line; // Define start page address
        cmd[4] = 0x00;  // Set time interval between each scroll step as 5 frames
        cmd[5] = !line; // Define end page address
        cmd[6] = 0x00;  // Dummy byte
        cmd[7] = 0xFF;  // Dummy byte
        multiSend(cmd, 8);
        send(0x00,0x2f);
	}
}

/** ===========================================================================
 *   @n@b autoscroll()
 *
 *   @b Description
 *   @n Turns on automatic scrolling of the LCD
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void autoscroll(void)
{
	Uint8 cmd[10];    // For multibyte commands

    /* Set vertical and horizontal scrolling */
    cmd[0] = 0x00;
    cmd[1] = 0x2A;  // Vertical and Right Horizontal Scroll 26 = left, 27 = right, 29 = vl, 2A = vr
    cmd[2] = 0x00;  // Dummy byte
    cmd[3] = 0x00;  // Define start page address
    cmd[4] = 0x00;  // Set time interval between each scroll step as 5 frames
    cmd[5] = 0x01;  // Define end page address
    cmd[6] = 0x01;  // Vertical scrolling offset
    multiSend(cmd, 7);
    send(0x00,0x2f);

    /* Keep first 8 rows from vertical scrolling  */
    cmd[0] = 0x00;
    cmd[1] = 0xa3;  // Set Vertical Scroll Area
    cmd[2] = 0x08;  // Set No. of rows in top fixed area
    cmd[3] = 0x08;  // Set No. of rows in scroll area
    multiSend(cmd, 4);
}

/** ===========================================================================
 *   @n@b autoscroll()
 *
 *   @b Description
 *   @n Turns off automatic scrolling of the LCD
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void noAutoscroll(void)
{
	send(0x40,0x00);
	send(0x40,0x00);  // 2 Spaces
    send(0x00,0x2e);  // Deactivate Scrolling
}

/** ===========================================================================
 *   @n@b scrollDisplayRight()
 *
 *   @b Description
 *   @n Scrolls the contents of the display (text and cursor) to the right
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
I2C_RET scrollDisplayRight(void)
{
	Uint8 cmd[10];    // For multibyte commands

    /* Set horizontal scrolling */
    cmd[0] = 0x00;
    cmd[1] = 0x26;  // Vertical and Left Horizontal Scroll 26 = left, 27 = right, 29 = vl, 2A = vr
    cmd[2] = 0x00;  // Dummy byte
    cmd[3] = 0x00;  // Define start page address
    cmd[4] = 0x00;  // Set time interval between each scroll step as 5 frames
    cmd[5] = 0x01;  // Define end page address
    cmd[6] = 0x00;  // Dummy byte
    cmd[7] = 0xFF;  // Dummy byte
    multiSend(cmd, 8);
    send(0x00,0x2f);

    return I2C_RET_OK;
}

/** ===========================================================================
 *   @n@b scrollDisplayRight(line)
 *
 *   @b Description
 *   @n Scrolls the contents of a particular line, of the display (text and
 *      cursor) to the right
 *
 *   @b Arguments
 *   @verbatim
 *     int line      <- Line number
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void scrollDisplayRightLine(Int8 line)
{
	Uint8 cmd[10];    // For multibyte commands

    if ((0 == line) || (1 == line))
    {
        /* Set horizontal scrolling */
        cmd[0] = 0x00;
        cmd[1] = 0x26;  // Vertical and Left Horizontal Scroll 26 = left, 27 = right, 29 = vl, 2A = vr
        cmd[2] = 0x00;  // Dummy byte
        cmd[3] = !line; // Define start page address
        cmd[4] = 0x00;  // Set time interval between each scroll step as 5 frames
        cmd[5] = !line; // Define end page address
        cmd[6] = 0x00;  // Dummy byte
        cmd[7] = 0xFF;  // Dummy byte
        multiSend(cmd, 8);
        send(0x00,0x2f);
	}
}

/** ===========================================================================
 *   @n@b noDisplay()
 *
 *   @b Description
 *   @n Turns off the LCD display
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void noDisplay()
{
    send(0x00,0xae); // Turn off oled panel
}

/** ===========================================================================
 *   @n@b display()
 *
 *   @b Description
 *   @n Turns on the LCD display
 *
 *   @b Arguments
 *   @verbatim
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void display()
{
    send(0x00,0xaf); // Turn on oled panel
}

/** ===========================================================================
 *   @n@b resetCursor(line)
 *
 *   @b Description
 *   @n Resets the cursor position to the beginning in a given line
 *
 *   @b Arguments
 *   @verbatim
 *      int line    <- line number
     @endverbatim
 *
 *   <b> Return Value </b>
 *    @n None
 *
 *  ===========================================================================
 */
void resetCursor(Int8 line)
{
	totalcols = 0;
	if(line == 0)
	{
		/* Write to line 0 */
		send(0x00,0x00);   // Set low column address
		send(0x00,0x10);   // Set high column address
		send(0x00,0xb0+1); // Set page for page 0 to page 5
	}
	else if(line == 1)
	{
	   /* Write to line 1*/
		send(0x00,0x00);   // Set low column address
		send(0x00,0x10);   // Set high column address
		send(0x00,0xb0+0); // Set page for page 0 to page 5
	}
}

/**
 *  \brief    This function is used to check if oled dispaly unit presents
 *            or not on the boosterpack
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS oled_detect_test(void *testArgs)
{
	Int16 returnvalue;
	Uint16 datapointer[1] = {0x81};
	Uint16 datalength = 2;

	returnvalue = i2cProbe(0x3C,datapointer,datalength);
	if(returnvalue != CSL_SOK)
	{
		C55x_msgWrite("I2C detect failed\n\r");
		return returnvalue;
	}

    return (TEST_PASS);

}

/**
 *  \brief    This function is used to display messages on the first
 *            rows of the OLED dispaly unit
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS msg_display_test_on_row1(void *testArgs)
{
	Int16 retVal;

	retVal = oledInit();
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("oledInit: oled module init Failed\n\r");
		return (TEST_FAIL);
	}

	retVal = clear();
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("clear: oled screen clear Failed\n\r");
		return (TEST_FAIL);
	}

	C55x_msgWrite("\n\rDisplaying 'Cos tam' on first row...\n\r");

	retVal = setline(0);
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("setline: line setting to first Failed\n\r");
		return (TEST_FAIL);
	}

	retVal = setOrientation(1);
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("setOrientation: Horizontal orientation Failed\n\r");
		return (TEST_FAIL);
	}

	retVal = printstr("Cos tam");
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("printstr: Oled print Failed\n\r");
		return (TEST_FAIL);
	}

	C55x_delay_msec(5000);

    return (TEST_PASS);

}

/**
 *  \brief    This function is used to display messages on the second
 *            rows of the OLED dispaly unit
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS msg_display_test_on_row2(void *testArgs)
{
	Int16 retVal;

	retVal = oledInit();
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("oledInit: oled module init Failed\n\r");
		return (TEST_FAIL);
	}

    retVal = clear();
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("clear: oled screen clear Failed\n\r");
		return (TEST_FAIL);
	}

	C55x_msgWrite("\n\rDisplaying 'Cos tam' on second row...\n\r");

	retVal = setline(1);
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("setline: line setting to second Failed\n\r");
		return (TEST_FAIL);
	}

	retVal = setOrientation(1);
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("setOrientation: Horizontal orientation Failed\n\r");
		return (TEST_FAIL);
	}

	retVal = printstr("Cos tam");
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("printstr: Oled print Failed\n\r");
		return (TEST_FAIL);
	}

    C55x_delay_msec(5000);

    return (TEST_PASS);

}

/**
 *  \brief    This function is used to display messages on the both
 *            the rows of OLED display unit
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS msg_display_test_on_both_the_rows(void *testArgs)
{
	Int16 retVal;

	retVal = oledInit();
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("oledInit: oled module init Failed\n\r");
		return (TEST_FAIL);
	}

	retVal = clear();
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("clear: oled screen clear Failed\n\r");
		return (TEST_FAIL);
	}

	C55x_msgWrite("\n\rDisplaying 'Karol i Bartek' on first row and \n\r"
			      "'BFF' on the second row\n\n\r");

	//print two lines
	retVal = setline(0);
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("setline: line setting to first Failed\n\r");
		return (TEST_FAIL);
	}

	retVal = setOrientation(1);
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("setOrientation: Horizontal orientation Failed\n\r");
		return (TEST_FAIL);
	}

	retVal = printstr("Karol i Bartek");
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("printstr: Oled print Failed\n\r");
		return (TEST_FAIL);
	}

	retVal = setline(1);
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("setline: line setting to second Failed\n\r");
		return (TEST_FAIL);
	}

	retVal = setOrientation(1);
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("setOrientation: Horizontal orientation Failed\n\r");
		return (TEST_FAIL);
	}

	retVal = printstr("BFF");
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("printstr: Oled print Failed\n\r");
		return (TEST_FAIL);
	}

	C55x_delay_msec(5000);

	C55x_msgWrite("Scrolling OLED Display from Right to Left\n\r");

    retVal = scrollDisplayLeft();
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("scrollDisplayLeft: Left scroll Failed\n\r");
		return (TEST_FAIL);
	}

	retVal = scrollDisplayLeft();
	if (retVal != I2C_RET_OK)
	{
		C55x_msgWrite("scrollDisplayRight: Right scroll Failed\n\r");
		return (TEST_FAIL);
	}

    return (TEST_PASS);

}

/**
 *  \brief    This function is used to performs initilisation of OLED
 *            module and sends some messages on the OLED display unit
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS run_oled_display_test(void *testArgs)
{
	Uint16 retVal;
	Uint8  c = 0;

	/* Initialize I2C */
	oled_display_I2C_init();

	C55x_msgWrite("Running OLED Device Detect Test...\n\r");
	retVal = oled_detect_test(testArgs);
	if(retVal != 0)
	{
		C55x_msgWrite("OLED detection failed\n\r");
		return (TEST_FAIL);
	}
	else
	{
		C55x_msgWrite("OLED detection Successful\n\n\r");
	}

	C55x_msgWrite("Running OLED Display Test...\n\r");
	retVal = msg_display_test_on_row1(testArgs);
	if(retVal != 0)
	{
		C55x_msgWrite("Message display test on the first row of the\n\r"
				      "BoosterPack failed\n\r");
		return (TEST_FAIL);
	}

	retVal = msg_display_test_on_row2(testArgs);
	if(retVal != 0)
	{
		C55x_msgWrite("Message display test on the second row of the\n\r"
				      "BoosterPack failed\n\r");
		return (TEST_FAIL);
	}

	retVal = msg_display_test_on_both_the_rows(testArgs);
	if(retVal != 0)
	{
		C55x_msgWrite("Message display test on the both the rows of the\n\r"
				      "BoosterPack failed\n\r");
		return (TEST_FAIL);
	}

	C55x_msgWrite("OLED Display Test Completed\n\r");

#ifdef USE_USER_INPUT

	C55x_msgWrite("\n\rPress Y/y if the test messages are displayed properly on OLED,\n\r"
			      "any other key for failure:\n\r");

	C55x_msgRead(&c, 1);
	if((c != 'y') && (c != 'Y'))
	{
		C55x_msgWrite("Test messages are not displayed on the OLED\n\r");
	    return (TEST_FAIL);
	}
#endif

    return (TEST_PASS);

}

/**
 * \brief This function performs oled display test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS oledDisplayTest(void *testArgs)
{
    Int16 retVal;

	C55x_msgWrite("\n*************************************\n\r");
	C55x_msgWrite(  "              OLED TEST              \n\r");
	C55x_msgWrite(  "*************************************\n\r");

	retVal = run_oled_display_test(testArgs);
	if(retVal != TEST_PASS)
    {
	    C55x_msgWrite("\n\nOLED Test Failed!\n\r");
	    return (TEST_FAIL);
	}
	else
	{
		C55x_msgWrite("\n\nOLED Test Passed!\n\r");
	}

	C55x_msgWrite("\nOLED Test Completed!!\n\r");

    return (TEST_PASS);

}
