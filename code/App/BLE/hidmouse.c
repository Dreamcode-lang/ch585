/********************************** (C) COPYRIGHT *******************************
 * File Name          : hidmouse.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/10
 * Description        : 蓝牙鼠标应用程序，初始化广播连接参数，然后广播，直至连接主机后，定时上传键值
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "CONFIG.h"
#include "battservice.h"
#include "devinfoservice.h"
#include "hiddev.h"
#include "hidmouse.h"
#include "hidmouseservice.h"
#include "../../Bsp/Disp/bsp_disp.h"
#include "../../Bsp/I2C/bsp_i2c_bus.h"
#include "../../Bsp/IMU/bsp_bmi270.h"
#include "../../Bsp/HR/bsp_max30100.h"
#include "../../Bsp/Motor/bsp_motor.h"
#include "../../Bsp/TouchPad/bsp_touchpad.h"
#include <stdio.h>

/*********************************************************************
 * MACROS
 */

// Selected HID mouse button values
#define MOUSE_BUTTON_NONE                    0x00
#define MOUSE_BUTTON_LEFT                    0x01
#define MOUSE_BUTTON_RIGHT                   0x02

#define HID_MOUSE_LEFT_KEY_PIN                GPIO_Pin_3
#define HID_MOUSE_RIGHT_KEY_PIN               GPIO_Pin_6
#define HID_MOUSE_KEY_DEBOUNCE_TICKS         1U

// HID mouse input report length
#define HID_MOUSE_IN_RPT_LEN                 4

#define BMI270_LOG_INTERVAL                  50U
#define BMI270_MOUSE_REPORT_PERIOD           20U
#define TOUCH_UPDATE_PERIOD                  50U
#define HEART_RATE_UPDATE_PERIOD            200U
#define I2C_LOW_PRIORITY_COOLDOWN_TICKS     2U
#define DISPLAY_UPDATE_PERIOD                100U
#define DISPLAY_PAGE_STATUS                  0U
#define DISPLAY_PAGE_TIME                    1U
#define TOUCH_PAGE_SWITCH_COOLDOWN_TICKS    6U
#define TOUCH_LONG_PRESS_TICKS             8U
#define TOUCH_READ_FAIL_REINIT_TICKS       2U
#define TOUCH_INIT_RETRY_TICKS            10U
#define MAX30100_RETRY_TICKS                 20U
#define MAX30100_READ_FAIL_REINIT_TICKS     3U
#define MAX30100_PROCESS_SAMPLES            10U
#define HEART_RATE_ALERT_THRESHOLD           140U
#define HEART_RATE_ALERT_CLEAR               135U
#define HEALTH_SAFE_GAIN_DIV                 360
#define MOTOR_ALERT_ON_TICKS                 4U
#define MOTOR_ALERT_OFF_TICKS                2U
#define BMI270_CALIB_SAMPLES                 64U
#define BMI270_TILT_DEADZONE                180
#define BMI270_TILT_GAIN_DIV                 180
#define BMI270_MOUSE_MAX_STEP                10
#define BMI270_MOUSE_BUILD_TAG               "tilt-key-hid-20260626-4"
#define BMI270_READ_FAIL_REINIT_TICKS        3U

/*********************************************************************
 * CONSTANTS
 */
// Param update delay
#define START_PARAM_UPDATE_EVT_DELAY         12800

// Param update delay
#define START_PHY_UPDATE_DELAY               1600

// HID idle timeout in msec; set to zero to disable timeout
#define DEFAULT_HID_IDLE_TIMEOUT             60000

// Minimum connection interval (units of 1.25ms)
#define DEFAULT_DESIRED_MIN_CONN_INTERVAL    8

// Maximum connection interval (units of 1.25ms)
#define DEFAULT_DESIRED_MAX_CONN_INTERVAL    8

// Slave latency to use if parameter update request
#define DEFAULT_DESIRED_SLAVE_LATENCY        0

// Supervision timeout value (units of 10ms)
#define DEFAULT_DESIRED_CONN_TIMEOUT         500

// Default passcode
#define DEFAULT_PASSCODE                     0

// Default GAP pairing mode
#define DEFAULT_PAIRING_MODE                 GAPBOND_PAIRING_MODE_WAIT_FOR_REQ

// Default MITM mode (TRUE to require passcode or OOB when pairing)
#define DEFAULT_MITM_MODE                    FALSE

// Default bonding mode, TRUE to bond
#define DEFAULT_BONDING_MODE                 TRUE

// Default GAP bonding I/O capabilities
#define DEFAULT_IO_CAPABILITIES              GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT

// Battery level is critical when it is less than this %
#define DEFAULT_BATT_CRITICAL_LEVEL          6

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Task ID
static uint8_t hidEmuTaskId = INVALID_TASK_ID;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// GAP Profile - Name attribute for SCAN RSP data
static uint8_t scanRspData[] = {
    0x0A,                           // length of this data
    GAP_ADTYPE_LOCAL_NAME_COMPLETE, // AD Type = Complete local name
    'H',
    'I',
    'D',
    ' ',
    'M',
    'o',
    'u',
    's',
    'e',
    // connection interval range
    0x05, // length of this data
    GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE,
    LO_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL), // 100ms
    HI_UINT16(DEFAULT_DESIRED_MIN_CONN_INTERVAL),
    LO_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL), // 1s
    HI_UINT16(DEFAULT_DESIRED_MAX_CONN_INTERVAL),

    // service UUIDs
    0x05, // length of this data
    GAP_ADTYPE_16BIT_MORE,
    LO_UINT16(HID_SERV_UUID),
    HI_UINT16(HID_SERV_UUID),
    LO_UINT16(BATT_SERV_UUID),
    HI_UINT16(BATT_SERV_UUID),

    // Tx power level
    0x02, // length of this data
    GAP_ADTYPE_POWER_LEVEL,
    0 // 0dBm
};

// Advertising data
static uint8_t advertData[] = {
    // flags
    0x02, // length of this data
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_LIMITED | GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED,

    // appearance
    0x03, // length of this data
    GAP_ADTYPE_APPEARANCE,
    LO_UINT16(GAP_APPEARE_HID_MOUSE),
    HI_UINT16(GAP_APPEARE_HID_MOUSE)};

// Device name attribute value
static CONST uint8_t attDeviceName[GAP_DEVICE_NAME_LEN] = "HID Mouse";

