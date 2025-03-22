/*
Name : Ganesh
Date : 08/02/2025
Project title : LS bit steganography
Description : Hide the message frrom secret.txt in stego.bmp file by encoding and retrive the message into decode.txt by decoding
*/



//Allneccessary header files
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"

int main(int argc,char **argv)
{

    //check if atleast 3 CLA are passed
    if(argc<4)
    {
        printf("Minimum command line arguments are not given\n");
        return 1;
    }

    //check if the user selected encoding or decoding   
    if(check_operation_type(argv)== e_encode)
    {
        printf("Selected encoding \n");

        //structure EncodeInfo
        EncodeInfo encoding_structure;

        //calling read and validate encode function to check if the argv is valid  
        if(read_and_validate_encode_args(argv,&encoding_structure)==e_success)
        {
            printf("Read and validate function is a success \n");
            printf("<-----Started to encode----->\n");

            //calling do encoding function to start encoding
            if(do_encoding(&encoding_structure)==e_success)
            {

                //successfully completed encoding
                printf("Successfully completed the encoding \n");

                //closing the files opened
                fclose(encoding_structure.fptr_secret);
                fclose(encoding_structure.fptr_src_image);
                fclose(encoding_structure.fptr_stego_image);
            }
            else
            {

                //printing failed to encode
                printf("Failed to perform encoding \n");
                return 2;
            }
        }
        else
        {

            //printing if agrv is not valid
            printf("Read and validate function is a failure \n");
            return 1;
        }

    }

    //decoding mode
    else if(check_operation_type(argv)== e_decode)
    {
        printf("Selected decoding \n");

        //structure DecodeInfo
        DecodeInfo decoding_structure;
        if(read_and_validate_decode_args(argv,&decoding_structure)==e_success)
        {
            printf("Read and validate function is a success\n ");
            printf("<---------Started to decode--------->\n");

            //calling do decoding function to start decoding
            if(do_decoding(&decoding_structure)==e_success)
            {
                //writing decoded message into decode.txt file
                fwrite(decoding_structure.decode_data,decoding_structure.decode_file_size,1,decoding_structure.fptr_decode);
                printf("Successfully completed decoding\n");

                //closing files opened while decoding
                fclose(decoding_structure.fptr_decode);
                fclose(decoding_structure.fptr_stego_src_image);
            }
            else{

                //printing if decoding failed
                printf("Failed to perform decoding\n");
                return 2;
            }
        }
         else
        {

            //printing if read and validate function failed
            printf("Read and validate function is a failure \n");
            return 1;
        }
    }
    else
    {

        //if cla is not atleast 3
        printf("Invalid option \nFor encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]\nFor decoding : ./a.out -d stego.bmp decode.txt\n");
    }

    return 0;
}

OperationType check_operation_type(char *argv[])
{

    //checking first argv string if encoding is selected
    if(strcmp(argv[1], "-e")==0)
        return e_encode;

    //checking first argv string if decoding is selected
    else if(strcmp(argv[1],"-d")==0)
        return e_decode;
    else
        return e_unsupported;
}