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


## ASIC Only

The following SDC commands are not supported.

• set_clock_gating_check

• set_clock_transition

• set_ideal_latency

• set_ideal_network

• set_ideal_transition

• set_max_fanout

Note: Maximum fanout is controlled by the MAX_FANOUT attribute during synthesis.

• set_drive

• set_driving_cell

• set_fanout_load

• set_input_transition

• set_max_area

• set_max_capacitance

• set_max_transition

• set_min_capacitance

• set_port_fanout_number

• set_resistance

• set_timing_derate

• set_voltage

• set_wire_load_min_block_size

• set_wire_load_mode

• set_wire_load_model

• set_wire_load_selection_group

• create_voltage_area

• set_level_shifter_strategy

• set_level_shifter_threshold

• set_max_dynamic_power
