#ifndef PLATFROM_H
#define PLATFROM_H

#define MAXCPU_MUM 1
#define QEMU_SIM

/* This machine puts UART registers here in physical memory. */
#ifdef QEMU_SIM
#define UART0 0x10000000L
#else
#define UART0_BASE 0x12000000L


typedef struct {
    union {
        __IOM uint32_t RBR;
        __IOM uint32_t DLL;
        __IOM uint32_t THR;
    };
    union {
        __IOM uint32_t DLM;
        __IOM uint32_t IER;
    };
    union {
        __IOM uint32_t IIR;
        __IOM uint32_t FCR;
    };
    __IOM uint32_t LCR;
    __IOM uint32_t MCR;
    __IOM uint32_t LSR;
    __IOM uint32_t MSR;
    __IOM uint32_t SCR;
} UART_TypeDef;

typedef enum uart_stop_bit {
    UART_STOP_BIT_1 = 0,
    UART_STOP_BIT_2 = 1
} UART_STOP_BIT;

typedef enum uart_parity_bit {
    UART_ODD   = 0,
    UART_EVEN  = 1,
    UART_SPACE = 2,
    UART_MARK  = 3
} UART_PARITY_BIT;

#define UART0   ((UART_TypeDef *) UART0_BASE)

void    uart_init( );
// int32_t uart_config_stopbit(UART_TypeDef *uart, UART_STOP_BIT stopbit);
// int32_t uart_enable_paritybit(UART_TypeDef *uart);
// int32_t uart_disable_paritybit(UART_TypeDef *uart);
// int32_t uart_set_parity(UART_TypeDef *uart, UART_PARITY_BIT paritybit);
int32_t uart_write(UART_TypeDef *uart, uint8_t val);
uint8_t uart_read(UART_TypeDef *uart);
// int32_t uart_enable_tx_empt_int(UART_TypeDef *uart);
// int32_t uart_disable_tx_empt_int(UART_TypeDef *uart);
// int32_t uart_set_rx_th(UART_TypeDef *uart, uint8_t th);
// int32_t uart_enable_rx_th_int(UART_TypeDef *uart);
// int32_t uart_disable_rx_th_int(UART_TypeDef *uart);
// int32_t uart_enable_rx_err_int(UART_TypeDef *uart);
// int32_t uart_disable_rx_err_int(UART_TypeDef *uart);
// int32_t uart_get_int_status(UART_TypeDef *uart);
int32_t uart_get_status(UART_TypeDef *uart);
#endif

#endif
