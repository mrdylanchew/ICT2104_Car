add_executable(Pico_M5
        Pico_M5.c
        )

# pull in common dependencies and additional i2c hardware support
target_link_libraries(Pico_M5 pico_stdlib
        pico_stdio
        hardware_adc
        hardware_gpio
        hardware_irq
        hardware_pwm
        hardware_timer
        hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(Pico_M5)

# add url via pico_set_program_url
example_auto_set_url(Pico_M5)

# Enable usb output, disable uart output
pico_enable_stdio_usb(Pico_M5 1)
pico_enable_stdio_uart(Pico_M5 1)