`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 05/14/2024 06:38:53 PM
// Design Name: 
// Module Name: hpf
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


module hpf#(
    parameter signed [15:0] b0=8192,//1, 
    parameter signed [15:0] b1=16384,//-2,
    parameter signed [15:0] b2=8192,//1,
    parameter signed [15:0] a1=32046,//1.956,
    parameter signed [15:0] a2=15679//-0.957
    )
   (
    input signed [31:0] y1,
    output signed [31:0] y2,
    input insclk,
    output outsclk
    );
    reg signed [31:0] y2i=0;
    reg signed [31:0] y2i1=0;
    reg signed [31:0] y2i2=0;
    reg signed [31:0] y1i1=0;
    reg signed [31:0] y1i2=0;
    assign outsclk = insclk;
    assign y2=y2i;
    always @(posedge insclk) begin
        y2i<=($signed(8192)*y1+(-$signed(16384)*y1i1)+$signed(8192)*y1i2+($signed(32046)*y2i1)+(-$signed(15679)*y2i2))>>>15;
    end
    
    always @(negedge insclk) begin
        y2i2<=y2i1;
        y2i1<=y2i;
        y1i2<=y1i1;
        y1i1<=y1;
    end
endmodule