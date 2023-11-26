#include <am.h>
#include <klib.h>
#include <klib-macros.h>
extern int dry_benchmark ();
extern void coremark_benchmark ();

typedef struct {
    volatile uint32_t PADOUT;
} GPIO_TypeDef;

#define GPIO_BASE (0x30000000)
#define GPIO      ((GPIO_TypeDef *) GPIO_BASE)

void rena_logo(){
  // printf("------------------------------------  \r\n");
  putch('\n');
  printf("   ____                               \r\n");
  printf("  |  _ \\    ___   _ __     __ _      \r\n");
  printf("  | |_) |  / _ \\ | '_ \\   / _` |    \r\n");
  printf("  |  _ <  |  __/ | | | | | (_| |      \r\n");
  printf("  |_| \\_\\  \\___| |_| |_|  \\__,_|  \r\n");
  printf("                                      \r\n");
  putch('\n');
  // printf("------------------------------------  \r\n");
}
void hello_show(){
  printf("\n");
  printf("   __    __   _______  __       __        ______      ____    __    ____  ______   .______       __       _______        \r\n");
  printf("  |  |  |  | |   ____||  |     |  |      /  __  \\     \\   \\  /  \\  /   / /  __  \\  |   _  \\     |  |     |       \\    \r\n");
  printf("  |  |__|  | |  |__   |  |     |  |     |  |  |  |     \\   \\/    \\/   / |  |  |  | |  |_)  |    |  |     |  .--.  |     \r\n");
  printf("  |   __   | |   __|  |  |     |  |     |  |  |  |      \\            /  |  |  |  | |      /     |  |     |  |  |  |       \r\n");
  printf("  |  |  |  | |  |____ |  `----.|  `----.|  `--'  |       \\    /\\    /   |  `--'  | |  |\\  \\----.|  `----.|  '--'  |    \r\n");
  printf("  |__|  |__| |_______||_______||_______| \\______/         \\__/  \\__/     \\______/  | _| `._____||_______||_______/      \r\n");
  printf("                                                                                                                           \r\n");
}

void help_show(){
  printf("\n");
  printf("----------------------------------------------\n");
  printf("|        _   _   _____   _       ____        |\n");
  printf("|       | | | | | ____| | |     |  _ \\       |\n");
  printf("|       | |_| | |  _|   | |     | |_) |      |\n");
  printf("|       |  _  | | |___  | |___  |  __/       |\n");
  printf("|       |_| |_| |_____| |_____| |_|          |\n");
  printf("----------------------------------------------\n");
  printf("| 0.help command   (cmd : help)              |\n");
  printf("| 1.hello test     (cmd : hello)             |\n");
  printf("| 2.gpio  test     (cmd : gpio number)       |\n");
  printf("| 3.benchmark      (cmd : drystone/coremark) |\n");
  printf("| 4.intereupt test (cmd : int number)        |\n");
  printf("----------------------------------------------\n");
}

void gpio_test(char num){
  int n = (num - '1');
  if(n > 8 ){
    printf("error led\n");
    return;
  }
  GPIO->PADOUT = 1 << n;
  printf("led state %02x\n",1 << n);
}

void led_show(){
  char led0 = 0x01;
  char led1 = 0x80;
  volatile int j;

  while(1){
    led0 = led0 << 1;
    led1 = led1 >> 1;
    if(led0 == 0) led0 = 0x01;
    if(led1 == 0) led1 = 0x80;
    GPIO->PADOUT = led0 | led1;
    for(j = 0 ; j< 100000 ; j++);
  }
}

int cmd_parse(char * cmd){
  printf("\n 1:%c 2:%c 3:%c 4:%c 5:%c 6:%c 7:%c 8:%c\n",cmd[1],cmd[2],cmd[3],cmd[4],cmd[5],cmd[6],cmd[7],cmd[8]);
  int i = strncmp(cmd,"help",3);
  printf("cmd is help %d\n",i);

  return 0;
}

int main(const char *args) {
  asm volatile("csrs mie, %0" : : "r"(0x080));

  rena_logo();

  char cmd[19];
  int i;

  help_show();
  while(1){
    putstr(">>>");
    memset(cmd , 'x' , 19);
    for(i =0 ;i<18;i++){
      cmd[i+1] = uart_getc();
      if(cmd[i+1] == '\r')
        break;
      printf("%c",cmd[i+1]);

    }

    if (cmd_parse(cmd) == -1){
      printf("command format error\n");
      return 1;
    }

    // help_show();
    // hello_show();
    // gpio_test(1);
    // coremark_benchmark(0,0);
    // dry_benchmark();
    // led_show();


  }

  return 0;
}