// HID Dev configuration
static hidDevCfg_t hidEmuCfg = {
    DEFAULT_HID_IDLE_TIMEOUT, // Idle timeout
    HID_FEATURE_FLAGS         // HID feature flags
};

static uint16_t hidEmuConnHandle = GAP_CONNHANDLE_INIT;
static uint8_t hidMouseReportEnabled = 0;
static BSP_BMI270_Device_t bmi270Dev;
static uint8_t bmi270Ready = 0;
static int16_t bmi270FiltTiltX = 0;
static int16_t bmi270FiltTiltY = 0;
static uint16_t bmi270LogDivider = 0;
static uint8_t hidMouseLastReportStatus = SUCCESS;
static uint16_t hidMouseReportSendCount = 0;
static uint8_t bmi270ReadFailCount = 0;
static uint8_t hidMouseButtons = MOUSE_BUTTON_NONE;
static uint8_t hidMouseButtonSample = MOUSE_BUTTON_NONE;
static uint8_t hidMouseButtonStable = MOUSE_BUTTON_NONE;
static uint8_t hidMouseButtonDebounce = 0;
static uint8_t hidMouseButtonChanged = 0;
static uint8_t displayBleConnected = 0;
static int8_t displayMouseX = 0;
static int8_t displayMouseY = 0;
static uint8_t displayHeartRate = 0;
static uint8_t displayHrReady = 0;
static uint8_t displayHrLine = 0;
static uint8_t displayPage = DISPLAY_PAGE_TIME;
static uint8_t displayPageRedraw = 1;
static uint8_t touchReady = 0;
static uint8_t touchLastPressed = 0;
static uint8_t touchSwitchCooldown = 0;
static uint8_t touchPressTicks = 0;
static uint8_t touchLongHandled = 0;
static uint8_t touchReadFailCount = 0;
static uint8_t touchInitRetryTicks = 0;
static uint8_t presenterMode = 0;
static int8_t presenterLastAction = 0;
static uint8_t timePageLine = 0;
static uint8_t max30100RetryTicks = 0;
static uint8_t max30100ReadFailCount = 0;
static uint8_t i2cLowPriorityCooldown = 0;
static uint32_t max30100LastProcessMs = 0;
static BSP_MAX30100_State_t max30100State;
static uint8_t heartRateAlertActive = 0;
static uint8_t healthSafeMode = 0;
static uint8_t motorAlertOn = 0;
static uint8_t motorAlertTicks = 0;
static uint16_t clockYear = 2026;
static uint8_t clockMonth = 1;
static uint8_t clockDay = 1;
static uint8_t clockHour = 0;
static uint8_t clockMinute = 0;
static uint8_t clockSecond = 0;
static uint32_t clockLastMs = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void    hidEmu_ProcessTMOSMsg(tmos_event_hdr_t *pMsg);
static void    hidEmuMouseKeysInit(void);
static uint8_t hidEmuMouseKeysScan(void);
static void    hidEmuProcessBmi270Mouse(void);
static void    hidEmuTogglePresenterMode(void);
static void    hidEmuUpdateDisplay(void);
static void    hidEmuProcessHeartRate(void);
static void    hidEmuRecoverI2c(uint8_t restore_touch);
static void    hidEmuProcessMotorAlert(void);
static void    hidEmuTouchInit(void);
static void    hidEmuProcessTouchPageSwitch(void);
static void    hidEmuSwitchDisplayPage(uint8_t page);
static void    hidEmuClockInit(void);
static void    hidEmuClockUpdate(void);
static void    hidEmuClockTickSecond(void);
static uint8_t hidEmuClockMonthFromBuild(const char *month);
static uint8_t hidEmuClockDaysInMonth(uint16_t year, uint8_t month);
static uint8_t hidEmuClockIsLeapYear(uint16_t year);
static void    hidEmuUpdateDisplayPage(void);
static void    hidEmuDrawTimeFrame(void);
static void    hidEmuUpdateTimePage(void);
static void    hidEmuUpdateStatusPage(void);
static int16_t hidEmuApplyDeadzone(int16_t value, int16_t deadzone);
static int8_t  hidEmuClampMouseStep(int16_t value);
static uint8_t hidEmuSendMouseReport(uint8_t buttons, uint8_t X_data, uint8_t Y_data);
static uint8_t hidEmuSendMouseWheelReport(int8_t wheel);
static uint8_t hidEmuRptCB(uint8_t id, uint8_t type, uint16_t uuid,
                           uint8_t oper, uint16_t *pLen, uint8_t *pData);
static void    hidEmuEvtCB(uint8_t evt);
static void    hidEmuStateCB(gapRole_States_t newState, gapRoleEvent_t *pEvent);

/*********************************************************************
 * PROFILE CALLBACKS
 */

static hidDevCB_t hidEmuHidCBs = {
    hidEmuRptCB,
    hidEmuEvtCB,
    NULL,
    hidEmuStateCB};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      HidEmu_Init
 *
 * @brief   Initialization function for the HidEmuKbd App Task.
 *          This is called during initialization and should contain
 *          any application specific initialization (ie. hardware
 *          initialization/setup, table initialization, power up
 *          notificaiton ... ).
 *
 * @param   task_id - the ID assigned by TMOS.  This ID should be
 *                    used to send messages and set timers.
 *
 * @return  none
 */
