    always @(posedge sclock) begin
        if (rt == 1) begin
            // If a new quarter period count is received, enable the trigger request
            if (stored_count_quater_period != count_quater_period) begin
                wantTrigger <= 1;
                stored_count_quater_period <= count_quater_period;
            end
        end

        if (wantTrigger == 1) begin
            counter <= counter + 1;
            sentTrigger <= 0;
        end
        else begin
            counter <= 0;
            signal <= 0;
        end

        // Add a clause to prevent triggering if counter is below 23000
        if (counter >= 23000 && counter == count_quater_period) begin
            signal <= 1;
            sentTrigger <= 1;
            counter <= 0;
            wantTrigger <= 0;
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

