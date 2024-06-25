arm-none-eabi-gcc -c -o main.o main.c -mthumb -mcpu=cortex-m3 -g -O0 -DSTM32F10X_MD -ICMSIS/device -ICMSIS/core -ISPL/inc -Iinc
arm-none-eabi-gcc -c -o obj/startup.o src/startup.c -mthumb -mcpu=cortex-m3 -g -O0 -DSTM32F10X_MD -ICMSIS/device -ICMSIS/core -ISPL/inc -Iinc
arm-none-eabi-gcc -c -o obj/lcd.o     src/lcd.c     -mthumb -mcpu=cortex-m3 -g -O0 -DSTM32F10X_MD -ICMSIS/device -ICMSIS/core -ISPL/inc -Iinc
arm-none-eabi-gcc -c -o obj/uart.o    src/uart.c    -mthumb -mcpu=cortex-m3 -g -O0 -DSTM32F10X_MD -ICMSIS/device -ICMSIS/core -ISPL/inc -Iinc
arm-none-eabi-gcc -c -o obj/rsku.o    src/rsku.c    -mthumb -mcpu=cortex-m3 -g -O0 -DSTM32F10X_MD -ICMSIS/device -ICMSIS/core -ISPL/inc -Iinc
arm-none-eabi-gcc -c -o obj/sonar.o   src/sonar.c   -mthumb -mcpu=cortex-m3 -g -O0 -DSTM32F10X_MD -ICMSIS/device -ICMSIS/core -ISPL/inc -Iinc
arm-none-eabi-gcc -c -o obj/motor.o   src/motor.c   -mthumb -mcpu=cortex-m3 -g -O0 -DSTM32F10X_MD -ICMSIS/device -ICMSIS/core -ISPL/inc -Iinc
arm-none-eabi-gcc -c -o obj/stepper.o src/stepper.c -mthumb -mcpu=cortex-m3 -g -O0 -DSTM32F10X_MD -ICMSIS/device -ICMSIS/core -ISPL/inc -Iinc
arm-none-eabi-gcc -c -o obj/wheel.o   src/wheel.c   -mthumb -mcpu=cortex-m3 -g -O0 -DSTM32F10X_MD -ICMSIS/device -ICMSIS/core -ISPL/inc -Iinc
arm-none-eabi-gcc -c -o obj/init.o    asm/init.s    -mthumb -mcpu=cortex-m3 -g
arm-none-eabi-ld -Tscript.ld -g -o pike.v1.0.x.elf  main.o obj/startup.o obj/lcd.o obj/uart.o obj/init.o obj/wheel.o obj/rsku.o obj/sonar.o obj/motor.o obj/stepper.o
arm-none-eabi-objcopy -O binary pike.v1.0.x.elf pike.v1.0.2.bin
ping 127.0.0.1 -n 30 > nul