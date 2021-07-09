// CCN Extractor by Batteryshark on 01/26/2013

/*
This program extracts the contents of a Multimedia Fusion 2 CCN "PAME" file.
*/

#include <stdio.h>
#include "zlib\zconf.h"
#include "zlib\zlib.h"
#include <string>

struct pameheader{


}phd;

struct fileheader{
	unsigned int filenameLen;
	unsigned long uncompressed_size;
	unsigned long compressed_size;

}curfile;


void test_inflate(Byte *compr, uLong comprLen,
                            Byte *uncompr, uLong uncomprLen){

    int err;
    z_stream d_stream; /* decompression stream */

printf("WOO");

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = compr;
    d_stream.avail_in = 0;
    d_stream.next_out = uncompr;

    err = inflateInit(&d_stream);
  

    while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        err = inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END) break;
     
    }

    err = inflateEnd(&d_stream);
   
}

int main(int argc,char*args[]){
	FILE * sample;
	sample = fopen("sample2","rb");
	if(!sample) return 1;

	FILE * sampleout;
	sampleout = fopen("out2","wb");

	fread(&curfile,12,1,sample);
	
	Bytef *compbuffer;
	Bytef *uncompbuffer;
	compbuffer=(Bytef *)malloc(curfile.compressed_size+1);
	uncompbuffer=(Bytef *)malloc(curfile.uncompressed_size+1);
	

	fread(compbuffer, curfile.compressed_size, 1, sample);

	uncompress(uncompbuffer,(uLongf *)&curfile.uncompressed_size,compbuffer,curfile.compressed_size);
	//fwrite(compbuffer,curfile.compressed_size,1,sampleout);
	
	fwrite(uncompbuffer,curfile.uncompressed_size,1,sampleout);
	
	fclose(sampleout);
		fclose(sample);
	return 0;
}