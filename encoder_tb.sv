module encoder_tb();
  
  encoder_spi dut(
    .sck(sck),
    .rst_n(rst_n),
    .miso(miso),
    .cs(cs),
    .din(din),
    .encoder_val_full(encoder_val_full)
  );
  
  always sck = #5 ~sck;
  
  initial begin 
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