void HidEmu_Init()
{
    hidEmuTaskId = TMOS_ProcessEventRegister(HidEmu_ProcessEvent);
    hidEmuMouseKeysInit();
    BSP_Motor_Init();
    hidEmuClockInit();
    hidEmuTouchInit();

    // Setup the GAP Peripheral Role Profile
    {
        uint8_t initial_advertising_enable = TRUE;

        // Set the GAP Role Parameters
        GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);

        GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);
        GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);
    }

    // Set the GAP Characteristics
    GGS_SetParameter(GGS_DEVICE_NAME_ATT, GAP_DEVICE_NAME_LEN, (void *)attDeviceName);

    // Setup the GAP Bond Manager
    {
        uint32_t passkey = DEFAULT_PASSCODE;
        uint8_t  pairMode = DEFAULT_PAIRING_MODE;
        uint8_t  mitm = DEFAULT_MITM_MODE;
        uint8_t  ioCap = DEFAULT_IO_CAPABILITIES;
        uint8_t  bonding = DEFAULT_BONDING_MODE;
        GAPBondMgr_SetParameter(GAPBOND_PERI_DEFAULT_PASSCODE, sizeof(uint32_t), &passkey);
        GAPBondMgr_SetParameter(GAPBOND_PERI_PAIRING_MODE, sizeof(uint8_t), &pairMode);
        GAPBondMgr_SetParameter(GAPBOND_PERI_MITM_PROTECTION, sizeof(uint8_t), &mitm);
        GAPBondMgr_SetParameter(GAPBOND_PERI_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
        GAPBondMgr_SetParameter(GAPBOND_PERI_BONDING_ENABLED, sizeof(uint8_t), &bonding);
    }

    // Setup Battery Characteristic Values
    {
        uint8_t critical = DEFAULT_BATT_CRITICAL_LEVEL;
        Batt_SetParameter(BATT_PARAM_CRITICAL_LEVEL, sizeof(uint8_t), &critical);
    }

    // Set up HID keyboard service
    Hid_AddService();

    // Register for HID Dev callback
    HidDev_Register(&hidEmuCfg, &hidEmuHidCBs);

    // Setup a delayed profile startup
    tmos_set_event(hidEmuTaskId, START_DEVICE_EVT);
    if(BSP_MAX30100_Init(&max30100State) == BSP_MAX30100_OK)
    {
        displayHrReady = 1;
        displayHeartRate = 0;
        max30100RetryTicks = 0;
        max30100LastProcessMs = TMOS_GetSystemClock();
        PRINT("MAX30100 OK id:0x%02x\n", max30100State.part_id);
    }
    else
    {
        displayHrReady = 0;
        displayHeartRate = 0;
        PRINT("MAX30100 init failed\n");
    }
    tmos_start_task(hidEmuTaskId, TOUCH_UPDATE_EVT, TOUCH_UPDATE_PERIOD);
    tmos_start_task(hidEmuTaskId, HEART_RATE_UPDATE_EVT, HEART_RATE_UPDATE_PERIOD);
    tmos_start_task(hidEmuTaskId, DISPLAY_UPDATE_EVT, DISPLAY_UPDATE_PERIOD);
}

/*********************************************************************
 * @fn      HidEmu_ProcessEvent
 *
 * @brief   HidEmuKbd Application Task event processor.  This function
 *          is called to process all events for the task.  Events
 *          include timers, messages and any other user defined events.
 *
 * @param   task_id  - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
uint16_t HidEmu_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if(events & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;

        if((pMsg = tmos_msg_receive(hidEmuTaskId)) != NULL)
        {
            hidEmu_ProcessTMOSMsg((tmos_event_hdr_t *)pMsg);

            // Release the TMOS message
            tmos_msg_deallocate(pMsg);
        }

        // return unprocessed events
        return (events ^ SYS_EVENT_MSG);
    }

    if(events & START_DEVICE_EVT)
    {
        uint8_t first_addr;

        if(BSP_BMI270_Init(&bmi270Dev) == 0)
        {
            PRINT("BMI270 OK addr:0x%02x chip:0x%02x status:0x%02x\n",
                  bmi270Dev.addr7, bmi270Dev.chip_id, bmi270Dev.internal_status);
            PRINT("Motion HID build:%s\n", BMI270_MOUSE_BUILD_TAG);
            bmi270Ready = 1;
            tmos_start_task(hidEmuTaskId, BMI270_TEST_EVT, BMI270_MOUSE_REPORT_PERIOD);
        }
        else
        {
            PRINT("BMI270 init failed addr:0x%02x chip:0x%02x status:0x%02x\n",
                  bmi270Dev.addr7, bmi270Dev.chip_id, bmi270Dev.internal_status);
            if(BSP_I2C_ScanFirst(&first_addr) == 0)
            {
                PRINT("I2C first device addr:0x%02x\n", first_addr);
            }
            else
            {
                PRINT("I2C scan no device\n");
            }
        }
        return (events ^ START_DEVICE_EVT);
    }

    if(events & START_PARAM_UPDATE_EVT)
    {
        // Send connect param update request
        GAPRole_PeripheralConnParamUpdateReq(hidEmuConnHandle,
                                             DEFAULT_DESIRED_MIN_CONN_INTERVAL,
                                             DEFAULT_DESIRED_MAX_CONN_INTERVAL,
                                             DEFAULT_DESIRED_SLAVE_LATENCY,
                                             DEFAULT_DESIRED_CONN_TIMEOUT,
                                             hidEmuTaskId);

        return (events ^ START_PARAM_UPDATE_EVT);
    }

    if(events & START_PHY_UPDATE_EVT)
    {
        // start phy update
        PRINT("Send Phy Update %x...\n", GAPRole_UpdatePHY(hidEmuConnHandle, 0,
                    GAP_PHY_BIT_LE_2M, GAP_PHY_BIT_LE_2M, 0));

        return (events ^ START_PHY_UPDATE_EVT);
    }

    if(events & START_REPORT_EVT)
    {
        if(bmi270Ready != 0)
        {
            tmos_start_task(hidEmuTaskId, BMI270_TEST_EVT, BMI270_MOUSE_REPORT_PERIOD);
        }
        return (events ^ START_REPORT_EVT);
    }

    if(events & TOUCH_UPDATE_EVT)
    {
        hidEmuProcessTouchPageSwitch();
        tmos_start_task(hidEmuTaskId, TOUCH_UPDATE_EVT, TOUCH_UPDATE_PERIOD);
        return (events ^ TOUCH_UPDATE_EVT);
    }

    if(events & BMI270_TEST_EVT)
    {
        hidEmuProcessBmi270Mouse();
        tmos_start_task(hidEmuTaskId, BMI270_TEST_EVT, BMI270_MOUSE_REPORT_PERIOD);
        return (events ^ BMI270_TEST_EVT);
    }

    if(events & HEART_RATE_UPDATE_EVT)
    {
        hidEmuProcessHeartRate();
        tmos_start_task(hidEmuTaskId, HEART_RATE_UPDATE_EVT, HEART_RATE_UPDATE_PERIOD);
        return (events ^ HEART_RATE_UPDATE_EVT);
    }

    if(events & DISPLAY_UPDATE_EVT)
    {
        hidEmuProcessMotorAlert();
        hidEmuClockUpdate();
        hidEmuUpdateDisplay();
        tmos_start_task(hidEmuTaskId, DISPLAY_UPDATE_EVT, DISPLAY_UPDATE_PERIOD);
        return (events ^ DISPLAY_UPDATE_EVT);
    }
    return 0;
}

/*********************************************************************
 * @fn      hidEmu_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  none
 */
