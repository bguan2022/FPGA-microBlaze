### Pin assignment
set_property -dict {PACKAGE_PIN AY9 IOSTANDARD LVCMOS18} [get_ports vcu_pin]

### If flops are async to each other 
set_false_path -from [get_pins pin_d1] -to [get_pins pin_d2]

### Input / output delay 
set_input_delay -clock clk1 1 [all_inputs]
set_output_delay -clock clk1 1 [all_inputs]
