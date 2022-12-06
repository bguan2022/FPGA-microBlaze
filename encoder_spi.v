
module encoder_reading(
  sck,
  rst_n,
  miso,
  encoder_val,
  enconder_val_full,
  data_valid
);
  
  
input logic miso,

  enum {IDLE, DATA_IN} state, next_state; 
  
  logic [23:0] encoder_val_full;
  logic [18:0] encoder_val;
  logic [4:0]  counter; 
  

      
  always @(posedge sck or negedge rst_n) begin
    if (!rst_n) begin
      state              <= IDLE; 
      encoder_val_full   <= 'x;
    end else begin 
      state             <= next_state; 
      data_valid        <= 1'b0;
      case (state):
        IDLE:
        if (!miso) begin 
            next_state  <= DATA_IN; 
        end 
        
        DATA_IN:
          din         <= {din[22:0],miso}; //shift register 
          if (counter >= 24) begin
            encoder_value_val   <= din;
            next_state          <= IDLE;
            counter             <= '0;
            data_valid          <= 1'b1;
          end else begin 
            counter             <= counter +1 ;
            
          end   
      end 
  end 
  
        assign encoder_val = encoder_val_full[21:3]; //19-bit of data     
end module; 
