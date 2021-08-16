# CAN Communication example between two stm32 MCUs.

In this example:
  We set up the MCU's bxCAN peripherals for  CAN communication.
  N1 sends a LED number every 1 second and requests a 2 byte every 4 seconds.
    For time purposes a timer has been configured to generate an exception at every 1 second.
  N2 toggles the LEDs that number's received from N1, and sends data for data requests.
  These situations were tested with acceptance filters also.
  
MCP2551 was used as CAN Receiver, and the application was tested with success at 500Kbps and 1Mbps.
