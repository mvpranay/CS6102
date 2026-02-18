// irreducible poly y^2 + y + 1
module gf2_2_multiplier(input wire [1:0] a, input wire [1:0] b, output wire [1:0] out);
    assign out[0] = (a[0] & b[0]) ^ (a[1] & b[1]);
    assign out[1] = (a[1] & b[1]) ^ (a[0] & b[1]) ^ (a[1] & b[0]);
endmodule

module forward_map(input wire [3:0] in, output wire [3:0] out);
    assign out[3] = in[3];
    assign out[2] = in[3] ^ in[2] ^ in[1];
    assign out[1] = in[3] ^ in[2];
    assign out[0] = in[0]; 
endmodule

module reverse_map(input wire [3:0] in, output wire [3:0] out);
    assign out[3] = in[3];
    assign out[2] = in[3] ^ in[1];
    assign out[1] = in[2] ^ in[1];
    assign out[0] = in[0];
endmodule

// irreducible poly x^2 + x + {2}
module multiplier(input wire [3:0] a, input wire [3:0] b, output wire [3:0] out) ;
    // DO NOT CHANGE THE NAME OF THE MODULE AND THE PORTS.
    // Changing the name of the module or ports will lead to a an automatic
    // disqualification
    wire [1:0] a0, a1, b0, b1;
    wire [1:0] a0b0, a0b1, a1b0, a1b1, a1b1_2;

    wire [3:0] p, q, mul_res;
    forward_map f1(a, p);
    forward_map f2(b, q);

    assign a0 = p[1:0];
    assign a1 = p[3:2];
    assign b0 = q[1:0];
    assign b1 = q[3:2];

    gf2_2_multiplier m1(a0, b0, a0b0);
    gf2_2_multiplier m2(a0, b1, a0b1);
    gf2_2_multiplier m3(a1, b0, a1b0);
    gf2_2_multiplier m4(a1, b1, a1b1);

    gf2_2_multiplier m5(a1b1, 2'b10, a1b1_2);

    assign mul_res[1:0] = a0b0 ^ a1b1_2;
    assign mul_res[3:2] = a1b0 ^ a0b1 ^ a1b1;

    reverse_map r(mul_res, out);

endmodule
