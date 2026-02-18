`timescale 1ns / 1ps
`include "multiplier.v"

module multiplier_tb;
    // Testbench signals
    reg [3:0] a_in;
    reg [3:0] b_in;
    wire [3:0] out_wire;

    // Instantiate the Unit Under Test (UUT)
    multiplier uut (
        .a(a_in),
        .b(b_in),
        .out(out_wire)
    );

    integer i, j;

    initial begin
        // Loop through all 16 possible values for 'a'
        for (i = 0; i < 16; i = i + 1) begin
            // Loop through all 16 possible values for 'b'
            for (j = 0; j < 16; j = j + 1) begin
                a_in = i;
                b_in = j;
                
                // Small delay to allow combinational logic to settle
                #1; 
                
                // %h prints in hex. Use %0h to avoid leading zeros for 4-bit values.
                $display("RESULT: %h %h = %h", a_in, b_in, out_wire);
            end
        end
        
        $finish;

    end
endmodule