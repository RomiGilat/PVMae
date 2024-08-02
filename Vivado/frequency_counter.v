`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: Anton Potocnik
// 
// Create Date: 07.01.2017 22:50:51
// Design Name: 
// Module Name: frequency_counter
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description:  Reciprotial method 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.1 - Reciprotial method implemented
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module frequency_counter #
(
    parameter ADC_WIDTH = 14,
    parameter AXIS_TDATA_WIDTH = 32,
    parameter COUNT_WIDTH = 32,
    parameter HIGH_THRESHOLD = -100,
    parameter LOW_THRESHOLD = -150
)
(
    (* X_INTERFACE_PARAMETER = "FREQ_HZ 125000000" *)
    input   datain,
    input                          clk,
    output                          outclk,
    input [COUNT_WIDTH-1:0]        Ncycles,
    output dataout,
	output [COUNT_WIDTH-1:0]       counter_output
);
    wire    data;
    reg                            state, state_next;
    reg [COUNT_WIDTH-1:0]          counter=0, counter_next=0;
    reg [COUNT_WIDTH-1:0]          counter_output=0, counter_output_next=0;
    reg [COUNT_WIDTH-1:0]          cycle=0, cycle_next=0;
    assign outclk = clk;
    // Wire datain to dataout
    assign  dataout = datain;
    // Extract only the 14-bits of ADC data 
    assign  data = datain;   
    // Handling of the state buffer for finding signal transition at the threshold
    always @(posedge clk) 
    begin
            state <= state_next;
    end   
    always @(*)            // logic for state buffer
    begin
        if (data == 1) begin
            state_next = 1;
            end
        else if (data ==0) begin
            state_next = 0;
            end
        else begin
            state_next = state;
            end
    end
    // Handling of counter, counter_output and cycle buffer
    always @(posedge clk) 
    begin
        counter <= counter_next;
        counter_output <= counter_output_next;
        cycle <= cycle_next;
    end

    always @* // logic for counter, counter_output, and cycle buffer
    begin
        counter_next = counter + 1; // increment on each clock cycle
        counter_output_next = counter_output;
        cycle_next = cycle;
        
        if (state > state_next) // low to high signal transition
        begin
            cycle_next = cycle + 1; // increment on each signal transition
            if (cycle >= Ncycles-1) 
            begin
                counter_next = 0;
                counter_output_next = counter;
                cycle_next = 0;
            end
        end
   end
endmodule
