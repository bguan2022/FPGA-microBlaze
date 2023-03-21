`timescale 1ns / 1ps

//step 1: configure the camera (using ublaze)
//step 2: configure the cphy core (using ublaze)
//step 3: get raw video stream onto the FPGA
//step 4: reduce data rate
//step 5: send it through the UART 1Mb /s

// given 32 bit header
// https://electronics.stackexchange.com/questions/42236/uart-receiver-clock-speed

module user_datapath(
    /* Video Stream */
    axi_stream_aclk,
    axi_stream_tvalid,
    axi_stream_tdata,
    rst_n,
    /* UART TX */
    clk_uart,   //153.6 kHz for 9600 baud
    uart_txd
    );

    input logic rst_n;
    input logic clk_uart;
    /* Video AXI Stream */
    input logic             axi_stream_aclk;
    input logic             axi_stream_tvalid;
    input logic [415:0]     axi_stream_tdata;

    output logic            uart_txd;

    logic                   vid_payload_crc_error;
    logic       [15:0]      vid_calculated_crc;
    logic       [15:0]      vid_extracted_crc;
    logic                   vid_payload_crc_info_valid;
    logic                   vid_sync_detected;
    logic       [31:0]      vid_pkt_payload_data_be;
    logic                   vid_pkt_info_rdy;
    logic                   rx_esc_cmd_error;
    logic                   eot_sync_error;
    logic                   sot_error;

    logic                   counter;

    /* UART AXI Stream */
    logic [31:0]            uart_s_axis_tdata;
    logic                   uart_s_axis_tvalid;
    logic                   uart_s_axis_tready;


    logic                   uart_busy;
    logic [15:0]            prescale;

    assign uart_s_axis_tdata       = axi_stream_tdata[31:0];
    assign vid_pkt_payload_data_be = axi_stream_tdata[150:119];

    assign prescale                = 0;

    enum {IDLE, AXI_TRANSFER, UART_TRANSFER} state, next_state;

    always @(posedge axi_stream_aclk or negedge rst_n) begin
        if (!rst_n) begin
            next_state <= IDLE;

        end else begin
            state <= next_state;
            case (state)
                IDLE: begin
                    if (uart_s_axis_tready && axi_stream_tvalid) begin
                        next_state  <= AXI_TRANSFER;
                    end
                end

                AXI_TRANSFER: begin
                    next_state <= UART_TRANSFER;
                end


                UART_TRANSFER: begin    //allow time for uart to transfer
                    counter <= counter + 1'b1;
                    if (counter > 400) begin
                        next_state <= IDLE;
                    end
                end

            endcase

        end
    end



    always_comb begin
        case(state)
            IDLE:
                uart_s_axis_tvalid = 0;
            AXI_TRANSFER:
                uart_s_axis_tvalid = 1; //make uart data valid for tx

            UART_TRANSFER:
                uart_s_axis_tvalid = 0;

        endcase
    end
    /*
    genvar i;
    generate
        for (i=0; i<32;i=i+1) begin
            always @(posedge axi_stream_aclk) begin
                if (axi_stream_tvalid && vid_pkt_payload_data_be[i]) begin
                    vid_pkt_payload_data[0:+8*i] <= axi_stream_tdata[406:151];
                end
            end
        end
    endgenerate */


    uart_tx uart_tx_inst(
        .clk            (clk_uart),
        .rst            (!rst_n),
        .s_axis_tdata   (uart_s_axis_tdata),
        .s_axis_tvalid  (uart_s_axis_tvalid),
        .s_axis_tready  (uart_s_axis_tready),
        .txd            (uart_txd),
        .busy           (uart_busy),
        .prescale       (prescale)
    );
endmodule
