# Hamming_code
Project of course in master degree - Hamming code

#Block Diagram
![Block Diagram](https://github.com/H-Y-Hs/Hamming_code/blob/main/Block_Diagram.jpg?raw=true)

##Input Sim.txt
![Block Diagram](https://github.com/H-Y-Hs/Hamming_code/blob/main/Sim.jpg?raw=true)

###Notes
1. Use the recursion  
$u_{l+6} = u_{l+1} ⊕ u_l$, for $l ≥ 0$  
with the initial conditions  
$u_0 =1,\ u_1 =u_2 =u_3 =u_4 =u_5 =0$ 
to generate $m\cdot k$ information bits where m is the number of test patterns.  
2. The generated sequence is 100000100001 . . . with period 63.
