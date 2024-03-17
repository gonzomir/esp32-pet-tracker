#define TINY_GSM_MODEM_SIM7600
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG Serial

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  60          // Time ESP32 will go to sleep (in seconds)

#define UART_BAUD    115200
#define MODEM_TX     27
#define MODEM_RX     26
#define MODEM_PWRKEY 4
#define MODEM_DTR    32
#define MODEM_RI     33
#define MODEM_FLIGHT 25
#define MODEM_STATUS 34

#define LOGING true
