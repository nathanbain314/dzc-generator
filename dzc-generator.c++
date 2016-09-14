#include <cmath>
#include <fstream>
#include <sys/stat.h>
#include <vips/vips8>

using namespace vips;
using namespace std;

void build_collection( string input_directory, string output_directory, string output_name )
{
  DIR *dir;
  struct dirent *ent;
  string str;
  int n = 0;

  if ((dir = opendir (input_directory.c_str())) != NULL) 
  {
    while ((ent = readdir (dir)) != NULL)
    {   
      if( ent->d_name[0] != '.' )
      {
        ++n;
      }
    }
  }

  int level = (int)log2( n );

  ostringstream ss;
  ss << output_directory << output_name << "_files/";
  g_mkdir(ss.str().c_str(), 0777);
  ss << level << "/";
  g_mkdir(ss.str().c_str(), 0777);

  int l = (int)ceil( log2( n ) / 2.0 );
  n = 0;

  ofstream dzc_file;
  dzc_file.open(string(output_directory).append(output_name).append(".dzc").c_str());
  dzc_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  dzc_file << "<Collection MaxLevel=\"" << level << "\" TileSize=\"256\" Format=\"jpg\" NextItemId=\"" << n+1 << "\" xmlns=\"http://schemas.microsoft.com/deepzoom/2008\">\n";
  dzc_file << "<Items>\n";

  // Iterate through all images in directory
  if ((dir = opendir (input_directory.c_str())) != NULL) 
  {
    while ((ent = readdir (dir)) != NULL)
    {
      str = input_directory;
      if( ent->d_name[0] != '.' )
      {
        // Load the image
        str.append(ent->d_name);
        
        VImage image = VImage::vipsload( (char *)str.c_str() );

        stringstream ss3;
        ss3 << output_directory << n;

        image.dzsave((char *)ss3.str().c_str());

        int width = image.width();
        int height = image.height();
        double size = ( width > height ) ? width : height;
        int s = (int)ceil( log2( size / 256.0 ) );
        s = 1<<s;

        int row = 0;
        int col = 0;

        for( int i = 0; i < l; ++i )
        {
          row |=  ( ( n >> i ) & 1<<i );
          col |=  ( ( n >> (i+1) ) & 1<<i );
        }

        ostringstream ss2;
        ss2 << ss.str() << row << "_" << col << ".jpg";

        image.shrink( s, s ).embed( 0, 0, 256, 256 ).jpegsave((char *)ss2.str().c_str());

        dzc_file << "<I Id=\"" << n << "\" N=\"" << n << "\" IsPath=\"1\" Source=\"" << n << ".dzi\">\n";
        dzc_file << "<Size Width=\"" << width << "\" Height=\"" << height << "\"/>\n";
        dzc_file << "</I>\n";

        ++n;
      }
    }

    closedir (dir);
  }

  dzc_file << "</Items>\n";
  dzc_file << "</Collection>\n";
  dzc_file.close();

  int size = ( floor( sqrt( n ) ) < sqrt( n ) ) ? ( (int)sqrt(n) )<<1 : (int)sqrt(n);

  for( ; level > 0; --level )
  {
    ostringstream upper, current;
    current << output_directory << output_name << "_files/" << level-1 << "/";
    upper << output_directory << output_name << "_files/" << level << "/";
    g_mkdir(current.str().c_str(), 0777);
    struct stat buf;
    for(int i = 1; i < size; i+=2)
    {
      for(int j = 1; j < size; j+=2)
      {
        if( stat( (char *)upper.str().append(to_string(i)+"_"+to_string(j)+".jpg").c_str(), &buf) == 0 )
        {
          (VImage::jpegload((char *)upper.str().append(to_string(i-1)+"_"+to_string(j-1)+".jpg").c_str(), VImage::option()->set( "shrink", 2)).
          join(VImage::jpegload((char *)upper.str().append(to_string(i)+"_"+to_string(j-1)+".jpg").c_str(), VImage::option()->set( "shrink", 2)),VIPS_DIRECTION_HORIZONTAL)).
          join(VImage::jpegload((char *)upper.str().append(to_string(i-1)+"_"+to_string(j)+".jpg").c_str(), VImage::option()->set( "shrink", 2)).
          join(VImage::jpegload((char *)upper.str().append(to_string(i)+"_"+to_string(j)+".jpg").c_str(), VImage::option()->set( "shrink", 2)),VIPS_DIRECTION_HORIZONTAL),VIPS_DIRECTION_VERTICAL).
          jpegsave((char *)current.str().append(to_string(i>>1)+"_"+to_string(j>>1)+".jpg").c_str() );
        }
        else if( stat( (char *)upper.str().append(to_string(i-1)+"_"+to_string(j)+".jpg").c_str(), &buf) == 0 )
        {
          (VImage::jpegload((char *)upper.str().append(to_string(i-1)+"_"+to_string(j-1)+".jpg").c_str(), VImage::option()->set( "shrink", 2)).
          join(VImage::jpegload((char *)upper.str().append(to_string(i)+"_"+to_string(j-1)+".jpg").c_str(), VImage::option()->set( "shrink", 2)),VIPS_DIRECTION_HORIZONTAL)).
          join(VImage::jpegload((char *)upper.str().append(to_string(i-1)+"_"+to_string(j)+".jpg").c_str(), VImage::option()->set( "shrink", 2)).
          embed(0,0,256,128),VIPS_DIRECTION_VERTICAL).
          jpegsave((char *)current.str().append(to_string(i>>1)+"_"+to_string(j>>1)+".jpg").c_str() );
        }
        else if( stat( (char *)upper.str().append(to_string(i)+"_"+to_string(j-1)+".jpg").c_str(), &buf ) == 0 )
        {
          (VImage::jpegload((char *)upper.str().append(to_string(i-1)+"_"+to_string(j-1)+".jpg").c_str(), VImage::option()->set( "shrink", 2)).
          join(VImage::jpegload((char *)upper.str().append(to_string(i)+"_"+to_string(j-1)+".jpg").c_str(), VImage::option()->set( "shrink", 2)),VIPS_DIRECTION_HORIZONTAL)).
          embed(0,0,256,256).
          jpegsave((char *)current.str().append(to_string(i>>1)+"_"+to_string(j>>1)+".jpg").c_str() );
        }
        else if( stat( (char *)upper.str().append(to_string(i-1)+"_"+to_string(j-1)+".jpg").c_str(), &buf ) == 0 )
        {
          VImage::jpegload((char *)upper.str().append(to_string(i-1)+"_"+to_string(j-1)+".jpg").c_str(), VImage::option()->set( "shrink", 2)).
          embed(0,0,256,256).
          jpegsave((char *)current.str().append(to_string(i>>1)+"_"+to_string(j>>1)+".jpg").c_str() );
        }
      }
    }
  }
}

int main( int argc, char **argv )
{
  build_collection( argv[1], argv[2], argv[3] );
  return 0;
}