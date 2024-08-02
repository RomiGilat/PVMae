`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/05/2024 11:50:47 AM
// Design Name: 
// Module Name: Sample_Clock
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: This module creates a clock that runs at the desired sample frequency of 100 kHz

// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module Sample_Clock #
(
    parameter sample_frequency = 100000 //In Hz
)
(
    input adc_clk,
    output sample_clock
);
    wire [31:0] ADC_frequency;
    reg [31:0] counter;
    reg out_clock=0;
    wire [31:0] transitions;
    assign sample_clock = out_clock;
    assign ADC_frequency = 32'd125000000;
    assign transitions = ADC_frequency/(2*sample_frequency); //if sample frequency=100 kHz then this is 625 transitions
    always @(posedge adc_clk) begin
        counter<=counter+1;
        if (counter==transitions) begin
            out_clock<=~out_clock;
            counter<=0;
        end
        else begin
            out_clock<=out_clock;
        end

    end
endmodule
