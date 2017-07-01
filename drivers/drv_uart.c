#include <rtthread.h>
#include <rtdevice.h>

#include "board.h"

#include "esp_types.h"
#include "esp_attr.h"
#include "esp_intr.h"
#include "esp_log.h"

#include "rom/uart.h"
#include "soc/uart_reg.h"
#include "soc/dport_reg.h"
#include "soc/uart_struct.h"

#include "drv_uart.h"

#define UART_FULL_THRESH_DEFAULT  (120)
#define UART_TOUT_THRESH_DEFAULT   (10)

#define UART_FIFO_LEN           (128)        /*!< Length of the hardware FIFO buffers */
#define UART_INTR_MASK          0x1ff        /*!< mask of all UART interrupts */
#define UART_LINE_INV_MASK      (0x3f << 19) /*!< TBD */
#define UART_BITRATE_MAX        5000000      /*!< Max bit rate supported by UART */
#define UART_PIN_NO_CHANGE      (-1)         /*!< Constant for uart_set_pin function which indicates that UART pin should not be changed */

#define UART_INVERSE_DISABLE  (0x0)            /*!< Disable UART signal inverse*/
#define UART_INVERSE_RXD   (UART_RXD_INV_M)    /*!< UART RXD input inverse*/
#define UART_INVERSE_CTS   (UART_CTS_INV_M)    /*!< UART CTS input inverse*/
#define UART_INVERSE_TXD   (UART_TXD_INV_M)    /*!< UART TXD output inverse*/
#define UART_INVERSE_RTS   (UART_RTS_INV_M)    /*!< UART RTS output inverse*/

/**
 * @brief UART word length constants
 */
typedef enum {
    UART_DATA_5_BITS = 0x0,    /*!< word length: 5bits*/
    UART_DATA_6_BITS = 0x1,    /*!< word length: 6bits*/
    UART_DATA_7_BITS = 0x2,    /*!< word length: 7bits*/
    UART_DATA_8_BITS = 0x3,    /*!< word length: 8bits*/
    UART_DATA_BITS_MAX = 0X4,
} uart_word_length_t;

/**
 * @brief UART stop bits number
 */
typedef enum {
    UART_STOP_BITS_1   = 0x1,  /*!< stop bit: 1bit*/
    UART_STOP_BITS_1_5 = 0x2,  /*!< stop bit: 1.5bits*/
    UART_STOP_BITS_2   = 0x3,  /*!< stop bit: 2bits*/
    UART_STOP_BITS_MAX = 0x4,
} uart_stop_bits_t;

/**
 * @brief UART peripheral number
 */
typedef enum {
    UART_NUM_0 = 0x0,  /*!< UART base address 0x3ff40000*/
    UART_NUM_1 = 0x1,  /*!< UART base address 0x3ff50000*/
    UART_NUM_2 = 0x2,  /*!< UART base address 0x3ff6E000*/
    UART_NUM_MAX,
} uart_port_t;

/**
 * @brief UART parity constants
 */
typedef enum {
    UART_PARITY_DISABLE = 0x0,   /*!< Disable UART parity*/
    UART_PARITY_EVEN = 0x2,     /*!< Enable UART even parity*/
    UART_PARITY_ODD  = 0x3      /*!< Enable UART odd parity*/
} uart_parity_t;

/**
 * @brief UART hardware flow control modes
 */
typedef enum {
    UART_HW_FLOWCTRL_DISABLE = 0x0,   /*!< disable hardware flow control*/
    UART_HW_FLOWCTRL_RTS     = 0x1,   /*!< enable RX hardware flow control (rts)*/
    UART_HW_FLOWCTRL_CTS     = 0x2,   /*!< enable TX hardware flow control (cts)*/
    UART_HW_FLOWCTRL_CTS_RTS = 0x3,   /*!< enable hardware flow control*/
    UART_HW_FLOWCTRL_MAX     = 0x4,
} uart_hw_flowcontrol_t;

