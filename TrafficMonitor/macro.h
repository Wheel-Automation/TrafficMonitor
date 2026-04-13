#ifndef WHELL_JETSON_VERSION_MACRO_HEADER_H
#define WHELL_JETSON_VERSION_MACRO_HEADER_H

#define PASSWORD_LOGIN          0               // user name and password for launch app
#define KEY_ENABLE              0
#define DEVTYPE                 2               //  enum defined in SUPT_DEV_TYPE(0: jetson. 1: c308, 2: rockchip)
#define SOCKET_REPEAT_TIMES     5               // repeat time for socket connection
#define SOCKET_PORT             41086           // device connection port
#define SERVER_PORT             51086           // server port
#define ROCKCHIP_PORT           7000            // rockchip ip server port
#define ADMIN                   true
#define TWO_DIRECTION_MODE      true
#define LOG_FILE                false            // log if true
#define COMPORT_SEND_WHLTYPE    0                // 1: send wheel type at comport, 0: don't send wheel type
#define AUTOHIDE_TIME           1000* 60 * 3                    // auto hide time when no user action
#define HEART_BIT_RATE          15                              // speed of heart bit in seconds
#define NETWORK_RESTART_TIME    180              // restart socket server at every 30 seconds
#define PORT_CHECK_TIME         5
#define AUTO_LOGIN              1
#ifndef QT_NO_DEBUG
#define CHECK_TRUE(instruction) Q_ASSERT(instruction)
#else
#define CHECK_TRUE(instruction) (instruction)
#endif

#endif // WHELL_JETSON_VERSION_MACRO_HEADER_
