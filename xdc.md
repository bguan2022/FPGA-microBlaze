#### * Create Clock 
create_clock -name clk_ddr -period 6 [get_ports DDR_CLK_IN]


#### * Pin assignment
set_property -dict {PACKAGE_PIN AY9 IOSTANDARD LVCMOS18} [get_ports vcu_pin]

#### * If flops are async to each other 
set_false_path -from [get_pins pin_d1] -to [get_pins pin_d2]

#### * Input / output delay 
set_input_delay -clock clk1 1 [all_inputs]

set_input_delay -clock clk_ddr -max 2.1 [get_ports DDR_IN]

set_input_delay -clock clk_ddr -min 0.9 [get_ports DDR_IN]

Note: Normally in ASIC design, create virtual clock to constraint the I/O