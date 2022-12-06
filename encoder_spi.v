
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
  
  always_comb begin 
    case(state)    
      IDLE: begin
            data_valid = 1'b0;
            if (!miso) begin 
                next_state  = DATA_IN; 
            end 
          end
      
      DATA_IN:
          data_valid = 1'b0;
          if (counter >= 24) begin
              next_state    = IDLE;
            end
    endcase 
  end 
      
  always @(posedge sck or negedge rst_n) begin
    if (!rst_n) begin
      state              <= IDLE; 
      encoder_val_full   <= 'x;
    end else begin 
      state             <= next_state; 
      case (state):
        IDLE: 
            counter     <= '0;
        DATA_IN: begin 
          din         <= {din[22:0],miso}; //shift register 
          counter     <= counter +1'b1;
          end
        end 
      endcase
     end 
  end 
  
   assign encoder_val = encoder_val_full[21:3]; //19-bit of data     
  
end module; 
