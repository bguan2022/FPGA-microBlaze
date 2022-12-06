module encoder_tb();
  
  encoder_spi dut(
    .sck(sck),
    .rst_n(rst_n),
    .miso(miso),
    .cs(cs),
    .din(din),
    .encoder_val_full(encoder_val_full),
    .data_valid(data_valid)
  );
  
  always sck = #5 ~sck;

  initial begin 
    $monitor("data_valid: %b, encoder data: %b", data_valid,encoder_val_full); 
    cs = 1;
    sck = 1;
    din = 1;
    
    #100; 
    cs = 0;
    #10; 
    cs = 1;
    #100;
  end 
  
endmodule 
