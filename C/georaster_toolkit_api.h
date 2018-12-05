#ifndef GRT_API_H
#define GRT_API_H
/*
Simple wraper around gdal raster for binding usage
*/
#include "stdbool.h"

#ifndef GDALDriverH
typedef void * GDALDriverH;
typedef void * GDALDatasetH;
typedef void * GDALRasterBandH;
#endif

enum grt_errors_t {GRT_OPEN_ERROR,GRT_BAND_COUNT_ERROR,GRT_POINT_OUT_OF_RANGE};

typedef struct {
  double a;
  double b;
  double c;
  double d;
  double e;
  double f;
} grt_geo_transform_t;

typedef struct{
  int block_x, block_y;
  int last_x, last_y;
} grt_chunker_t;

typedef struct {
  char * filename;

  GDALDriverH h_driver; 
  GDALDatasetH h_dataset; 
  GDALRasterBandH h_band; 

  
  //int x, y;
  int x_size, y_size;
  int selected_band; // count from 1 to  GDALGetRasterCount(raster->h_dataset). default 1
  bool is_file_temporary; // 1 - this is temporary file and cannot be stored! 0 - regular file
  grt_geo_transform_t fwd_geo;
  grt_geo_transform_t inv_geo;

  grt_chunker_t block_reader; // read block by block
  grt_chunker_t line_reader; // read line by line
  grt_chunker_t buffer_reader;  // read buffer by buffer
  grt_chunker_t block_writer; // read block by block

} grt_raster_t;


void (* grt_error_callback)(const char *, int);
void grt_error_printf(const char * msg, int errn);

void grt_init();

grt_raster_t * grt_open(const char * filename, int band_no);

int grt_close(grt_raster_t * raster);
int grt_save(grt_raster_t * raster);

int grt_band_count(grt_raster_t * raster);
int grt_select_band(grt_raster_t * raster, int band_no);

void grt_lp_to_coord(grt_raster_t * raster, double in_x, double in_y, double * out_x, double * out_y);
void grt_coord_to_lp(grt_raster_t * raster, double in_x, double in_y, double * out_x, double * out_y);

grt_geo_transform_t grt_get_geo_tranform(grt_raster_t * raster);
grt_geo_transform_t grt_get_inv_geo_tranform(grt_raster_t * raster);

int grt_read_point(grt_raster_t * raster, float * point, int x, int y);
float grt_read_point_float32(grt_raster_t * raster, int x, int y);

#endif