#define UART_INTR_NUM_0           17
#define UART_INTR_NUM_1           18
#define UART_INTR_NUM_2           19
static DRAM_ATTR uart_dev_t* const UART[UART_NUM_MAX] = {&UART0, &UART1, &UART2};

struct esp32_uart
{
    int num;
    int irq;
};

static rt_err_t esp32_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    struct esp32_uart* uart = (struct esp32_uart *)serial->parent.user_data;
    int uart_num = uart->num;

    uint32_t clk_div = (((UART_CLK_FREQ) << 4) / cfg->baud_rate);
    UART[uart_num]->clk_div.div_int = clk_div >> 4;
    UART[uart_num]->clk_div.div_frag = clk_div & 0xf;
    //
    uint32_t databits = UART_DATA_8_BITS;
    switch (cfg->data_bits)
    {
    case DATA_BITS_7:
        databits = UART_DATA_7_BITS;
        break;
    }
    UART[uart_num]->conf0.bit_num = databits;
    //
    uint32_t parity = UART_PARITY_DISABLE;
    switch (cfg->parity)
    {
    case PARITY_EVEN:
        parity = UART_PARITY_EVEN;
        break;
    case PARITY_ODD:
        parity = UART_PARITY_ODD;
        break;
    }
    UART[uart_num]->conf0.parity = parity & 0x1;
    UART[uart_num]->conf0.parity_en = (parity >> 1) & 0x1;
    //
    uint32_t stopbit = UART_STOP_BITS_1;
    switch (cfg->stop_bits)
    {
    case STOP_BITS_2:
        stopbit = UART_STOP_BITS_2;
        break;
    }
    UART[uart_num]->conf0.stop_bit_num = stopbit;
    //
    CLEAR_PERI_REG_MASK(UART_CONF0_REG(uart_num), UART_LINE_INV_MASK);
    SET_PERI_REG_MASK(UART_CONF0_REG(uart_num), UART_INVERSE_DISABLE);
    //
    UART[uart_num]->conf1.rx_flow_thrhd = UART_FULL_THRESH_DEFAULT;
    if(cfg->reserved & UART_HW_FLOWCTRL_RTS)
        UART[uart_num]->conf1.rx_flow_en = 1;
    else
        UART[uart_num]->conf1.rx_flow_en = 0;
    if(cfg->reserved & UART_HW_FLOWCTRL_CTS)
        UART[uart_num]->conf0.tx_flow_en = 1;
    else
        UART[uart_num]->conf0.tx_flow_en = 0;
    //
    UART[uart_num]->conf0.tick_ref_always_on = 1;
    ESP_INTR_DISABLE(uart->irq);
    return RT_EOK;
}

static rt_err_t esp32_control(struct rt_serial_device *serial, int cmd, void *arg)
{
    struct esp32_uart* uart = (struct esp32_uart *)serial->parent.user_data;

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        ESP_INTR_DISABLE(uart->irq);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        ESP_INTR_ENABLE(uart->irq);
        break;
    }

    return RT_EOK;
}

static int esp32_putc(struct rt_serial_device *serial, char c)
{
    struct esp32_uart* uart = (struct esp32_uart *)serial->parent.user_data;
    int uart_num = uart->num;

    while (true) {
        uint8_t tx_fifo_cnt = UART[uart_num]->status.txfifo_cnt;
        if (tx_fifo_cnt < UART_FIFO_LEN)
            break;
    }
    WRITE_PERI_REG(UART_FIFO_AHB_REG(uart_num), c);
    return 1;
}

static int esp32_getc(struct rt_serial_device *serial)
{
    struct esp32_uart* uart = (struct esp32_uart *)serial->parent.user_data;
    int uart_num = uart->num;

    int ch = -1;
    uint8_t rx_fifo_cnt = UART[uart_num]->status.rxfifo_cnt;
    if (rx_fifo_cnt > 0)
        ch = UART[uart_num]->fifo.rw_byte;
    return ch;
}

