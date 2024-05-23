`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 05/14/2024 06:38:53 PM
// Design Name: 
// Module Name: lpf
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


module lpf #(
    parameter signed [15:0] b0=507,//0.062, 
    parameter signed [15:0] b1=1014,//0.124,
    parameter signed [15:0] b2=507,//0.062,
    parameter signed [15:0] a1=19586,//1.195,
    parameter signed [15:0] a2=7382//-0.451
    )
   (
    input signed [31:0] x,
    output signed [31:0] y1,
    input insclk,
    output outsclk
    );
    reg signed [31:0] y1i=0;
    reg signed [31:0] y1i1=0;
    reg signed [31:0] y1i2=0;
    reg signed [31:0] xi1=0;
    reg signed [31:0] xi2=0;
    assign outsclk = insclk;
    assign y1=y1i;
    always @(posedge insclk) begin
        //y1i<=((b0*x+b1*xi1+b2*xi2+a1*y1i1+a2*y1i2))>>>14;
        y1i<=($signed(507)*x+$signed(1014)*xi1+$signed(507)*xi2+($signed(19586)*y1i1)+(-$signed(7382)*y1i2))>>>14;
    end
    
    always @(negedge insclk) begin
        y1i2<=y1i1;
        y1i1<=y1i;
        xi2<=xi1;
        xi1<=x;
    end
endmodule