static void hidEmuMouseKeysInit(void)
{
    GPIOA_ModeCfg(HID_MOUSE_LEFT_KEY_PIN | HID_MOUSE_RIGHT_KEY_PIN, GPIO_ModeIN_PU);
}

static uint8_t hidEmuMouseKeysScan(void)
{
    uint8_t sample = MOUSE_BUTTON_NONE;
    uint32_t raw_pin;

    raw_pin = GPIOA_ReadPortPin(HID_MOUSE_LEFT_KEY_PIN | HID_MOUSE_RIGHT_KEY_PIN);
    if((raw_pin & HID_MOUSE_LEFT_KEY_PIN) == 0)
    {
        sample |= MOUSE_BUTTON_LEFT;
    }
    if((raw_pin & HID_MOUSE_RIGHT_KEY_PIN) == 0)
    {
        sample |= MOUSE_BUTTON_RIGHT;
    }

    if(sample != hidMouseButtonStable)
    {
        hidMouseButtonSample = sample;
        hidMouseButtonStable = sample;
        hidMouseButtonChanged = 1;
        PRINT("HID key raw:0x%04x buttons:0x%02x\n", (uint16_t)raw_pin, hidMouseButtonStable);
    }
    else if(sample == hidMouseButtonSample)
    {
        if(hidMouseButtonDebounce < HID_MOUSE_KEY_DEBOUNCE_TICKS)
        {
            hidMouseButtonDebounce++;
        }
    }
    else
    {
        hidMouseButtonSample = sample;
        hidMouseButtonDebounce = 0;
    }

    return hidMouseButtonStable;
}

static int16_t hidEmuApplyDeadzone(int16_t value, int16_t deadzone)
{
    if((value > -deadzone) && (value < deadzone))
    {
        return 0;
    }
    return value;
}

static int8_t hidEmuClampMouseStep(int16_t value)
{
    if(value > BMI270_MOUSE_MAX_STEP)
    {
        return BMI270_MOUSE_MAX_STEP;
    }
    if(value < -BMI270_MOUSE_MAX_STEP)
    {
        return -BMI270_MOUSE_MAX_STEP;
    }
    return (int8_t)value;
}

static void hidEmuRecoverI2c(uint8_t restore_touch)
{
    i2cLowPriorityCooldown = I2C_LOW_PRIORITY_COOLDOWN_TICKS;
    BSP_I2C_Bus_Recover();

    if(restore_touch != 0)
    {
        touchReady = 0;
        touchLastPressed = 0;
        touchPressTicks = 0;
        touchLongHandled = 0;
        touchReadFailCount = 0;
        touchInitRetryTicks = 0;
        hidEmuTouchInit();
    }
}

static void hidEmuProcessBmi270Mouse(void)
{
    BSP_BMI270_RawData_t raw;
    int16_t tilt_x;
    int16_t tilt_y;
    int16_t gain_div;
    int8_t mouse_x;
    int8_t mouse_y;

    hidMouseButtons = hidEmuMouseKeysScan();
    if(hidMouseButtonChanged != 0)
    {
        if(presenterMode != 0)
        {
            if((hidMouseButtons & MOUSE_BUTTON_LEFT) != 0)
            {
                hidMouseLastReportStatus = hidEmuSendMouseWheelReport(1);
                presenterLastAction = -1;
            }
            else if((hidMouseButtons & MOUSE_BUTTON_RIGHT) != 0)
            {
                hidMouseLastReportStatus = hidEmuSendMouseWheelReport(-1);
                presenterLastAction = 1;
            }
        }
        else
        {
            hidMouseLastReportStatus = hidEmuSendMouseReport(hidMouseButtons, 0, 0);
        }
        hidMouseReportSendCount++;
        displayMouseX = 0;
        displayMouseY = 0;
        hidMouseButtonChanged = 0;
        return;
    }

    if(presenterMode != 0)
    {
        displayMouseX = 0;
        displayMouseY = 0;
        return;
    }

    if((bmi270Ready == 0) || (BSP_BMI270_ReadRawData(&bmi270Dev, &raw) != 0))
    {
        bmi270ReadFailCount++;
        PRINT("BMI270 read raw failed count:%d btn:%02x\n", bmi270ReadFailCount, hidMouseButtons);

        if(bmi270ReadFailCount >= BMI270_READ_FAIL_REINIT_TICKS)
        {
            bmi270ReadFailCount = 0;
            bmi270Ready = 0;
            PRINT("BMI270 recover init...\n");
            hidEmuRecoverI2c(1);
            if(BSP_BMI270_Init(&bmi270Dev) == 0)
            {
                bmi270Ready = 1;
                bmi270FiltTiltX = 0;
                bmi270FiltTiltY = 0;
                PRINT("BMI270 recover OK addr:0x%02x status:0x%02x\n", bmi270Dev.addr7, bmi270Dev.internal_status);
            }
            else
            {
                PRINT("BMI270 recover failed addr:0x%02x status:0x%02x\n", bmi270Dev.addr7, bmi270Dev.internal_status);
            }
        }
        return;
    }
    bmi270ReadFailCount = 0;

    bmi270FiltTiltX = (int16_t)((bmi270FiltTiltX * 3 + raw.acc_x) / 4);
    bmi270FiltTiltY = (int16_t)((bmi270FiltTiltY * 3 + raw.acc_y) / 4);

    tilt_x = hidEmuApplyDeadzone(bmi270FiltTiltX, BMI270_TILT_DEADZONE);
    tilt_y = hidEmuApplyDeadzone(bmi270FiltTiltY, BMI270_TILT_DEADZONE);

    gain_div = (healthSafeMode != 0) ? HEALTH_SAFE_GAIN_DIV : BMI270_TILT_GAIN_DIV;
    mouse_x = hidEmuClampMouseStep((int16_t)(tilt_y / gain_div));
    mouse_y = hidEmuClampMouseStep((int16_t)(tilt_x / gain_div));
    displayMouseX = mouse_x;
    displayMouseY = mouse_y;

    if((mouse_x != 0) || (mouse_y != 0) || (hidMouseButtonChanged != 0))
    {
        hidMouseLastReportStatus = hidEmuSendMouseReport(hidMouseButtons, (uint8_t)mouse_x, (uint8_t)mouse_y);
        hidMouseReportSendCount++;
        hidMouseButtonChanged = 0;
    }

    bmi270LogDivider++;
    if(bmi270LogDivider >= BMI270_LOG_INTERVAL)
    {
        bmi270LogDivider = 0;
        PRINT("BMI270H acc:%d,%d,%d gyro:%d,%d,%d mouse:%d,%d btn:%02x hid:%d st:%x cnt:%d\n",
              raw.acc_x, raw.acc_y, raw.acc_z,
              raw.gyro_x, raw.gyro_y, raw.gyro_z, mouse_x, mouse_y, hidMouseButtons,
              hidMouseReportEnabled, hidMouseLastReportStatus, hidMouseReportSendCount);
    }
}