//internal isr handler for default driver code.
static void IRAM_ATTR uart_rx_intr_handler_default(void *param)
{
    struct rt_serial_device *serial = (struct rt_serial_device *)param;
    struct esp32_uart* uart = (struct esp32_uart *)serial->parent.user_data;
    uint8_t uart_num = uart->num;
    uint32_t uart_intr_status = UART[uart_num]->int_st.val;

    while(uart_intr_status != 0x0)
    {
        if((uart_intr_status & UART_RXFIFO_TOUT_INT_ST_M)
            || (uart_intr_status & UART_RXFIFO_FULL_INT_ST_M)) {
            rt_interrupt_enter();
            rt_hw_serial_isr(serial, RT_SERIAL_EVENT_RX_IND);
            rt_interrupt_leave();
            UART[uart_num]->int_clr.rxfifo_tout = 1;
            UART[uart_num]->int_clr.rxfifo_full = 1;
        } else if(uart_intr_status & UART_RXFIFO_OVF_INT_ST_M) {
            UART[uart_num]->conf0.rxfifo_rst = 1;
            UART[uart_num]->conf0.rxfifo_rst = 0;
            UART[uart_num]->int_clr.rxfifo_ovf = 1;
        } else if(uart_intr_status & UART_BRK_DET_INT_ST_M) {
            UART[uart_num]->int_clr.brk_det = 1;
        } else if(uart_intr_status & UART_PARITY_ERR_INT_ST_M ) {
            UART[uart_num]->int_clr.parity_err = 1;
        } else if(uart_intr_status & UART_FRM_ERR_INT_ST_M) {
            UART[uart_num]->int_clr.frm_err = 1;
        } else {
            /*simply clear all other intr status*/
            UART[uart_num]->int_clr.val = uart_intr_status;
        }
        uart_intr_status = UART[uart_num]->int_st.val;
    }
}

static const struct rt_uart_ops esp32_uart_ops =
{
    esp32_configure,
    esp32_control,
    esp32_putc,
    esp32_getc,
    RT_NULL,
};

#ifdef RT_USING_UART0
static struct rt_serial_device serial0;
static struct esp32_uart uart0 = { UART_NUM_0, UART_INTR_NUM_0 };
#endif

#ifdef RT_USING_UART1
static struct rt_serial_device serial1;
static struct esp32_uart uart1 = { UART_NUM_1, UART_INTR_NUM_1 };
#endif

#ifdef RT_USING_UART2
static struct rt_serial_device serial2;
static struct esp32_uart uart2 = { UART_NUM_2, UART_INTR_NUM_2 };
#endif

int rt_hw_uart_init()
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.bufsz = 1024;

