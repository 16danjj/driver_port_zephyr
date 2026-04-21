# driver_port_zephyr
Porting a baremetal MCP25626 CAN driver to zephyr.

Contains only .c, .h files of the driver

"test" is a sample application that uses this driver with a compatible "microchip,mcp25625" in loopback mode, where the controller receives its own CAN Frames that it sends out

PMOD CAN Reference Manual : https://digilent.com/reference/pmod/pmodcan/reference-manual?redirect=1
Datasheet : https://ww1.microchip.com/downloads/en/DeviceDoc/MCP25625-CAN-Controller-Data-Sheet-20005282C.pdf
