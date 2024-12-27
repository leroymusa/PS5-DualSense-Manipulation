#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VENDOR_ID  0x054C   //Sony
#define PRODUCT_ID 0x0CE6   //DualSense

#define INPUT_REPORT_SIZE      64
#define OUTPUT_REPORT_SIZE     78

void check_error(int error,const char* message) {
    if (error<0) {
        fprintf(stderr, "%s: %s\n", message, libusb_strerror(error));
        exit(EXIT_FAILURE);
    }
}

int main() {
    libusb_context *context=NULL;
    libusb_device_handle *handle=NULL;
    unsigned char input_buffer[INPUT_REPORT_SIZE];
    unsigned char output_buffer[OUTPUT_REPORT_SIZE];
    int transferred=0;

    check_error(libusb_init(&context),"libusb init failed");
    handle=libusb_open_device_with_vid_pid(context,VENDOR_ID,PRODUCT_ID);
    if (!handle) {
        fprintf(stderr,"DualSense not found\n");
        libusb_exit(context);
        return EXIT_FAILURE;
    }

    check_error(libusb_claim_interface(handle, 0), "Interface claim failed");

    //Change light bar to purple and enable vibration
    memset(output_buffer, 0, sizeof(output_buffer));
    output_buffer[0]=0x02;       //Report ID
    output_buffer[11]=0xFF;      //Red
    output_buffer[12]=0x00;      //Green
    output_buffer[13]=0xFF;      //Blue
    output_buffer[3]=0x20;       //Right motor enable
    output_buffer[5]=0xFF;       //Right motor strength
    check_error(libusb_interrupt_transfer(handle,0x02,output_buffer,sizeof(output_buffer),&transferred,0),"Output report failed");

    //Read input reports continuously
    while (1) {
        int result=libusb_interrupt_transfer(handle,0x81,input_buffer,sizeof(input_buffer),&transferred,0);
        if (result==LIBUSB_ERROR_TIMEOUT)continue;
        check_error(result,"Input report failed");
    }

    libusb_release_interface(handle,0);
    libusb_close(handle);
    libusb_exit(context);
    return 0;
}
