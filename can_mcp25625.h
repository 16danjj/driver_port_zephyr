#ifndef _MCP25625_H
#define _MCP25625_H

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/can.h>
#include <zephyr/device.h>

#define MCP25625_RX_COUNT            2
#define MCP25625_TX_COUNT            1
#define MCP25625_FRAME_LEN          13

struct mcp25625_tx_cb {
    can_tx_callback_t cb;
    void *cb_arg;
};

struct mcp25625_data {
    struct can_driver_data common;

    /* tx data */
    struct k_sem tx_sem;
    struct mcp25625_tx_cb tx_cb[MCP25625_TX_COUNT];
    uint8_t tx_busy_map;

    /* filter data */
    uint32_t filter_usage;
    can_rx_callback_t rx_cb[CONFIG_CAN_MAX_FILTER];
    void *cb_arg[CONFIG_CAN_MAX_FILTER];
    struct can_filter filter[CONFIG_CAN_MAX_FILTER];

    /* general data */
    struct k_mutex mutex;
    enum can_state old_state;
    uint8_t mcp25625_mode;

    /*interrupt handling thread-data*/
    k_thread_stack_t *int_thread_stack;
    struct k_thread int_thread;
    struct k_sem int_sem;
};

struct mcp25625_config {
    const struct can_driver_config common;

    /* spi configuration */
    struct spi_dt_spec bus;

    /* CAN Timing */
    uint32_t osc_freq;

    void (*irq_config_func)(const struct device *dev);

    /* interrupt handling thread-config */
    int int_thread_priority;
    size_t int_thread_stack_size;
};

/*
 * Startup time of 128 OSC1 clock cycles at 1MHz (minimum clock in frequency)
 * see MCP25625 datasheet section 8.1 Oscillator Start-up Timer
 */
#define MCP25625_OSC_STARTUP_US		128U

/* MCP25625 Opcodes */
#define MCP25625_OPCODE_WRITE            0x02
#define MCP25625_OPCODE_READ             0x03
#define MCP25625_OPCODE_BIT_MODIFY       0x05
#define MCP25625_OPCODE_LOAD_TX_BUFFER   0x40
#define MCP25625_OPCODE_RTS              0x80
#define MCP25625_OPCODE_READ_RX_BUFFER   0x90
#define MCP25625_OPCODE_READ_STATUS      0xA0
#define MCP25625_OPCODE_RESET            0xC0

/* MCP25625 Registers */
#define MCP25625_ADDR_CANSTAT            0x0E
#define MCP25625_ADDR_CANCTRL            0x0F
#define MCP25625_ADDR_TEC                0x1C
#define MCP25625_ADDR_REC                0x1D
#define MCP25625_ADDR_CNF3               0x28
#define MCP25625_ADDR_CNF2               0x29
#define MCP25625_ADDR_CNF1               0x2A
#define MCP25625_ADDR_CANINTE            0x2B
#define MCP25625_ADDR_CANINTF            0x2C
#define MCP25625_ADDR_EFLG               0x2D
#define MCP25625_ADDR_TXB0CTRL           0x30
#define MCP25625_ADDR_TXB1CTRL           0x40
#define MCP25625_ADDR_TXB2CTRL           0x50
#define MCP25625_ADDR_RXB0CTRL           0x60
#define MCP25625_ADDR_RXB1CTRL           0x70
#define MCP25625_ADDR_BFPCTRL            0x0C

#define MCP25625_ADDR_OFFSET_FRAME2FRAME	0x10
#define MCP25625_ADDR_OFFSET_CTRL2FRAME	0x01

/* MCP25625 Operation Modes */
#define MCP25625_MODE_NORMAL             0x00
#define MCP25625_MODE_LOOPBACK           0x02
#define MCP25625_MODE_SILENT             0x03
#define MCP25625_MODE_CONFIGURATION      0x04

/* MCP25625_FRAME_OFFSET */
#define MCP25625_FRAME_OFFSET_SIDH       0
#define MCP25625_FRAME_OFFSET_SIDL       1
#define MCP25625_FRAME_OFFSET_EID8       2
#define MCP25625_FRAME_OFFSET_EID0       3
#define MCP25625_FRAME_OFFSET_DLC        4
#define MCP25625_FRAME_OFFSET_D0         5

/* MCP25625_CANINTF */
#define MCP25625_CANINTF_RX0IF           BIT(0)
#define MCP25625_CANINTF_RX1IF           BIT(1)
#define MCP25625_CANINTF_TX0IF           BIT(2)
#define MCP25625_CANINTF_TX1IF           BIT(3)
#define MCP25625_CANINTF_TX2IF           BIT(4)
#define MCP25625_CANINTF_ERRIF           BIT(5)
#define MCP25625_CANINTF_WAKIF           BIT(6)
#define MCP25625_CANINTF_MERRF           BIT(7)

#define MCP25625_INTE_RX0IE              BIT(0)
#define MCP25625_INTE_RX1IE              BIT(1)
#define MCP25625_INTE_TX0IE              BIT(2)
#define MCP25625_INTE_TX1IE              BIT(3)
#define MCP25625_INTE_TX2IE              BIT(4)
#define MCP25625_INTE_ERRIE              BIT(5)
#define MCP25625_INTE_WAKIE              BIT(6)
#define MCP25625_INTE_MERRE              BIT(7)

#define MCP25625_EFLG_EWARN              BIT(0)
#define MCP25625_EFLG_RXWAR              BIT(1)
#define MCP25625_EFLG_TXWAR              BIT(2)
#define MCP25625_EFLG_RXEP               BIT(3)
#define MCP25625_EFLG_TXEP               BIT(4)
#define MCP25625_EFLG_TXBO               BIT(5)
#define MCP25625_EFLG_RX0OVR             BIT(6)
#define MCP25625_EFLG_RX1OVR             BIT(7)

#define MCP25625_TXCTRL_TXREQ			 BIT(3)

#define MCP25625_BFPCTRL_B0BFE           BIT(2)
#define MCP25625_BFPCTRL_B1BFE           BIT(3)

#define MCP25625_CANSTAT_MODE_POS		5
#define MCP25625_CANSTAT_MODE_MASK		(0x07 << MCP25625_CANSTAT_MODE_POS)
#define MCP25625_CANCTRL_MODE_POS		5
#define MCP25625_CANCTRL_MODE_MASK		(0x07 << MCP25625_CANCTRL_MODE_POS)
#define MCP25625_TXBNCTRL_TXREQ_POS		3
#define MCP25625_TXBNCTRL_TXREQ_MASK	(0x01 << MCP25625_TXBNCTRL_TXREQ_POS)

#endif /* _MCP25625_H */