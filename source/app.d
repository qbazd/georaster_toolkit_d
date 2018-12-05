import std.stdio;
import georaster_toolkit_api;

//gsshared
static void InitializeGdal(){
  grt_init();
};

class GeoRaster{

  this(string file, int band){

  }


  void close(){


  }
}

void main()
{
  grt_init();    

	writeln("Georaster test program");

  auto raster = grt_open(cast(char*)"out.tif",1);

  double out_x;
  double out_y;
  
  grt_lp_to_coord (raster, 10.0, 20.0,  &out_x, &out_y);
  writeln("coord:", cast(int)out_x, "," ,cast(int)out_y);

  double n_x;
  double n_y;

  grt_coord_to_lp (raster, out_x, out_y,  &n_x, &n_y);
  writeln("lp:", cast(int)n_x, "," ,cast(int)n_y);

  auto val = grt_read_point_float32(raster, cast(int)n_x,cast(int)n_y);
  writeln("val:", val);

  grt_close(raster);
}
