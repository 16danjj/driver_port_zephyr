#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>

#define CAN_ID_123 0x123

const struct device *can_dev;

void rx_callback_function(const struct device *dev, struct can_frame *frame, void *user_data) {
    printf("Received frame with %d bytes\n", frame->dlc);
    printf("5th byte is %d\n", frame->data[4]);
}

void tx_callback(const struct device *dev, int error, void *user_data) {

    char *sender = (char *)user_data;

    if (error != 0) {
        printf("Sending failed [%d]\n Sender: %s\n", error, sender);
    }

    printf("TX Successful \n");
}

int main(void) {

    int ret, filter_id; 

    const struct can_filter filter_one = {
            .flags = 0,
            .id = CAN_ID_123,
            .mask = CAN_STD_ID_MASK
    };
    
    can_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

    if (!device_is_ready(can_dev)) {
        printf("CAN device not ready!\n");
        return 1;
    }

    ret = can_set_mode(can_dev, CAN_MODE_LOOPBACK);
    if (ret != 0) {
        printf("Error setting CAN mode [%d]", ret);
        return 0;
    }

    ret = can_start(can_dev);
    if (ret != 0) {
        printf("Error starting CAN controller [%d]", ret);
        return 0;
    }

    filter_id = can_add_rx_filter(can_dev, rx_callback_function, NULL, &filter_one);
    if (filter_id < 0) {
        printf("Unable to add rx filter [%d]", filter_id);
    } 

    struct can_frame frame = {
        .flags = 0,
        .id = CAN_ID_123,
        .dlc = 8,
        .data = {1,2,3,4,16,6,7,8}
    };
    
    ret = can_send(can_dev, &frame, K_FOREVER, tx_callback, "Sender 1"); 
    
    if (ret != 0) {
        printf("Error sending [%d]", ret);
        return 0;
    }

    return 0;
}