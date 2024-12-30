#include <stdio.h>
#include <stdlib.h>
#include <hidapi/hidapi.h>
#include <unistd.h>

#define VENDOR_ID 0x054C
#define PRODUCT_ID 0x0CE6



void set_lightbar_color(hid_device *device, uint8_t r, uint8_t g, uint8_t b) {
    // Output report format for DualSense lightbar
    unsigned char report[64] = {0};

    report[0] = 0x02; // Always 0x02 for output report
    report[1] = 0x01; // Command type
    report[3] = 0x10; // Enable lightbar
    report[6] = 0x04; // Flags to enable LED changes
    report[7] = 0x02; // LED state flag
    report[8] = r;    // Red intensity
    report[9] = g;    // Green intensity
    report[10] = b;   // Blue intensity

    // Send report to the controller
    int res = hid_write(device, report, sizeof(report));
    if (res < 0) {
        fprintf(stderr, "Failed to send lightbar color command: %s\n", hid_error(device));
    } else {
        printf("Lightbar color set to R:%d G:%d B:%d\n", r, g, b);
    }
}



int main() {
    if (hid_init()) {
        fprintf(stderr, "Failed to initialize HIDAPI\n");
        return -1;
    }

    // Open the DualSense controller
    hid_device *device = hid_open(VENDOR_ID, PRODUCT_ID, NULL);
    if (!device) {
        fprintf(stderr, "Failed to open DualSense controller\n");
        return -1;
    }

    printf("DualSense controller connected!\n");

    // Example: Set the lightbar to purple (R:128, G:0, B:128)
    set_lightbar_color(device, 128, 0, 128);

    // Keep the lightbar on for 5 seconds
    sleep(5);

    // Turn off the lightbar (set color to black)
    set_lightbar_color(device, 0, 0, 0);

    // Cleanup
    hid_close(device);
    hid_exit();

    printf("Program finished.\n");
    return 0;
}
