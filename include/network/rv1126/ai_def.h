#ifndef __AI_DEF_H
#define __AI_DEF_H

//Add by 2022-07-20; AI Part Define;
#define ROI_TYPE_LINE      0x01
#define ROI_TYPE_RECT      0x02
#define ROI_TYPE_POLYGON   0x03
#define ROI_RECT_NUM       4
#define ROI_LINE_NUM       4
#define ROI_POLYGON_NUM    4
#define ROI_MAX_VERT       10  //max border is 10

typedef enum {
	DIR_INNER = 0, // inner region is valid
	DIR_OUT,   // outer region is valid
	DIR_UP,    //
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_LEFT_UP,
	DIR_LEFT_DOWN,
	DIR_RIGHT_UP,
	DIR_RIGHT_DOWN
}DIR_E;

typedef struct __roi_rect_t__
{
	int xpoint;
	int ypoint;
	int nWidth;
	int nHeigh;
	int nDirection; //DIR_INNER£¬DIR_OUT
} roi_rect_t;

typedef struct __roi_point_t__
{
	int xPoint;
	int yPoint;
} roi_point_t;

typedef struct __roi_line_t__
{
	int nStartXpoint;
	int nStartYPoint;
    int nEndXPoint;
	int nEndYPoint;
	int nDirection; //DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT,DIR_LEFT_UP,DIR_LEFT_DOWN,DIR_RIGHT_UP,DIR_RIGHT_DOWN
} roi_line_t;

typedef struct __roi_polygon_t__
{
	int point_num;
	int nDirection; //DIR_INNER£¬DIR_OUT
	roi_point_t point[ROI_MAX_VERT];
} roi_polygon_t;

typedef struct __video_roi_area_t__
{
	int rect_num;     //number of rects
	int line_num;     //number of lines
	int polygon_num;  //number of polygons
	roi_rect_t rect[ROI_RECT_NUM];
	roi_line_t line[ROI_LINE_NUM];
	roi_polygon_t polygon[ROI_POLYGON_NUM];
} video_roi_area_t;

typedef struct __video_roi_config_t__
{
	int chann;
	video_roi_area_t roi_area;
} video_roi_config_t;

//End AI Part; 2022-07-20;


#endif  //__AI_DEF_H