static void hidEmuProcessHeartRate(void)
{
    uint8_t i;
    uint32_t now_ms;
    uint32_t elapsed_ms;
    uint32_t sample_ms;

    if(i2cLowPriorityCooldown != 0)
    {
        i2cLowPriorityCooldown--;
        return;
    }

    if(displayHrReady == 0)
    {
        if(max30100RetryTicks < MAX30100_RETRY_TICKS)
        {
            max30100RetryTicks++;
            return;
        }
        max30100RetryTicks = 0;

        hidEmuRecoverI2c(1);
        if(BSP_MAX30100_Init(&max30100State) == BSP_MAX30100_OK)
        {
            displayHrReady = 1;
            max30100ReadFailCount = 0;
            max30100LastProcessMs = TMOS_GetSystemClock();
            PRINT("MAX30100 recover OK id:0x%02x\n", max30100State.part_id);
        }
        return;
    }

    now_ms = TMOS_GetSystemClock();
    elapsed_ms = now_ms - max30100LastProcessMs;
    if(elapsed_ms == 0)
    {
        elapsed_ms = DISPLAY_UPDATE_PERIOD;
    }

    for(i = 0; i < MAX30100_PROCESS_SAMPLES; i++)
    {
        sample_ms = max30100LastProcessMs + ((elapsed_ms * (i + 1U)) / MAX30100_PROCESS_SAMPLES);
        if(BSP_MAX30100_Process(&max30100State, sample_ms) != BSP_MAX30100_OK)
        {
            max30100ReadFailCount++;
            PRINT("MAX30100 read failed count:%d\n", max30100ReadFailCount);
            if(max30100ReadFailCount >= MAX30100_READ_FAIL_REINIT_TICKS)
            {
                max30100ReadFailCount = 0;
                displayHeartRate = 0;
                PRINT("MAX30100 recover init...\n");
                hidEmuRecoverI2c(1);
                if(BSP_MAX30100_Init(&max30100State) == BSP_MAX30100_OK)
                {
                    displayHrReady = 1;
                    max30100LastProcessMs = TMOS_GetSystemClock();
                    PRINT("MAX30100 recover OK id:0x%02x\n", max30100State.part_id);
                }
                else
                {
                    displayHrReady = 0;
                    max30100RetryTicks = 0;
                    PRINT("MAX30100 recover failed\n");
                }
            }
            return;
        }

        if(max30100State.beat_detected != 0)
        {
            PRINT("MAX30100 beat hr:%d ir:%d red:%d\n",
                  max30100State.heart_rate, max30100State.last_ir, max30100State.last_red);
        }
    }

    max30100ReadFailCount = 0;
    max30100LastProcessMs = now_ms;
    displayHeartRate = max30100State.heart_rate;
}

static void hidEmuProcessMotorAlert(void)
{
    if(displayHeartRate >= HEART_RATE_ALERT_THRESHOLD)
    {
        heartRateAlertActive = 1;
    }
    else if((displayHeartRate == 0) || (displayHeartRate <= HEART_RATE_ALERT_CLEAR))
    {
        heartRateAlertActive = 0;
    }

    if(healthSafeMode != heartRateAlertActive)
    {
        healthSafeMode = heartRateAlertActive;
        displayPageRedraw = 1;
        timePageLine = 0;
        displayHrLine = 0;
        PRINT("Health safe mode %s hr:%d\n", healthSafeMode ? "ON" : "OFF", displayHeartRate);
    }

    if(heartRateAlertActive == 0)
    {
        motorAlertOn = 0;
        motorAlertTicks = 0;
        BSP_Motor_Off();
        return;
    }

    motorAlertTicks++;
    if(motorAlertOn == 0)
    {
        if(motorAlertTicks >= MOTOR_ALERT_OFF_TICKS)
        {
            motorAlertTicks = 0;
            motorAlertOn = 1;
            BSP_Motor_On();
        }
    }
    else if(motorAlertTicks >= MOTOR_ALERT_ON_TICKS)
    {
        motorAlertTicks = 0;
        motorAlertOn = 0;
        BSP_Motor_Off();
    }
}

static void hidEmuTouchInit(void)
{
    uint8_t chip_id = 0;

    if(BSP_TouchPad_Init() == 0)
    {
        touchReady = 1;
        touchReadFailCount = 0;
        touchInitRetryTicks = 0;
        if(BSP_TouchPad_ReadChipId(&chip_id) == 0)
        {
            PRINT("TouchPad OK id:0x%02x\n", chip_id);
        }
        else
        {
            PRINT("TouchPad OK\n");
        }
    }
    else
    {
        touchReady = 0;
        touchReadFailCount = 0;
        touchInitRetryTicks = 0;
        PRINT("TouchPad init failed\n");
    }
}

static void hidEmuSwitchDisplayPage(uint8_t page)
{
    if(displayPage == page)
    {
        return;
    }

    displayPage = page;
    displayPageRedraw = 1;
    displayHrLine = 0;
    timePageLine = 0;
}

static void hidEmuTogglePresenterMode(void)
{
    presenterMode = (presenterMode == 0) ? 1U : 0U;
    presenterLastAction = 0;
    displayMouseX = 0;
    displayMouseY = 0;
    displayPageRedraw = 1;
    displayHrLine = 0;
    PRINT("Presenter mode:%s\n", presenterMode ? "ON" : "OFF");
}

