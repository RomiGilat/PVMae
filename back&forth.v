module QPD # // Quarter Period Delay
(
    parameter sample_frequency = 100000 // in Hz
)
(
    input rt,
    input sclock,
    input [7:0] count_quater_period, // This has the delay that the user wants built in. Check C server
    output reg [0:0] trigger // Changed to reg to allow assignment in always block
);
    reg [31:0] counter = 0;
    reg [0:0] wantTrigger = 0;
    reg [0:0] signal = 0;
    reg [0:0] sentTrigger = 0;
    reg [7:0] new_count_quater_period = 0; // This variable helps to check when new measurement parameters have been sent.

    always @(posedge sclock) begin
        if (rt == 1) begin
            if (new_count_quater_period != count_quater_period) begin
                wantTrigger <= 1'b1;
            end
            else if (sentTrigger == 1) begin
                wantTrigger <= 1'b0;
            end
        end
        
        if (wantTrigger == 1) begin
            counter <= counter + 1;
            sentTrigger <= 1'b0;
        end
        else begin
            counter <= 0;
            signal <= 0;
        end
        
        if (counter >= count_quater_period) begin
            signal <= 1'b1;
            sentTrigger <= 1;
            counter <= 0;
            new_count_quater_period <= count_quater_period;
        end
        else begin
            signal <= 0;
        end
        
        trigger <= signal;
    end
endmodule


intcommand: 12
delay: 100
delay: 100
count: 249147
count quarter period: 24.000000
intcommand: 7
delay: 100
count: 250399
count quarter period: 25.000000
intcommand: 9
delay: 25
delay: 25
count: 249147
count quarter period: 24.000000
intcommand: 7
delay: 100
count: 250399
count quarter period: 25.000000
intcommand: 10
delay: 50
delay: 50
count: 250399
count quarter period: 25.000000
intcommand: 7
delay: 100
count: 250399
count quarter period: 25.000000
intcommand: 11
delay: 75
delay: 75
count: 250399
count quarter period: 25.000000
intcommand: 7
delay: 100
count: 249147
count quarter period: 24.000000
intcommand: 12
delay: 96
delay: 96
count: 249147
count quarter period: 24.000000
intcommand: 7
delay: 100
count: 249147
count quarter period: 24.000000
intcommand: 9
delay: 24
delay: 24
count: 249147
count quarter period: 24.000000
intcommand: 7
delay: 100
count: 125199
count quarter period: 12.000000
intcommand: 10
delay: 24
delay: 24
count: 249147
count quarter period: 24.000000

