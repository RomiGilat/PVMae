`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/07/2024 10:43:32 AM
// Design Name: 
// Module Name: Comparator
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module Comparator(
    input signed [31:0] datain,
    output [0:0] square
);
    reg signed [31:0] datain_reg;
    reg dataout_reg;
    assign square = dataout_reg;


    always @(datain) begin
        datain_reg=datain;
        if (datain_reg<-50) begin //This is the threshold around the 0 crossing that I first designed. It could be better or worse, but it should cover the noise from the ADC
            dataout_reg=1'b0;
        end
        if (datain_reg>50) begin
            dataout_reg=1'b1;
        end
        else begin //If the data lands within the threshold, then dataout_reg keeps it's last value.
            dataout_reg=dataout_reg;
        end
    end

endmodule


