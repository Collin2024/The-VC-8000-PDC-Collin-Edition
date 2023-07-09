\# The VC8000 PDC Collin Edi  on 

The VC8000 PDC is a Personal Decimal Computer with 1,000,000 words of memory. Each word consists of 9 decimal digits. There is an addi  onal 10 words of memory called registers. These are on chip memory.  The machine language instruc  ons for the VC8000 is of the following form  Note: each has the same number of digits. 

`                             `|2 digits             | 1 digit                 |6 digits             |                              |opera  on code | register number |address por  on|   

or 

`                             `|2 digits             | 1 digit                 |1 digit               |  5 digits     |                              |opera  on code | register number |register number| unused      |

` `A machine language program is a sequence of machine language instruc  ons stored in memory. The computer runs a machine language program by execu  ng machine instruc  ons stored in successive words of memory. The VC8000 machine assumes that the first instruc  on to be executed will be at loca  on 100. The following are the machine language instruc  ons for the VC8000. 

\### NAME           OP. CODE     MEANING 

ADD                     01           Reg <-- c(Reg) + c(ADDR)   (The contents of the register specified in the instruc  on and of the memory loca  on specified by the address por  on of the instruc  on are added together. The result is placed in the register.): 

SUBTRACT         02            Reg <-- c(Reg) - c(ADDR) MULTIPLY           03            Reg <-- c(Reg) \* c(ADDR) DIVIDE                 04            Reg <-- c(Reg) / c(ADDR) LOAD                   05            Reg <-- c(ADDR) STORE                06            ADDR <-- c(Reg) 

ADD REG            07            REG1 <--c(REG1) + c(REG2)   (Note: this is the second instruc  on format where two registers are specified.) 

SUB REG            08            REG1 <--c(REG1) - c(REG2)   MULT REG          09            REG1 <--c(REG1) \* c(REG2)   DIV REG              10            REG1 <--c(REG1) / c(REG2)   

READ                   11            A line is read in and the number found there is recorded in the specified memory address.  The register value is ignored. 

WRITE                 12            c(ADDR) is displayed  The register value is ignored. BRANCH              13           go to ADDR for next instruc  on.  The register value is ignored. BRANCH MINUS  14          go to ADDR if c(Reg) < 0 

BRANCH ZERO    15          go to ADDR if c(Reg) = 0 

BRANCH POSITIVE 16       go to ADDR if c(Reg) > 0 

HALT                    17           terminate execu  on.  The register value and address are ignored. 

\## A message from the author of this project To whom it may concern 

This is a term project that I did while I was a student (sophmore) at Ramapo College in the CMPS 361 SOFTWARE\_DESIGN course 

I will leave the notes on the implementa  on for reference for anyone looking at how it works. Not only is this  

a fully working implementa  on that does everything it is supposed to but in addi  on I added some addi  onal touches 

to this assembler like a HD COLOR ASCII graphic duck (my instructors favorite animal) playing Audio 

![](Aspose.Words.eea1f821-c278-4146-ae95-e76377b9b504.001.png)

and Blue Screen Of Death when displaying errors. 

![](Aspose.Words.eea1f821-c278-4146-ae95-e76377b9b504.002.jpeg)

![](Aspose.Words.eea1f821-c278-4146-ae95-e76377b9b504.003.jpeg)

I hope you enjoy this take on the VC 8000 PDC and with that have a good day  

TEST CASES cut and paste into the Test.txt file TEST CASE #1 

- file will read in 2 numbers and do a mul   register opera  on on them 

`        `org    100 

- read in the 1st number 

`    `collin      read    a 

- register loca  on(1 in our case) = a 

`      `load 1,a 

- save a in the specified register loca  on(1) 

`      `store 1,a 

- read in the second number 

`       `read b 

- register loca  on(6 in our case) = b 

`       `load 6,b 

- save b in the specified register loca  on(6) 

`       `store 6,b 

- mul  ply the 2 registers together  
- you can change this opera  on code to "addr" for addi  on "subr" for subtrac  on 
- "multr" for mul  plica  on and "divr" for division (case insensi  ve) 

`       `MULTR 1,6 

- save the resultant in the specified register loca  on 

`       `store 1,a 

- a = the resultant 

`       `load 1,a 

- display the final answer (a) 

`       `write 1,a 

- repeat un  l the solu  on is less then or equal to 0 

`       `bp 1,collin 

- terminate  

`       `halt 

`    `x      dc      5 

`    `y      DS      99 

`    `b      dc      555 

`    `a      dc      100000000         end 

TEST CASE #2 

`        `;this is a test 

`        `org    100 

`        `hi     read    x;this comment is immediately a  er statement         load    1,x 

`        `hay   store   1,y ; This is the another comment. 

`        `write    1,x 

`        `bp      1,hi 

`        `halt 

`    `;test comment 

`    `x      dc      5 

`    `y      ds      99 

`    `b      dc      555 

`    `a      dc      100 

`        `end 

TEST CASE #3 

`         `org 100 

`                `read 0, n 

`        `more        load  1, n; This is a comment 

`    `;Here is a comment that sit on its own line. 

`                `mult 1, fac 

`                `store 1, fac 

`                `load 1, n 

`                `sub 1, one 

`                `store 1, n 

`                `bp 0,more 

`                `write 0, fac 

`                `halt 

`                `n              ds 100; just to show that you code can handle big areas.                 fac           dc 1 

`                `one          dc 1 

`                `test          dc 1234 ; show your program can handle big constants.                 end 
