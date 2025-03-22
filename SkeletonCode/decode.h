#ifndef DECODE_H
#define DECODE_H

#include "types.h"

/* 
 * Structure to store information required for
 * decoding stego.bmp file to decode.txt file
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DECODEINFO
{
    /* Source Image Info */
    char *stego_src_image_fname;
    FILE *fptr_stego_src_image;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /*Decode.txt Info*/
    char *decode_fname;
    FILE *fptr_decode;  
    char *decode_data;
    int decode_file_size;
    int decode_file_ext_size;
    char *decode_file_ext;
}DecodeInfo;    

/*decoding function prototype*/

/* Perform the decoding */
Status do_decoding(DecodeInfo *DecInfo);

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_to_decode(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(char *magic_string,DecodeInfo *decInfo);

/* decode function, which does the real decoding */
Status decode_image_to_data(char *data, int size, DecodeInfo *encInfo);

/* decode LSB of image data array into a byte  */
Status decode_lsb_to_byte(char *data,int letter,char *image_data);

/*Decode secret file extn size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/*Decode lsb to the integer*/
Status decode_size_to_lsb(int task,DecodeInfo *decInfo);

/*Decode secret file extn*/
Status decode_secret_file_extn(DecodeInfo *decInfo);

/*Decode secret file size*/
Status decode_secret_file_size(DecodeInfo *decInfo);

/*Decode secret file extn size*/
Status decode_secret_file_data(DecodeInfo *decInfo);
#endif