static void hidEmuProcessTouchPageSwitch(void)
{
    BSP_TouchPad_Point_t point;
    uint8_t pressed;

    if(touchReady == 0)
    {
        if(touchInitRetryTicks < TOUCH_INIT_RETRY_TICKS)
        {
            touchInitRetryTicks++;
            return;
        }
        touchInitRetryTicks = 0;
        PRINT("TouchPad retry init\n");
        BSP_I2C_Bus_Recover();
        hidEmuTouchInit();
        return;
    }

    if(touchSwitchCooldown != 0)
    {
        touchSwitchCooldown--;
    }

    if(BSP_TouchPad_ReadPoint(&point) != 0)
    {
        touchReadFailCount++;
        if(touchReadFailCount >= TOUCH_READ_FAIL_REINIT_TICKS)
        {
            PRINT("TouchPad read recover\n");
            touchReadFailCount = 0;
            touchReady = 0;
            touchLastPressed = 0;
            touchPressTicks = 0;
            touchLongHandled = 0;
            hidEmuRecoverI2c(1);
        }
        return;
    }

    touchReadFailCount = 0;
    pressed = (point.touched != 0U) ? 1U : 0U;
    if(pressed != 0U)
    {
        if(touchPressTicks < 0xFFU)
        {
            touchPressTicks++;
        }
        if((touchPressTicks >= TOUCH_LONG_PRESS_TICKS) && (touchLongHandled == 0U))
        {
            hidEmuTogglePresenterMode();
            touchLongHandled = 1;
            touchSwitchCooldown = TOUCH_PAGE_SWITCH_COOLDOWN_TICKS;
        }
    }
    else
    {
        if((touchLastPressed != 0U) && (touchLongHandled == 0U) && (touchSwitchCooldown == 0U))
        {
            if(displayPage == DISPLAY_PAGE_TIME)
            {
                hidEmuSwitchDisplayPage(DISPLAY_PAGE_STATUS);
                PRINT("Display page:status touch x:%d y:%d g:%02x\n", point.x, point.y, point.gesture);
            }
            else
            {
                hidEmuSwitchDisplayPage(DISPLAY_PAGE_TIME);
                PRINT("Display page:time touch x:%d y:%d g:%02x\n", point.x, point.y, point.gesture);
            }
            touchSwitchCooldown = TOUCH_PAGE_SWITCH_COOLDOWN_TICKS;
        }
        touchPressTicks = 0;
        touchLongHandled = 0;
    }

    touchLastPressed = pressed;
}

static uint8_t hidEmuClockMonthFromBuild(const char *month)
{
    static const char names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    uint8_t i;

    for(i = 0; i < 12U; i++)
    {
        const char *name = &names[i * 3U];
        if((month[0] == name[0]) && (month[1] == name[1]) && (month[2] == name[2]))
        {
            return (uint8_t)(i + 1U);
        }
    }
    return 1U;
}

static uint8_t hidEmuClockIsLeapYear(uint16_t year)
{
    if((year % 400U) == 0U)
    {
        return 1U;
    }
    if((year % 100U) == 0U)
    {
        return 0U;
    }
    return ((year % 4U) == 0U) ? 1U : 0U;
}

