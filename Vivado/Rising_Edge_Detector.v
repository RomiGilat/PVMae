`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/06/2024 11:31:04 AM
// Design Name: 
// Module Name: Rising_Edge_Detector
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


module Rising_Edge_Detector(
    input sclk,
    input [0:0] data,
    output rising_edge,
    output outsclk
    
);
    reg [0:0] polarity_next; //Now that the data has been converted to a square wave, a 1 is positive and a 0 is negative
    reg [0:0] polarity;
    reg [0:0] re; //rising edge internal variable
    assign rising_edge=re;
    assign outsclk = sclk;
    always @(*)begin //polarity_next is always updated with new data
        polarity_next<=data;
    end

    always @(posedge sclk) begin
        if (~polarity&&polarity_next) begin //negative to positive  transition
            re<=1'b1;
            polarity<=polarity_next;
        end
        else begin
            re<=1'b0;
            polarity<=polarity_next;
        end
    end

endmodule
