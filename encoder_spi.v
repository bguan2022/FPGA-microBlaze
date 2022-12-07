//Max clock 5MHz, ideally  0.5MHz 

//SPI: PIN 1  VDD 
//PIN 2: SCK (G21)
//PIN 3: SDI  (G22)
//PIN 4: CS (active low)
//PIN 5: NC
//PIN 6: GND

// tie cs to a push button on vcu 118

module encoder_reading(
  clk,
  rst_n,
  enable,
  sclk,
  cs,
  miso,
  encoder_val,
  enconder_val_full,
  data_valid
);

  input logic enable,
  input logic clk;
  input logic miso;
  input logic rst_n;
  output logic sclk, cs;

  enum {IDLE, CS, DATA_IN} state, next_state; 
  
  output logic [23:0] encoder_val_full;
  output logic [18:0] encoder_val;
  output logic        data_valid;
  logic        [9:0]  clk_counter;
  logic        [4:0]  data_counter; 
  
  assign sclk = clk_counter[9] ;
    
  always @(posedge clk or negedge rst_n) begin 
    if (!rst_n)
        clk_counter <= '0;
    end else begin 
        clk_counter <= clk_counter + 1'b1;
    end 
  end 
  
  always_comb begin 
    case(state)    
      IDLE: begin
          cs         = 1'b1;
          data_valid = 1'b0;
          if (enable) begin 
                next_state  = CS; 
            end 
          end
      
      CS: 
          cs          = 1'b0;
          next_state  = DATA_IN;
      
      DATA_IN:
          cs         = 1'b0;
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
          din           <= {din[22:0],miso}; //shift register 
          data_counter  <= data_counter +1'b1;
          end
        end 
      endcase
     end 
  end 
  
   assign encoder_val = encoder_val_full[21:3]; //19-bit of data     
  
end module; 
