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

int collatz_conjecture(int n, hid_device *device) {
    int steps = 0;

    while (n > 1) {
        if (n % 2 == 0) {
            n /= 2;
            set_lightbar_color(device, 0, 0, 255); // Blue for even
        } else {
            n = 3 * n + 1;
            set_lightbar_color(device, 255, 0, 0); // Red for odd
        }
        steps++;
        printf("Current value: %d\n", n);
        usleep(500000); // Delay for visualization
    }
    return steps;
}

void goldbach_conjecture(int n, hid_device *device) {
    int found = 0;

    for (int i = 2; i <= n / 2; i++) {
        int j = n - i;
        int is_prime_i = 1, is_prime_j = 1;

        for (int k = 2; k * k <= i; k++) {
            if (i % k == 0) {
                is_prime_i = 0;
                break;
            }
        }
        for (int k = 2; k * k <= j; k++) {
            if (j % k == 0) {
                is_prime_j = 0;
                break;
            }
        }

        if (is_prime_i && is_prime_j) {
            printf("%d = %d + %d\n", n, i, j);
            found = 1;
            set_lightbar_color(device, 0, 255, 0); //Green for success
            sleep(1);
            break;
        }
    }

    if (!found) {
        printf("No Goldbach partition found for %d (not even?)\n", n);
        set_lightbar_color(device, 255, 255, 0); //Yellow for error
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

    // Math conjecture selection
    printf("Choose a conjecture:\n1. Collatz Conjecture\n2. Goldbach's Conjecture\n");
    int choice;

    //TODO: Add instructions to run program
    scanf("%d", &choice);

    int number;
    printf("Enter a number: ");
    scanf("%d", &number);

    switch (choice) {
        
        case 1:
            printf("Visualizing Collatz Conjecture...\n");
            int steps = collatz_conjecture(number, device);
            printf("Collatz Conjecture completed in %d steps.\n", steps);
            break;

        case 2:
            if (number % 2 != 0) {
                printf("Goldbach's Conjecture only applies to even numbers greater than 2.\n");
            } else {
                printf("Visualizing Goldbach's Conjecture...\n");
                goldbach_conjecture(number, device);
            }
            break;

        default:
            printf("Invalid choice.\n");
            set_lightbar_color(device, 255, 0, 0); //Red for invalid input
            break;
    }

    // Turn off the lightbar (set color to black)
    set_lightbar_color(device, 0, 0, 0);

    // Cleanup
    hid_close(device);
    hid_exit();

    printf("Program finished.\n");
    return 0;
}