#ifdef RT_USING_UART0
    // wait for fifo empty
    while (UART[uart0.num]->status.txfifo_cnt);
    uart_tx_wait_idle(0);
    //
    if(RTC_GPIO_IS_VALID_GPIO(GPIO_NUM_1)) rtc_gpio_deinit(GPIO_NUM_1);
    if(RTC_GPIO_IS_VALID_GPIO(GPIO_NUM_3)) rtc_gpio_deinit(GPIO_NUM_3);
    if(RTC_GPIO_IS_VALID_GPIO(GPIO_NUM_23)) rtc_gpio_deinit(GPIO_NUM_23);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_1], PIN_FUNC_GPIO);
    gpio_set_pull_mode(GPIO_NUM_1, GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO_NUM_1, GPIO_MODE_OUTPUT);
    gpio_matrix_out(GPIO_NUM_1, U0TXD_OUT_IDX, 0, 0);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_3], PIN_FUNC_GPIO);
    gpio_set_pull_mode(GPIO_NUM_3, GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO_NUM_3, GPIO_MODE_INPUT);
    gpio_matrix_in(GPIO_NUM_3, U0RXD_IN_IDX, 0);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_23], PIN_FUNC_GPIO);
    gpio_set_pull_mode(GPIO_NUM_23, GPIO_PULLUP_ONLY);
    gpio_set_level(GPIO_NUM_23, 1);
    gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);
    gpio_matrix_out(GPIO_NUM_23, SIG_GPIO_OUT_IDX, 0, 0);
    config.reserved = UART_HW_FLOWCTRL_DISABLE;

    intr_matrix_set(xPortGetCoreID(), ETS_UART0_INTR_SOURCE, uart0.irq);
    xt_set_interrupt_handler(uart0.irq, uart_rx_intr_handler_default, &serial0);
    UART[uart0.num]->int_clr.val = UART_INTR_MASK;
    UART[uart0.num]->conf1.rx_tout_thrhd = UART_TOUT_THRESH_DEFAULT;
    UART[uart0.num]->conf1.rx_tout_en = 1;
    UART[uart0.num]->conf1.rxfifo_full_thrhd = UART_FULL_THRESH_DEFAULT;
    UART[uart0.num]->int_ena.val = UART_RXFIFO_FULL_INT_ENA_M
                            | UART_RXFIFO_TOUT_INT_ENA_M
                            | UART_RXFIFO_OVF_INT_ENA_M
                            | UART_FRM_ERR_INT_ENA_M
                            | UART_BRK_DET_INT_ENA_M
                            | UART_PARITY_ERR_INT_ENA_M;
    ESP_INTR_DISABLE(uart0.irq);

    serial0.ops = &esp32_uart_ops;
    serial0.config = config;
    rt_hw_serial_register(&serial0, "uart0",
        RT_DEVICE_FLAG_RDWR|RT_DEVICE_FLAG_INT_RX, &uart0);
#endif
#ifdef RT_USING_UART1
    periph_module_enable(PERIPH_UART1_MODULE);
    UART[uart1.num]->conf0.rxfifo_rst = 1;
    UART[uart1.num]->conf0.rxfifo_rst = 0;
    UART[uart1.num]->conf0.txfifo_rst = 1;
    UART[uart1.num]->conf0.txfifo_rst = 0;

    if(RTC_GPIO_IS_VALID_GPIO(GPIO_NUM_2)) rtc_gpio_deinit(GPIO_NUM_2);
    if(RTC_GPIO_IS_VALID_GPIO(GPIO_NUM_4)) rtc_gpio_deinit(GPIO_NUM_4);
    if(RTC_GPIO_IS_VALID_GPIO(GPIO_NUM_5)) rtc_gpio_deinit(GPIO_NUM_5);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_2], PIN_FUNC_GPIO);
    gpio_set_pull_mode(GPIO_NUM_2, GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    gpio_matrix_out(GPIO_NUM_2, U1TXD_OUT_IDX, 0, 0);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_4], PIN_FUNC_GPIO);
    gpio_set_pull_mode(GPIO_NUM_4, GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);
    gpio_matrix_in(GPIO_NUM_4, U1RXD_IN_IDX, 0);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_5], PIN_FUNC_GPIO);
    gpio_set_pull_mode(GPIO_NUM_5, GPIO_PULLUP_ONLY);
    gpio_set_level(GPIO_NUM_5, 1);
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    gpio_matrix_out(GPIO_NUM_5, SIG_GPIO_OUT_IDX, 0, 0);
    config.reserved = UART_HW_FLOWCTRL_DISABLE;

    intr_matrix_set(xPortGetCoreID(), ETS_UART1_INTR_SOURCE, uart1.irq);
    xt_set_interrupt_handler(uart1.irq, uart_rx_intr_handler_default, &serial1);
    UART[uart1.num]->int_clr.val = UART_INTR_MASK;
    UART[uart1.num]->conf1.rx_tout_thrhd = UART_TOUT_THRESH_DEFAULT;
    UART[uart1.num]->conf1.rx_tout_en = 1;
    UART[uart1.num]->conf1.rxfifo_full_thrhd = UART_FULL_THRESH_DEFAULT;
    UART[uart1.num]->int_ena.val = UART_RXFIFO_FULL_INT_ENA_M
                            | UART_RXFIFO_TOUT_INT_ENA_M
                            | UART_RXFIFO_OVF_INT_ENA_M
                            | UART_FRM_ERR_INT_ENA_M
                            | UART_BRK_DET_INT_ENA_M
                            | UART_PARITY_ERR_INT_ENA_M;
    ESP_INTR_DISABLE(uart1.irq);

    serial1.ops = &esp32_uart_ops;
    serial1.config = config;
    rt_hw_serial_register(&serial1, "uart1",
        RT_DEVICE_FLAG_RDWR|RT_DEVICE_FLAG_INT_RX, &uart1);