static uint8_t hidEmuClockDaysInMonth(uint16_t year, uint8_t month)
{
    static const uint8_t days[] = {31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    if((month == 2U) && (hidEmuClockIsLeapYear(year) != 0U))
    {
        return 29U;
    }
    if((month == 0U) || (month > 12U))
    {
        return 31U;
    }
    return days[month - 1U];
}

static void hidEmuClockInit(void)
{
    const char *date = __DATE__;
    const char *time = __TIME__;

    clockMonth = hidEmuClockMonthFromBuild(date);
    clockDay = (uint8_t)(((date[4] == ' ') ? 0U : (uint8_t)(date[4] - '0') * 10U) + (uint8_t)(date[5] - '0'));
    clockYear = (uint16_t)(((uint16_t)(date[7] - '0') * 1000U) +
                           ((uint16_t)(date[8] - '0') * 100U) +
                           ((uint16_t)(date[9] - '0') * 10U) +
                           (uint16_t)(date[10] - '0'));
    clockHour = (uint8_t)(((uint8_t)(time[0] - '0') * 10U) + (uint8_t)(time[1] - '0'));
    clockMinute = (uint8_t)(((uint8_t)(time[3] - '0') * 10U) + (uint8_t)(time[4] - '0'));
    clockSecond = (uint8_t)(((uint8_t)(time[6] - '0') * 10U) + (uint8_t)(time[7] - '0'));
    clockLastMs = TMOS_GetSystemClock();
}

static void hidEmuClockTickSecond(void)
{
    clockSecond++;
    if(clockSecond < 60U)
    {
        return;
    }

    clockSecond = 0;
    clockMinute++;
    if(clockMinute < 60U)
    {
        return;
    }

    clockMinute = 0;
    clockHour++;
    if(clockHour < 24U)
    {
        return;
    }

    clockHour = 0;
    clockDay++;
    if(clockDay <= hidEmuClockDaysInMonth(clockYear, clockMonth))
    {
        return;
    }

    clockDay = 1;
    clockMonth++;
    if(clockMonth <= 12U)
    {
        return;
    }

    clockMonth = 1;
    clockYear++;
}

static void hidEmuClockUpdate(void)
{
    uint32_t now_ms = TMOS_GetSystemClock();

    while((uint32_t)(now_ms - clockLastMs) >= 1000U)
    {
        clockLastMs += 1000U;
        hidEmuClockTickSecond();
    }
}

static void hidEmuDrawHealthAlertFrame(void)
{
    char value[24];

    BSP_Disp_FillColor(BSP_DISP_COLOR_RED);
    BSP_Disp_FillRect(0, 0, BSP_DISP_WIDTH, 42, BSP_DISP_COLOR_BLACK);
    BSP_Disp_DrawString(18, 12, "HEART ALERT", BSP_DISP_COLOR_RED, BSP_DISP_COLOR_BLACK, 2);
    BSP_Disp_DrawString(24, 70, "HIGH HR", BSP_DISP_COLOR_WHITE, BSP_DISP_COLOR_RED, 3);
    sprintf(value, "%u BPM", displayHeartRate);
    BSP_Disp_DrawString(24, 122, value, BSP_DISP_COLOR_YELLOW, BSP_DISP_COLOR_RED, 4);
    BSP_Disp_DrawString(24, 188, "SAFE MODE", BSP_DISP_COLOR_WHITE, BSP_DISP_COLOR_RED, 2);
    BSP_Disp_DrawString(24, 222, "MOUSE SLOW", BSP_DISP_COLOR_WHITE, BSP_DISP_COLOR_RED, 2);
}

static void hidEmuUpdateHealthAlertPage(void)
{
    char value[24];

    BSP_Disp_FillRect(24, 122, 190, 32, BSP_DISP_COLOR_RED);
    sprintf(value, "%u BPM", displayHeartRate);
    BSP_Disp_DrawString(24, 122, value, BSP_DISP_COLOR_YELLOW, BSP_DISP_COLOR_RED, 4);
}

static void hidEmuDrawTimeFrame(void)
{
    BSP_Disp_FillColor(BSP_DISP_COLOR_BLACK);
    BSP_Disp_FillRect(0, 0, BSP_DISP_WIDTH, 34, BSP_DISP_COLOR_DARK);
    BSP_Disp_FillRect(0, 34, BSP_DISP_WIDTH, 2, BSP_DISP_COLOR_CYAN);
    BSP_Disp_DrawString(14, 10, "TIME", BSP_DISP_COLOR_CYAN, BSP_DISP_COLOR_DARK, 2);
    BSP_Disp_DrawString(14, 54, "DATE", BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 2);
    BSP_Disp_DrawString(14, 104, "CLOCK", BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 2);
    BSP_Disp_DrawString(14, 164, "HR", BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 2);
    BSP_Disp_DrawString(14, 224, "AUTO BUILD TIME", BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 1);
}

static void hidEmuUpdateTimePage(void)
{
    char value[24];

    switch(timePageLine)
    {
        case 0:
            sprintf(value, "%04u-%02u-%02u", clockYear, clockMonth, clockDay);
            BSP_Disp_FillRect(14, 76, 210, 24, BSP_DISP_COLOR_BLACK);
            BSP_Disp_DrawString(14, 76, value, BSP_DISP_COLOR_WHITE, BSP_DISP_COLOR_BLACK, 3);
            break;
        case 1:
            sprintf(value, "%02u:%02u:%02u", clockHour, clockMinute, clockSecond);
            BSP_Disp_FillRect(14, 126, 210, 32, BSP_DISP_COLOR_BLACK);
            BSP_Disp_DrawString(14, 126, value, BSP_DISP_COLOR_GREEN, BSP_DISP_COLOR_BLACK, 4);
            break;
        default:
            BSP_Disp_FillRect(76, 164, 154, 24, BSP_DISP_COLOR_BLACK);
            if(displayHrReady == 0)
            {
                BSP_Disp_DrawString(76, 164, "NO DEV", BSP_DISP_COLOR_RED, BSP_DISP_COLOR_BLACK, 2);
            }
            else if(displayHeartRate == 0)
            {
                BSP_Disp_DrawString(76, 164, "-- BPM", BSP_DISP_COLOR_GRAY, BSP_DISP_COLOR_BLACK, 2);
            }
            else if(heartRateAlertActive != 0)
            {
                sprintf(value, "%u HIGH", displayHeartRate);
                BSP_Disp_DrawString(76, 164, value, BSP_DISP_COLOR_RED, BSP_DISP_COLOR_BLACK, 2);
            }
            else
            {
                sprintf(value, "%u BPM", displayHeartRate);
                BSP_Disp_DrawString(76, 164, value, BSP_DISP_COLOR_MAGENTA, BSP_DISP_COLOR_BLACK, 2);
            }
            break;
    }

    timePageLine++;
    if(timePageLine >= 3U)
    {
        timePageLine = 0;
    }
}

static void hidEmuUpdateStatusPage(void)
{
    char value[20];

    switch(displayHrLine)
    {
        case 0:
            BSP_Disp_DrawStatusLine(0, "BLE", displayBleConnected ? "CONN" : "ADV",
                                    displayBleConnected ? BSP_DISP_COLOR_GREEN : BSP_DISP_COLOR_YELLOW);
            break;
        case 1:
            BSP_Disp_DrawStatusLine(1, "HID", hidMouseReportEnabled ? "READY" : "WAIT",
                                    hidMouseReportEnabled ? BSP_DISP_COLOR_GREEN : BSP_DISP_COLOR_YELLOW);
            break;
        case 2:
            BSP_Disp_DrawStatusLine(2, "IMU", bmi270Ready ? "OK" : "ERR",
                                    bmi270Ready ? BSP_DISP_COLOR_GREEN : BSP_DISP_COLOR_RED);
            break;
        case 3:
            if(hidMouseButtons == MOUSE_BUTTON_LEFT)
            {
                BSP_Disp_DrawStatusLine(3, "BTN", "LEFT", BSP_DISP_COLOR_CYAN);
            }
            else if(hidMouseButtons == MOUSE_BUTTON_RIGHT)
            {
                BSP_Disp_DrawStatusLine(3, "BTN", "RIGHT", BSP_DISP_COLOR_CYAN);
            }
            else if(hidMouseButtons == (MOUSE_BUTTON_LEFT | MOUSE_BUTTON_RIGHT))
            {
                BSP_Disp_DrawStatusLine(3, "BTN", "LR", BSP_DISP_COLOR_CYAN);
            }
            else
            {
                BSP_Disp_DrawStatusLine(3, "BTN", "NONE", BSP_DISP_COLOR_GRAY);
            }
            break;
        case 4:
            if(presenterMode != 0)
            {
                if(presenterLastAction > 0)
                {
                    BSP_Disp_DrawStatusLine(4, "MODE", "PRES NEXT", BSP_DISP_COLOR_YELLOW);
                }
                else if(presenterLastAction < 0)
                {
                    BSP_Disp_DrawStatusLine(4, "MODE", "PRES PREV", BSP_DISP_COLOR_YELLOW);
                }
                else
                {
                    BSP_Disp_DrawStatusLine(4, "MODE", "PRES", BSP_DISP_COLOR_YELLOW);
                }
            }
            else
            {
                sprintf(value, "X%d Y%d", displayMouseX, displayMouseY);
                BSP_Disp_DrawStatusLine(4, "AIR", value, BSP_DISP_COLOR_WHITE);
            }
            break;
        default:
            if(displayHrReady == 0)
            {
                BSP_Disp_DrawStatusLine(5, "HR", "NO DEV", BSP_DISP_COLOR_RED);
            }
            else if(displayHeartRate == 0)
            {
                BSP_Disp_DrawStatusLine(5, "HR", "-- BPM", BSP_DISP_COLOR_GRAY);
            }
            else if(heartRateAlertActive != 0)
            {
                sprintf(value, "%d HIGH", displayHeartRate);
                BSP_Disp_DrawStatusLine(5, "HR", value, BSP_DISP_COLOR_RED);
            }
            else
            {
                sprintf(value, "%d BPM", displayHeartRate);
                BSP_Disp_DrawStatusLine(5, "HR", value, BSP_DISP_COLOR_MAGENTA);
            }
            break;
    }

    displayHrLine++;
    if(displayHrLine >= 6)
    {
        displayHrLine = 0;
    }
}

static void hidEmuUpdateDisplayPage(void)
{
    if(displayPageRedraw == 0)
    {
        return;
    }

    displayPageRedraw = 0;
    if(healthSafeMode != 0)
    {
        hidEmuDrawHealthAlertFrame();
        return;
    }

    if(displayPage == DISPLAY_PAGE_STATUS)
    {
        BSP_Disp_DrawStatusFrame();
    }
    else
    {
        hidEmuDrawTimeFrame();
    }
}
static void hidEmuUpdateDisplay(void)
{
    hidEmuUpdateDisplayPage();
    if(healthSafeMode != 0)
    {
        hidEmuUpdateHealthAlertPage();
        return;
    }

    if(displayPage == DISPLAY_PAGE_STATUS)
    {
        hidEmuUpdateStatusPage();
    }
    else
    {
        hidEmuUpdateTimePage();
    }
}
static void hidEmu_ProcessTMOSMsg(tmos_event_hdr_t *pMsg)
{
    switch(pMsg->event)
    {
        default:
            break;
    }
}

/*********************************************************************
 * @fn      hidEmuSendMouseReport
 *
 * @brief   Build and send a HID mouse report.
 *
 * @param   buttons - Mouse button code
 *					X_data - X axis move data
 *					Y_data - Y axis move data
 *
 * @return  none
 */
static uint8_t hidEmuSendMouseReport(uint8_t buttons, uint8_t X_data, uint8_t Y_data)
{
    uint8_t buf[HID_MOUSE_IN_RPT_LEN];

    buf[0] = buttons; // Buttons
    buf[1] = X_data;  // X
    buf[2] = Y_data;  // Y
    buf[3] = 0;       // Wheel

    return HidDev_Report(HID_RPT_ID_MOUSE_IN, HID_REPORT_TYPE_INPUT,
                  HID_MOUSE_IN_RPT_LEN, buf);
}

static uint8_t hidEmuSendMouseWheelReport(int8_t wheel)
{
    uint8_t buf[HID_MOUSE_IN_RPT_LEN];

    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = (uint8_t)wheel;

    return HidDev_Report(HID_RPT_ID_MOUSE_IN, HID_REPORT_TYPE_INPUT,
                  HID_MOUSE_IN_RPT_LEN, buf);
}

/*********************************************************************
 * @fn      hidEmuStateCB
 *
 * @brief   GAP state change callback.
 *
 * @param   newState - new state
 *
 * @return  none
 */
static void hidEmuStateCB(gapRole_States_t newState, gapRoleEvent_t *pEvent)
{
    switch(newState & GAPROLE_STATE_ADV_MASK)
    {
        case GAPROLE_STARTED:
        {
            uint8_t ownAddr[6];
            GAPRole_GetParameter(GAPROLE_BD_ADDR, ownAddr);
            GAP_ConfigDeviceAddr(ADDRTYPE_STATIC, ownAddr);
            PRINT("Initialized..\n");
        }
        break;

        case GAPROLE_ADVERTISING:
            if(pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT)
            {
                PRINT("Advertising..\n");
            }
            break;

        case GAPROLE_CONNECTED:
            if(pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT)
            {
                gapEstLinkReqEvent_t *event = (gapEstLinkReqEvent_t *)pEvent;

                // get connection handle
                hidEmuConnHandle = event->connectionHandle;
                tmos_start_task(hidEmuTaskId, START_PARAM_UPDATE_EVT, START_PARAM_UPDATE_EVT_DELAY);
                hidMouseReportEnabled = 0;
                displayBleConnected = 1;
                PRINT("Connected..\n");
            }
            break;

        case GAPROLE_CONNECTED_ADV:
            if(pEvent->gap.opcode == GAP_MAKE_DISCOVERABLE_DONE_EVENT)
            {
                PRINT("Connected Advertising..\n");
            }
            break;

        case GAPROLE_WAITING:
            if(pEvent->gap.opcode == GAP_END_DISCOVERABLE_DONE_EVENT)
            {
                PRINT("Waiting for advertising..\n");
            }
            else if(pEvent->gap.opcode == GAP_LINK_TERMINATED_EVENT)
            {
                PRINT("Disconnected.. Reason:%x\n", pEvent->linkTerminate.reason);
                hidMouseReportEnabled = 0;
                displayBleConnected = 0;
            }
            else if(pEvent->gap.opcode == GAP_LINK_ESTABLISHED_EVENT)
            {
                PRINT("Advertising timeout..\n");
            }
            // Enable advertising
            {
                uint8_t initial_advertising_enable = TRUE;
                // Set the GAP Role Parameters
                GAPRole_SetParameter(GAPROLE_ADVERT_ENABLED, sizeof(uint8_t), &initial_advertising_enable);
            }
            break;

        case GAPROLE_ERROR:
            PRINT("Error %x ..\n", pEvent->gap.opcode);
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      hidEmuRptCB
 *
 * @brief   HID Dev report callback.
 *
 * @param   id - HID report ID.
 * @param   type - HID report type.
 * @param   uuid - attribute uuid.
 * @param   oper - operation:  read, write, etc.
 * @param   len - Length of report.
 * @param   pData - Report data.
 *
 * @return  GATT status code.
 */
static uint8_t hidEmuRptCB(uint8_t id, uint8_t type, uint16_t uuid,
                           uint8_t oper, uint16_t *pLen, uint8_t *pData)
{
    uint8_t status = SUCCESS;

    // write
    if(oper == HID_DEV_OPER_WRITE)
    {
        status = Hid_SetParameter(id, type, uuid, *pLen, pData);
    }
    // read
    else if(oper == HID_DEV_OPER_READ)
    {
        status = Hid_GetParameter(id, type, uuid, pLen, pData);
    }
    // notifications enabled
    else if(oper == HID_DEV_OPER_ENABLE)
    {
        hidMouseReportEnabled = 1;
        PRINT("HID notify enable id:%d type:%d\n", id, type);
        tmos_start_task(hidEmuTaskId, START_REPORT_EVT, 500);
    }
    else if(oper == HID_DEV_OPER_DISABLE)
    {
        hidMouseReportEnabled = 0;
        PRINT("HID notify disable id:%d type:%d\n", id, type);
    }
    return status;
}

/*********************************************************************
 * @fn      hidEmuEvtCB
 *
 * @brief   HID Dev event callback.
 *
 * @param   evt - event ID.
 *
 * @return  HID response code.
 */
static void hidEmuEvtCB(uint8_t evt)
{
    // process enter/exit suspend or enter/exit boot mode
    return;
}

/*********************************************************************
*********************************************************************/
