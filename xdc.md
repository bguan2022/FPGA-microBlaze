#### * Create Clock 
create_clock -name clk_ddr -period 6 [get_ports DDR_CLK_IN]

set_clock_uncertainty

set_propagated_clock

set_clock_latency

set_clock_group -async [group1] to [group2]

#### * I/O Pin assignment
set_property -dict {PACKAGE_PIN AY9 IOSTANDARD LVCMOS18} [get_ports vcu_pin]

IOSTANDARD: 
    LVCMOS18, LVCMOS12
#### * If flops are async to each other 
set_false_path -from [get_pins pin_d1] -to [get_pins pin_d2]

#### * Input / output delay 
set_input_delay -clock clk1 1 [all_inputs]

set_input_delay -clock clk_ddr -max 2.1 [get_ports DDR_IN]

set_input_delay -clock clk_ddr -min 0.9 [get_ports DDR_IN]

Note: Normally in ASIC design, create virtual clock to constraint the I/O


#### * If data is not required in 1 clock cycle 
set_multicycle_path

#### * If need to set select bit of clock mux 
set_case_analysis


#### * 