#endif
#ifdef RT_USING_UART2
    periph_module_enable(PERIPH_UART2_MODULE);
    UART[uart2.num]->conf0.rxfifo_rst = 1;
    UART[uart2.num]->conf0.rxfifo_rst = 0;
    UART[uart2.num]->conf0.txfifo_rst = 1;
    UART[uart2.num]->conf0.txfifo_rst = 0;

    if(RTC_GPIO_IS_VALID_GPIO(GPIO_NUM_12)) rtc_gpio_deinit(GPIO_NUM_12);
    if(RTC_GPIO_IS_VALID_GPIO(GPIO_NUM_33)) rtc_gpio_deinit(GPIO_NUM_33);
    if(RTC_GPIO_IS_VALID_GPIO(GPIO_NUM_17)) rtc_gpio_deinit(GPIO_NUM_17);
    if(RTC_GPIO_IS_VALID_GPIO(GPIO_NUM_16)) rtc_gpio_deinit(GPIO_NUM_16);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_12], PIN_FUNC_GPIO);
    gpio_set_pull_mode(GPIO_NUM_12, GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO_NUM_12, GPIO_MODE_OUTPUT);
    gpio_matrix_out(GPIO_NUM_12, U2TXD_OUT_IDX, 0, 0);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_33], PIN_FUNC_GPIO);
    gpio_set_pull_mode(GPIO_NUM_33, GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO_NUM_33, GPIO_MODE_INPUT);
    gpio_matrix_in(GPIO_NUM_33, U2RXD_IN_IDX, 0);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_17], PIN_FUNC_GPIO);
    gpio_set_pull_mode(GPIO_NUM_17, GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO_NUM_17, GPIO_MODE_OUTPUT);
    gpio_matrix_out(GPIO_NUM_17, U2RTS_OUT_IDX, 0, 0);
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_NUM_16], PIN_FUNC_GPIO);
    gpio_set_pull_mode(GPIO_NUM_16, GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO_NUM_16, GPIO_MODE_INPUT);
    gpio_matrix_in(GPIO_NUM_16, U2CTS_IN_IDX, 0);
    config.reserved = UART_HW_FLOWCTRL_DISABLE;

    intr_matrix_set(xPortGetCoreID(), ETS_UART2_INTR_SOURCE, uart2.irq);
    xt_set_interrupt_handler(uart2.irq, uart_rx_intr_handler_default, &serial2);
    UART[uart2.num]->int_clr.val = UART_INTR_MASK;
    UART[uart2.num]->conf1.rx_tout_thrhd = UART_TOUT_THRESH_DEFAULT;
    UART[uart2.num]->conf1.rx_tout_en = 1;
    UART[uart2.num]->conf1.rxfifo_full_thrhd = UART_FULL_THRESH_DEFAULT;
    UART[uart2.num]->int_ena.val = UART_RXFIFO_FULL_INT_ENA_M
                            | UART_RXFIFO_TOUT_INT_ENA_M
                            | UART_RXFIFO_OVF_INT_ENA_M
                            | UART_FRM_ERR_INT_ENA_M
                            | UART_BRK_DET_INT_ENA_M
                            | UART_PARITY_ERR_INT_ENA_M;
    ESP_INTR_DISABLE(uart2.irq);

    serial2.ops = &esp32_uart_ops;
    serial2.config = config;
    rt_hw_serial_register(&serial2, "uart2",
        RT_DEVICE_FLAG_RDWR|RT_DEVICE_FLAG_INT_RX, &uart2);
#endif

    return 0;
}
