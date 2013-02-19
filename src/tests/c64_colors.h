#ifndef __C64_COLORS__
#define __C64_COLORS__

/* ncurses lib takes RGB values from 0 to 1000, 8bit color values 
 * are written to the comments. 
 * 8bit rgb color values will result in a color depth of 24bit. For example 
 * SDL uses a 32bit color datatype: RGBA each of 8bit. A is Alpha and should 
 * be set to 0xff normally. 
 * simple "shading" could be achieved by decreasing this value.
 */

#define C64_COLORS_0_BLACK_R	0
#define C64_COLORS_0_BLACK_G	0
#define C64_COLORS_0_BLACK_B	0

#define C64_COLORS_1_WHITE_R	1000
#define C64_COLORS_1_WHITE_G	1000
#define C64_COLORS_1_WHITE_B	1000

#define C64_COLORS_2_RED_R    1000  // 135
#define C64_COLORS_2_RED_G    0
#define C64_COLORS_2_RED_B    0

#define C64_COLORS_3_CYAN_R    0 // 40
#define C64_COLORS_3_CYAN_G    0 // 240
#define C64_COLORS_3_CYAN_B    0 // 255

#define C64_COLORS_4_PURPLE_R    0 // 200
#define C64_COLORS_4_PURPLE_G    0 // 70
#define C64_COLORS_4_PURPLE_B    0 // 200

#define C64_COLORS_5_GREEN_R    0 
#define C64_COLORS_5_GREEN_G    1000 
#define C64_COLORS_5_GREEN_B    0

#define C64_COLORS_6_BLUE_R    0 
#define C64_COLORS_6_BLUE_G    0
#define C64_COLORS_6_BLUE_B    1000

#define C64_COLORS_7_YELLOW_R    0 // 230
#define C64_COLORS_7_YELLOW_G    0 // 220
#define C64_COLORS_7_YELLOW_B    0

#define C64_COLORS_8_ORANGE_R    0 // 230
#define C64_COLORS_8_ORANGE_G    0 // 155
#define C64_COLORS_8_ORANGE_B    0 // 85

#define C64_COLORS_9_BROWN_R    0 // 100
#define C64_COLORS_9_BROWN_G    0 // 70
#define C64_COLORS_9_BROWN_B    0 // 0

#define C64_COLORS_10_LRED_R    0 // 255
#define C64_COLORS_10_LRED_G    0 // 120
#define C64_COLORS_10_LRED_B    0 // 120

#define C64_COLORS_11_DGRAY_R    0 // 50
#define C64_COLORS_11_DGRAY_G    0 // 50
#define C64_COLORS_11_DGRAY_B    0 // 50

#define C64_COLORS_12_GRAY_R    0 // 100
#define C64_COLORS_12_GRAY_G    0 // 100
#define C64_COLORS_12_GRAY_B    0 // 100

#define C64_COLORS_13_LGREEN_R    0 // 170
#define C64_COLORS_13_LGREEN_G    0 // 255
#define C64_COLORS_13_LGREEN_B    0 // 100

#define C64_COLORS_14_LBLUE_R    0 // 10
#define C64_COLORS_14_LBLUE_G    0 // 160
#define C64_COLORS_14_LBLUE_B    0 // 240

#define C64_COLORS_15_LGREY_R    0 // 180
#define C64_COLORS_15_LGREY_G    0 // 180
#define C64_COLORS_15_LGREY_B    0 // 180

#endif

