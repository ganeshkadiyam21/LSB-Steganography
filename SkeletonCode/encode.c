#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
   // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
   // printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
     //validate if proper .bmp file and proper .txt file is passed
     if(strstr(argv[2],".bmp")!=NULL)
     {
        encInfo->src_image_fname=argv[2];
     }
     else
        return e_failure;
    if(strstr(argv[3],".txt")!=NULL)
    {
        encInfo->secret_fname=argv[3];
    }
    else
        return e_failure;

    //check if .bmp file is given if not give the name
    if(argv[4]!=NULL)
        encInfo ->stego_image_fname=argv[4];
    else   
        encInfo->stego_image_fname="stego.bmp";
    return e_success;
}


Status check_capacity(EncodeInfo *encInfo)
{
    //size of beautiful.bmp
    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    //size of secret.txt
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);

    //checking the capacity
    if(encInfo->image_capacity>(16+32+32+32+(encInfo->size_secret_file*8)))
    {
        return e_success;
    }
    return e_failure;
}

uint get_file_size(FILE *fptr)
{

    //getting the size of .txt file
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
    char str[54];

    //moving the pointer to start
    fseek(fptr_src_image,0,SEEK_SET);

    //reading and writing first 54 bits 
    fread(str,54,1,fptr_src_image);
    fwrite(str,54,1,fptr_dest_image);
    return e_success;
}
Status encode_magic_string(char  *magic_string,EncodeInfo *encInfo)
{

    //calling function to encode data to image
    encode_data_to_image(magic_string,2,encInfo);
    return e_success;
}

Status encode_data_to_image(char *data,int size,EncodeInfo *encInfo)
{
    //encoding characters
    for(int i=0;i<size;i++)
    {
        //reading 8 bytes from source file
        fread(encInfo-> image_data,8,1,encInfo->fptr_src_image);

        //calling encode byte to lsb function
        encode_byte_to_lsb(data[i],encInfo->image_data);

        //writing encoded 8 bytes data to destination file
        fwrite(encInfo->image_data,8,1,encInfo->fptr_stego_image);
    }
}

Status encode_byte_to_lsb(char data,char *image_buffer)
{
    //changing lsb of 8 pixels to store a char(8 bits)
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=(image_buffer[i] & 0xFE) | (data >> (7-i) & 1);
    }
}
Status encode_secret_file_extn_size(int size,EncodeInfo *encInfo)
{
    //calling encode size to lsb function
    encode_size_to_lsb(size,encInfo);
    return e_success;
}
Status encode_size_to_lsb(int size,EncodeInfo *encInfo)
{
    char str[32];
     //read 32 bytes from beautiful.bmp
    fread(str,32,1,encInfo ->fptr_src_image); 

    //changing lsb of 32 bytes read from source file to encode an int
    for(int i=0;i<32;i++)
    {
        str[i]=(str[i] &0xFE)|((size >>(31-i))&1);
    }

    //write back 32 bytes to stego.bmp
    fwrite(str,32,1,encInfo->fptr_stego_image);
}
Status encode_secret_file_extn(char *file_extn,EncodeInfo *encInfo)
{
    //calling encode data to image function
    encode_data_to_image(file_extn,4,encInfo);
    return e_success;
}
Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{
    //calling encode size to lsb function
    encode_size_to_lsb(file_size,encInfo);
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //setting file pointer to start 
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    char str[encInfo->size_secret_file];

    //reading data from secret.txt
    fread(str,encInfo->size_secret_file,1,encInfo->fptr_secret);

    //calling encode_data_to_image
    encode_data_to_image(str,encInfo->size_secret_file,encInfo);
    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
    char ch;
    while(1)
    {
        //reading from source.bmp until you reach end of file
        fread(&ch,1,1,fptr_src);
        if(feof(fptr_src)!=0)
            break;
        fwrite(&ch,1,1,fptr_dest);
    }
    return e_success;
}

/*
  steps to do encoding
  1.calling open files function to open the file
  2.calling check capacity function to check the capacity
  3.copying bmp header by calling copy bmp header function(which contains all source file details)
  4.encoding magic string(#*) by calling encode magic string function
  5.encode secret file extension size using encode_secret_file_extn_size function
  6.encode secret file extension using encode_secret_file_extn function
  7.encode secret file size using encode_secret_file_size function
  8.encoding data from secret.txt using encode_secret_file_data function
  9.encoding remaining data using copy_remaining_img_data function
*/
Status do_encoding(EncodeInfo *encInfo)
{
   if(open_files(encInfo)==e_success)
   {
    printf("Open files is a success \n");
   }
   else{
    printf("Open files is a failure\n");
    return e_failure;
   }
   if(check_capacity(encInfo)==e_success)
   {
    printf("Check capacity function is a success \n");
   }
   else{
    printf("check capacity function is a failure\n");
    return e_failure;
   }
   if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
   {
    printf("Copying BMP header is a success\n");
   }
   else
   {
    printf("Copying BMP header is a failure\n");
    return e_failure;
   }
   if(encode_magic_string(MAGIC_STRING, encInfo)==e_success)
   {
    printf("Encoding magic string is a success\n");
   }
   else{
    printf("Encoding magic string is a failure\n");
    return e_failure;
   }
   //Extract the extension from the file name
   strcpy(encInfo->extn_secret_file,strchr(encInfo->secret_fname,'.'));
   if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)== e_success)
   {
    printf("Encoded secret file extn size successfully\n");
   }
   else{
    printf("Failed to encode secret file extn size\n");
    return e_failure;
   }
   if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)
   {
    printf("Encoded secret file extension successfully\n");
   }
   else{
    printf("Failed to Encode secret file extension\n");
    return e_failure;
   }
   if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
   {
    printf("Encoded secret file size successfully\n");
   }
   else{
    printf("Failed to Encode secret file size\n");
    return e_failure;
   }
   if(encode_secret_file_data(encInfo)==e_success)
   {
    printf("Encoded secret file data successfully\n");
   }
   else{
    printf("Failed to Encode secret file data\n");
    return e_failure;
   }
   if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
   {
    printf("Copied remaining data successfully\n");
   }
   else{
    printf("Failed to copy remaining data\n");
    return e_failure;
   }
   return e_success;
}