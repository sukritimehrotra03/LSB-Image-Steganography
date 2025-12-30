/*******************************************************************************
* Name: Sukriti Mehrotra
* Date: 23/10/2024
* 
* Project Name: LSB Image Steganography
*
* Description: Secret data is encoded into the LSB of each byte in a media
* (.bmp) file and then that media (.bmp) file is decoded to obtain secret data.
*
* Sample Input: 
* For Encoding: ./a.out -e beautiful.bmp secret.txt [Destination_image_file]
* For Decoding: ./a.out -d output.bmp [output_file_name]
*
* Sample Output:
* For Encoding: Destination_image.bmp
* For Decoding: output_file.txt
********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
// User-defined header files
#include "encode.h"
#include "decode.h"
#include "types.h"


// Function prototype for atexit function
void clear_memory_close_fptr(void);

// Globally declare the EncodeInfo and DecodeInfo structure variables
EncodeInfo encInfo;
DecodeInfo decInfo;

int main(int argc, char *argv[])
{
    // atexit function to free the dynamically allocated memory and close the file pointers
    // at the end of the operation
    if(atexit(clear_memory_close_fptr) != 0)
    {
        fprintf(stderr, "Error: Failed to free the dynamically allocated memory and close pointers at exit.\n");
        return 1;
    }

    // Function to validate number of arguments
    Status args = validate_args(argc, argv);
    if(args == e_failure)
    {
        return 1;
    }

    // Function to check the type of operation (encoding or decoding)
    OperationType op_type = check_operation_type(argv[1]);

    // STEP1: Check the op_type is e_encode
    // STEP2: If yes -> Start encode, No -> Goto STEP3
    if(op_type == e_encode)
    {
        printf("Selected Encoding, Encoding started\n\n");
        sleep(1);
        // Validate the arguments/file extensions entered by the user
        Status val = read_and_validate_encode_args(argc, argv, &encInfo);

        if(val == e_success)
        {
            // If arguments are validated, start encoding
            printf("INFO: Arguments Validated!!\n\n");
            if(do_encoding(&encInfo) == e_failure)
            {
                return 1;
            }
        }
        else
        {
            printf("Error: Not Validated, give the correct file extension!!\n");
        }
    }
    // STEP3: Check op_type is e_decode
    // STEP4: Start decode, No -> Goto STEP5
    else if(op_type == e_decode)
    {
        printf("Selected Decoding, Decoding started\n\n");
        sleep(1);
        // Validate the arguments/file extensions entered by the user
        Status val = read_and_validate_decode_args(argc, argv, &decInfo);

        if(val == e_success)
        {
            printf("INFO: Arguments Validated!!\n\n");
            // If arguments are validated, start encoding
            if(do_decoding(&decInfo) == e_failure);
            {
                return 1;
            }
        }
        else
        {
            printf("Error: Not Validated, give the correct file extension!!\n");
        }
    }
    // STEP5: Print error and stop the process
    else
    {
        printf("Error: Enter '-e' or '-d'!!\n");
    }
    return 0;
}

// Function to validate the number of args in each case
Status validate_args(int argc, char *argv[])
{
    // If no arguments are provided after ./a.out
    if(argc < 2)
    {
        printf("INFO: Please pass valid arguments.\n\n");
        printf("INFO: Encoding - minimum 4 arguments. \nUsage :- ./a.out -e source_image_file secret_data_file [Destination_image_file]\n\n");
        printf("INFO: Decoding - minimum 3 arguments. \nUsage :- ./a.out -d encoded_image [output_file_name]\n");
        return e_failure;
    }

    // If encoding is selected and the arguments entered are less than 4
    if(strcmp(argv[1], "-e") == e_success)
    {
        if(argc < 4)
        {
            printf("INFO: For Encoding please pass minimum 4 arguments like ./a.out -e source_file.bmp secret_data.txt [Destination_image.bmp]\n");
            return e_failure;
        }
    }
    // If Decoding is selected and the arguments entered are less than 3
    else if(strcmp(argv[1], "-d") == e_success)
    {
        if(argc < 3)
        {
            printf("INFO: For Decoding please pass minimum 3 arguments like ./a.out -d encoded_imag.bmp [Output_file_name]\n");
            return e_failure;
        }
    }
    // Return success if no errors
    return e_success;
}

// Function to check the type of operation
OperationType check_operation_type(char *argv)
{
    // STEP1: Compare argv with -e
    // STEP2: If yes -> return e_encode, no Goto STEP3
    if(strcmp(argv, "-e") == e_success)
    {
        return e_encode;
    }
    // STEP3: Compare argv with -d
    // STEP4: If yes -> return e_decode, no Goto STEP5
    else if(strcmp(argv, "-d") == e_success)
    {
        return e_decode;
    }
    // STEP5: return e_unsupported
    else
    {
        return e_unsupported;
    }
}

// Function to free the dynamically allocated memory and close file pointers at the end
void clear_memory_close_fptr(void)
{
    // Free allocated memory for encoding
    if(encInfo.src_image_fname) free(encInfo.src_image_fname);
    if(encInfo.secret_fname) free(encInfo.secret_fname);
    if(encInfo.stego_image_fname) free(encInfo.stego_image_fname);

    // Close file pointers for encoding if they are open
    if(encInfo.fptr_secret) fclose(encInfo.fptr_secret);
    if(encInfo.fptr_src_image) fclose(encInfo.fptr_src_image);
    if(encInfo.fptr_stego_image) fclose(encInfo.fptr_stego_image);

    // Free allocated memory for decoding
    if(decInfo.enc_image_fname) free(decInfo.enc_image_fname);
    if(decInfo.extn_secret_file) free(decInfo.extn_secret_file);
    if(decInfo.secret_fname) free(decInfo.secret_fname);

    // Close file pointers for decoding if they are open
    if(decInfo.fptr_enc_image) fclose(decInfo.fptr_enc_image);
    if(decInfo.fptr_secret) fclose(decInfo.fptr_secret);
}