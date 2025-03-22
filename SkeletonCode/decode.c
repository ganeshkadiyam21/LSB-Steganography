#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"
#include<stdlib.h>

Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
{
     //validate if proper .bmp file and proper .txt file is passed
     if(strstr(argv[2],".bmp")!=NULL)
     {
        decInfo->stego_src_image_fname=argv[2];
     }
     else
        return e_failure;
    //check if .txt file is given if not give the name
    if(argv[3]!=NULL)
        decInfo ->decode_fname=argv[3];
    else   
        decInfo->decode_fname="decode.txt";
    return e_success;
}


Status open_files_to_decode(DecodeInfo *decInfo)
{
    // Src(stego) Image file
    decInfo->fptr_stego_src_image = fopen(decInfo->stego_src_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_src_image_fname);

    	return e_failure;
    }

    //  Decoded file
    decInfo->fptr_decode = fopen(decInfo->decode_fname, "w");
    // Do Error handling
    if (decInfo->fptr_decode == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decode_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status decode_magic_string(char  *magic_string,DecodeInfo *decInfo)
{
    //calling decode_image_to_data function to decode image to data
    if(decode_image_to_data(magic_string,2,decInfo)==e_success)
    return e_success;
    else
    {
        return e_failure;
    }
}

Status decode_image_to_data(char *data,int size,DecodeInfo *decInfo)
{
    //setting file pointer to start
    fseek(decInfo->fptr_stego_src_image,54,SEEK_SET);
     for(int i=0;i<size;i++)
    {
        //reading 8 bytes for size number of types from stego.bmp file 
        fread(decInfo->image_data,8,1,decInfo->fptr_stego_src_image);

        //calling decode lsb to byte function
        if(decode_lsb_to_byte(data,i,decInfo->image_data)==e_failure)
        {
            return e_failure;
        }
    }
   return e_success;
}

Status decode_lsb_to_byte(char *data,int letter,char *image_data)
{
    //doing bit operations to convert lsb data of 8 bytes to a byte
    char str,data_copy=0;
    for(int i=0;i<8;i++)
    {
        str=image_data[i];
        str=str&1;
        data_copy=(data_copy)|(str<<(7-i));
    }
    //checking if magic string character is matched with decoded character to continue
    if(data_copy!=data[letter])
    {
        return e_failure;
    }
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    //calling decode size to lsb function
    if(decode_size_to_lsb(0,decInfo)==e_success)
    {
        return e_success;
    }
    return e_failure;
}
Status decode_size_to_lsb(int task,DecodeInfo *decInfo)
{
    char str[32];
    decInfo->decode_file_ext_size=0;
    decInfo->decode_file_size=0;

    //reading 32 bytes from stego source file
    fread(str,32,1,decInfo->fptr_stego_src_image);

    //doing neccessary bit operations to decode 32 bytes and get 4 bytes (int)
    for(int i=0;i<32;i++)
    {
        str[i]=str[i]&1;
        //to find file ext size
        if(task==0)
        decInfo->decode_file_ext_size=(decInfo->decode_file_ext_size)|(str[i]<<(31-i));
        //to find file size
        if(task==1)
        decInfo->decode_file_size=(decInfo->decode_file_size)|(str[i]<<(31-i));
    }
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char extn[8],ch;
    //dynamically allocating memory for file extension string
    decInfo->decode_file_ext=(char *)malloc(8*sizeof(char));
    for(int i=0;i<4;i++)
    {
        ch=0;
        //reading 8 bytes from stego source
        fread(extn,8,1,decInfo->fptr_stego_src_image);
        //doing bit operations
        for(int j=0;j<8;j++)
        {
            char str=extn[j]&1;
            ch|=(str<<(7-j));
        }
        //storing character 
        decInfo->decode_file_ext[i]=ch;
    }
    //putting '\0' to indicate end of string
    decInfo->decode_file_ext[4]='\0';
    return e_success;
   
}
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    //calling decode size to lsb function
    if(decode_size_to_lsb(1,decInfo)==e_success)
    {
        return e_success;
    }
    return e_failure;
}
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    //dynamically allocating memory for file size string
    decInfo->decode_data = (char *)malloc(decInfo->decode_file_size + 1);
    for(int i=0;i<decInfo->decode_file_size;i++)
    {
         ch=0;
         //reading 8 bytes from stego source
        fread(decInfo->image_data,1,8,decInfo->fptr_stego_src_image);
        //doing bit operations
        for(int j=0;j<8;j++)
        {
            char str=decInfo->image_data[j]&1;
            ch|=(str<<(7-j));
        }
        //storing character
        decInfo->decode_data[i]=ch;
    }
    //putting '\0' to indicate end of string
    decInfo->decode_data[decInfo->decode_file_size]='\0';
    return e_success;
}

/*
  steps to do encoding
  1.calling open files to decode function to open the file
  2.decoding magic string(#*) by calling decode magic string function and checking if magic string is matched to continue further
  3.decode secret file extension size using decode_secret_file_extn_size function
  4.decode secret file extension using decode_secret_file_extn function
  5.decode secret file size using decode_secret_file_size function
  6.decoding data from stego.bmp using decode_secret_file_data function
*/
Status do_decoding(DecodeInfo *decInfo)
{
   if(open_files_to_decode(decInfo)==e_success)
   {
    printf("Open files is a success \n");
   }
   else{
    printf("Open files is a failure\n");
    return e_failure;
   }
   if(decode_magic_string(MAGIC_STRING,decInfo)==e_success)
   {
    printf("Decoding magic string is a success\n");
   }
   else{
    printf("Decoding magic string is a failure\n");
    return e_failure;
   }
   if(decode_secret_file_extn_size(decInfo)== e_success)
   {
    printf("Decoded secret file extn size successfully\n");
   }
   else{
    printf("Failed to decode secret file extn size\n");
    return e_failure;
   }
   if(decode_secret_file_extn(decInfo)==e_success)
   {
    printf("Decoded secret file extension successfully\n");
   }
   else{
    printf("Failed to decode secret file extension\n");
    return e_failure;
   }
   if(decode_secret_file_size(decInfo)==e_success)
   {
    printf("Decoded secret file size successfully\n");
   }
   else{
    printf("Failed to decode secret file size\n");
    return e_failure;
   }
   if(decode_secret_file_data(decInfo)==e_success)
   {
    printf("Decoded secret file data successfully\n");
   }
   else{
    printf("Failed to decode secret file data\n");
    return e_failure;
   }
   return e_success;
}