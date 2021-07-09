// Target Terror Raw Thrills Filesystem (RTFS) Patches
#include "rtfs.h"
#include "rtfs_util.h"

// Raw Thrills wraps open, hashes the path, converts it to a string, and then opens the mangled filename.
int rtfs_open(char* src){

    char fpath[1024] = {0x00};
    sprintf(fpath,"./data/%s",src);
    RTFS_StandardizeFilename(fpath);
    // If the resolved file path exists, we open it.
    if(path_exists(fpath)){
        return open(fpath,0);
    }
    // If not, we have to hash it, dump from their rtfs setup, and then open it.
    rtfs_fixfile(src,fpath,0);
    if(path_exists(fpath)){
        return open(fpath,0);
    }
    rfx_printf("rtfs_open: %s Not Found\n",fpath);
    return 0;
}

// Raw Thrills wraps fopen, hashes the path, converts it to a string, and then opens the mangled filename.
FILE* rtfs_fopen(char* src, int modes){

    char fpath[1024] = {0x00};
    sprintf(fpath,"./data/%s",src);
    RTFS_StandardizeFilename(fpath);
    // If the resolved file path exists, we open it.
    if(path_exists(fpath)){
        return fopen(fpath,modes);
    }
    // If not, we have to hash it, dump from their rtfs setup, and then open it.
    rtfs_fixfile(src,fpath,0);
    if(path_exists(fpath)){
        return fopen(fpath,modes);
    }
    rfx_printf("rtfs_fopen: %s Not Found\n",fpath);
    return 0;
}

// Sometimes it's not about opening a file, but translating a stored name to add it to a list.
void RTFS_TranslateFilename(char*src,char*dest){

    char fpath[1024] = {0x00};
    sprintf(fpath,"./data/%s",src);
    RTFS_StandardizeFilename(fpath);
    fixslashes(&fpath);
    //rfx_printf("Translate: %s\n",fpath);
    // If the resolved file path exists, we open it.
    if(path_exists(fpath)){
        memcpy(dest,fpath,0x9C);
        return;
    }
    // If not, we have to hash it, dump from their rtfs setup, and then open it.
    rtfs_fixfile(src,fpath,0);
    if(path_exists(fpath)){
        memcpy(dest,fpath,0x9C);
        return;
    }
    // Sometimes, files won't be found... mostly due to older versions of the game where files had been removed.
    rfx_printf("%s Not Found (Translate)\n",fpath);
    return;
}

// Resolves the filename, opens a compressed file (some tiff lzw derivative compression), and then returns an object.
struct lzwobj* RTFS_LZWfopen(char*src,char* a2){

    struct lzwobj *lzw = malloc(0x10);
    if(lzw == NULL){
        rfx_printf("Failed to Allocate lzw Object Memory Block\n");
        return 0;
    }

    lzw->is_rb = 0;
    lzw->idk_8 = 0;
    char tb[4]={0x00};
    memcpy(tb,a2,4);

    // Check for rb mode.
    if(tb[1] == 0x42 || tb[1] == 0x62){
        lzw->is_rb = 1;
    }

    // First, get the final name if it needs converting.
    char fpath[1024] = {0x00};
    sprintf(fpath,"./data/%s",src);
    RTFS_StandardizeFilename(fpath);

    // If the uncompressed file exists, open, read, update object, return.
    if(path_exists(fpath) != NULL){
        FILE *up = fopen(fpath,"rb");
        unsigned int fsz = 0;
        fseek(up,0,2);
        fsz =  ftell(up);
        fseek(up,0,0);
        unsigned char* udata = (unsigned char*)malloc(fsz);
        lzw->uncompressed_filesize = fsz;
        lzw->uncompressed_buffer = udata;
        fread(lzw->uncompressed_buffer,fsz,1,up);
        fclose(up);
        return lzw;


    }
    //rfx_printf("%s\n",src);
    unsigned char* crpt = (unsigned char*)malloc(16);
    unsigned char* crpt2 = (unsigned char*)malloc(64);
    crpt = rtfs_convert_name(src);
    sprintf(crpt2,"./enc/%s",crpt);
    if(path_exists(crpt2) == NULL){
        rfx_printf("Compressed File %s not found\n",crpt2);
        return 0;
    }

    // Stop... FILE TIME!
    FILE *cp = fopen(crpt,"rb");
    unsigned int uncompressed_filesize = 0;
    unsigned short filename_length = 0;
    fread(&uncompressed_filesize,4,1,cp);
    lzw->uncompressed_filesize = uncompressed_filesize;
    fread(&filename_length,2,1,cp);
    rfx_printf("Filename Length Reported is: %d\n",filename_length);
    fseek(cp,0,0);
    unsigned char lzwhdr[8]={0x00};
    fread(lzwhdr,8,1,cp);
    char* fnff = (char*)malloc(filename_length+1);
    memset(fnff,0x00,filename_length+1);
    fread(fnff,filename_length,1,cp);
    unsigned int start_offset = ftell(cp);
    fseek(cp,0,2);
    unsigned int end_offset =  ftell(cp);
    unsigned int compressed_filesize = end_offset - start_offset;
    rfx_printf("START OFFSET: %d\n",start_offset);
    rfx_printf("Compressed Filesize: %d\n",compressed_filesize);
    fseek(cp,start_offset,0);
    rfx_printf("Opened Mangled File: %s\n",crpt);

    // Failure Check 1
    if(strstr(src,"mainpls") == NULL){
        if(check_decode_name(src,fnff)){
            free(fnff);
            free(crpt);
            fclose(cp);
            return 0;
        }
    }


    char* uncompressed_buffer = (unsigned char*)malloc(uncompressed_filesize);
    char* compressed_buffer = (unsigned char*)malloc(compressed_filesize);
    lzw->uncompressed_buffer = uncompressed_buffer;
    fread(compressed_buffer,compressed_filesize,1,cp);
    // Setting these up for later.
    typedef int func(unsigned char*,unsigned char*);
    func* lzw_decompress = (func*)0x080DB500;
    int retval = lzw_decompress((int)compressed_buffer, (char*)lzw->uncompressed_buffer);
    //printf("%s\n",lzw->uncompressed_buffer);
    //Save the file to disk.
    create_path(fpath);
    FILE *gp = fopen(fpath,"wb");
    fwrite(lzw->uncompressed_buffer,uncompressed_filesize,1,gp);
    fclose(gp);
    free(compressed_buffer);
    free(fnff);
    free(crpt);
    return lzw;
}

// This was used to dump some of the compressed files that hadn't been accessed, no longer needed.
void force_unpack(char* srcz,char*destpath){
    struct lzwobj *lzw = RTFS_LZWfopen(srcz,"rb");
    FILE *gp = fopen(destpath,"wb");
    fwrite(lzw->uncompressed_buffer,lzw->uncompressed_filesize,1,gp);
    fclose(gp);
}
