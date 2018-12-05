#include "gdal.h"
#include "cpl_conv.h" /* for CPLMalloc() */
#include "ogr_api.h"
#include "stdbool.h"
#include "stdio.h"

#include "georaster_toolkit_api.h"
// compile:
//gcc georaster_toolkit.c `gdal-config --libs` `gdal-config --cflags`

void (* grt_error_callback)(const char *, int);

void grt_error_printf(const char * msg, int errn){
  printf("GRT_ERR(%d): %s",errn, msg);
}

void grt_init(){
  GDALAllRegister();
  OGRRegisterAll();
  CPLPushErrorHandler( CPLQuietErrorHandler );
  grt_error_callback = grt_error_printf;
}

grt_raster_t * grt_open(const char * filename, int band_no){
  grt_raster_t * raster = (grt_raster_t *) malloc( sizeof(grt_raster_t) );

  //Opening the File
  raster->h_dataset = NULL;
  raster->h_dataset = GDALOpen(filename, GA_ReadOnly );
  raster->filename = (char *) malloc( strlen(filename) + 2);
  strcpy(raster->filename, filename); //

  if( raster->h_dataset == NULL ) {
    // sorry, uknown file type
    //printf("GRT Unknown file");
    free(raster);
    raster = NULL;

    grt_error_callback("Gdal - cannot open file",GRT_OPEN_ERROR);

  }else{

    // Fetching a Driver
    raster->h_driver = GDALGetDatasetDriver( raster->h_dataset );

    grt_select_band(raster, band_no);

  }

  return raster;
}

int grt_close(grt_raster_t * raster){
  if(raster != NULL){
    if( raster->h_dataset != NULL )
      GDALClose( raster->h_dataset );

    free(raster->filename);
    free(raster);
  }
  return 0;
}

int grt_save(grt_raster_t * raster){
  return grt_close(raster);
}
// band_no starts from 1, gdal counts from 1
int grt_select_band(grt_raster_t * raster, int band_no){
  if (band_no >= GDALGetRasterCount(raster->h_dataset) )
    grt_error_callback("Requested band number exceed band count in dataset", GRT_BAND_COUNT_ERROR);

  if (band_no < 1 )
    grt_error_callback("Requested band number cannot be < 0", GRT_BAND_COUNT_ERROR);

  raster->h_band = raster->h_band = GDALGetRasterBand( raster->h_dataset, band_no ); 
  raster->selected_band = band_no;

  raster->x_size = GDALGetRasterBandXSize( raster->h_band );
  raster->y_size = GDALGetRasterBandYSize( raster->h_band );

  raster->fwd_geo = grt_get_geo_tranform(raster);
  raster->inv_geo = grt_get_inv_geo_tranform(raster);

  raster->line_reader.block_x = raster->x_size;
  raster->line_reader.block_y = 1;
  raster->line_reader.last_x = 0;
  raster->line_reader.last_y = -1;

  raster->block_reader.block_x = 30;
  raster->block_reader.block_y = 30;
  raster->block_reader.last_x = 0;
  raster->block_reader.last_y = -1;

  return band_no;
}

int grt_band_count(grt_raster_t * raster){
  return( GDALGetRasterCount(raster->h_dataset) );
}

int grt_read_point(grt_raster_t * raster, float * point, int x, int y){
  int nXSize = GDALGetRasterBandXSize( raster->h_band );
  int nYSize = GDALGetRasterBandYSize( raster->h_band );

  if (x > nXSize || x < 0)
    grt_error_callback("Point out of range", GRT_POINT_OUT_OF_RANGE);

  if (y > nYSize || y < 0)
    grt_error_callback("Point out of range", GRT_POINT_OUT_OF_RANGE);

  return GDALRasterIO( raster->h_band, GF_Read, x, y, 1, 1, point, 1, 1, GDT_Float32,  0, 0 );
}

float grt_read_point_float32(grt_raster_t * raster, int x, int y){
  int nXSize = GDALGetRasterBandXSize( raster->h_band );
  int nYSize = GDALGetRasterBandYSize( raster->h_band );

  float f;

  if (x > nXSize || x < 0)
    grt_error_callback("Point out of range", GRT_POINT_OUT_OF_RANGE);

  if (y > nYSize || y < 0)
    grt_error_callback("Point out of range", GRT_POINT_OUT_OF_RANGE);

  int ret = GDALRasterIO( raster->h_band, GF_Read, x, y, 1, 1, &f, 1, 1, GDT_Float32,  0, 0 );

  return f;
}


grt_geo_transform_t grt_get_geo_tranform(grt_raster_t * raster){

  grt_geo_transform_t geo_transform;

  if( GDALGetGeoTransform(raster->h_dataset, (double *) &geo_transform) == CE_None ){
    return geo_transform;
  } else {
    // something went wrong or there is no geo transform information
    return geo_transform;
  }
}

grt_geo_transform_t grt_get_inv_geo_tranform(grt_raster_t * raster){

  grt_geo_transform_t geo_transform ;
  grt_geo_transform_t inv_geo_transform ;

  if( GDALGetGeoTransform(raster->h_dataset, (double *) &geo_transform) == CE_None && 
     GDALInvGeoTransform((double *) &geo_transform, (double *) &inv_geo_transform) == CE_None){

    return inv_geo_transform;
  } else {
    // something went wrong or there is no geo transform information
    return inv_geo_transform;
  }
}

void grt_set_geo_tranform(grt_raster_t * raster, grt_geo_transform_t * geo_transform ){
  GDALSetGeoTransform( raster->h_dataset, (double *) geo_transform );
  raster->fwd_geo = *geo_transform;
  raster->inv_geo = grt_get_inv_geo_tranform(raster);
}

void grt_lp_to_coord(grt_raster_t * raster, double in_x, double in_y, double * out_x, double * out_y){
  //GDALApplyGeoTransform((raster->inv_geo.d.arr), in_x, in_y, out_x, out_y);
  GDALApplyGeoTransform((double *)&raster->fwd_geo, in_x, in_y, out_x, out_y);
}

void grt_coord_to_lp(grt_raster_t * raster, double in_x, double in_y, double * out_x, double * out_y){
  GDALApplyGeoTransform((double *)&raster->inv_geo, in_x, in_y, out_x, out_y);
}
