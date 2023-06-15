; The variants of the Organic Synthesis domain were created by
; Dr. Russell Viirre, Hadi Qovaizi, and Prof. Mikhail Soutchanski.
;
; This work is licensed under a Creative Commons Attribution,
; NonCommercial, ShareAlike 3.0 Unported License.
;
; For further information, please access the following web page:
; https://www.cs.ryerson.ca/~mes/publications/
(define (problem initialBonds15) (:domain Chemical)
(:objects
; water_1
o50 - oxygen
h50 - hydrogen
h51 - hydrogen
; water_2
o51 - oxygen
h52 - hydrogen
h53 - hydrogen
; MeOH 
c1 - carbon
o1 - oxygen
h1 - hydrogen
h2 - hydrogen
h3 - hydrogen
h19 - hydrogen
; EtOH 
c2 - carbon
c3 - carbon
h4 - hydrogen
h5 - hydrogen
h6 - hydrogen
h7 - hydrogen
h8 - hydrogen
h15 - hydrogen
o2 - oxygen
; the carbonyl 
c4 - carbon
o3 - oxygen
h16 - hydrogen
h17 - hydrogen
; free Mg 
mg - magnesium
; second free Mg 
mg2 - magnesium
; free hydrogen 
h18 - hydrogen
; PBr3 
p - phosphorus
br1 - bromine
br2 - bromine
br3 - bromine
; second PBr3 
p2 - phosphorus
br4 - bromine
br5 - bromine
br6 - bromine
; third PBr3 
p3 - phosphorus
br7 - bromine
br8 - bromine
br9 - bromine
; PCC 
c12 - carbon
c13 - carbon
c14 - carbon
c15 - carbon
c16 - carbon
n4 - nitrogen
h9 - hydrogen
h10 - hydrogen
h11 - hydrogen
h12 - hydrogen
h13 - hydrogen
h14 - hydrogen
cr - chromium
o4 - oxygen
o5 - oxygen
o6 - oxygen
cl1 - chlorine
; second PCC 
c17 - carbon
c18 - carbon
c19 - carbon
c20 - carbon
c21 - carbon
n5 - nitrogen
h20 - hydrogen
h21 - hydrogen
h22 - hydrogen
h23 - hydrogen
h24 - hydrogen
h25 - hydrogen
cr2 - chromium
o7 - oxygen
o8 - oxygen
o9 - oxygen
cl2 - chlorine
; second MeOH 
c22 - carbon
o10 - oxygen
h26 - hydrogen
h27 - hydrogen
h28 - hydrogen
h29 - hydrogen
; KCN 
k1 - potassium
c23 - carbon
n6 - nitrogen
; LiAlH4
li1 - lithium
al1 - aluminium
h30 - hydrogen
h31 - hydrogen
h32 - hydrogen
h33 - hydrogen
; second free hydrogen
h34 - hydrogen
;  second LiAlH4 
li2 - lithium
al2 - aluminium
h35 - hydrogen
h36 - hydrogen
h37 - hydrogen
h38 - hydrogen
)
(:init
; water_1
(bond o50 h50)
(bond h50 o50)
(bond o50 h51)
(bond h51 o50)
; water_2
(bond o51 h52)
(bond h52 o51)
(bond o51 h53)
(bond h53 o51)
; MeOH 
(bond c1 o1)
(bond c1 h1)
(bond c1 h2)
(bond c1 h3)
(bond o1 c1)
(bond h1 c1)
(bond h2 c1)
(bond h3 c1)
(bond o1 h19)
(bond h19 o1)
; EtOH 
(bond c2 c3)
(bond c3 c2)
(bond c3 o2)
(bond o2 c3)
(bond c2 h4)
(bond c2 h5)
(bond c2 h6)
(bond h4 c2)
(bond h5 c2)
(bond h6 c2)
(bond c3 h7)
(bond c3 h8)
(bond h7 c3)
(bond h8 c3)
(bond o2 h15)
(bond h15 o2)
; the carbonyl 
(bond h16 c4)
(bond h17 c4)
(bond c4 h16)
(bond c4 h17)
(doublebond o3 c4)
(doublebond c4 o3)
; free Mg 
; second free Mg 
; free hydrogen 
; PBr3 
(bond p br1)
(bond p br2)
(bond p br3)
(bond br1 p)
(bond br2 p)
(bond br3 p)
; second PBr3 
(bond p2 br4)
(bond p2 br5)
(bond p2 br6)
(bond br4 p2)
(bond br5 p2)
(bond br6 p2)
; third PBr3 
(bond p3 br7)
(bond p3 br8)
(bond p3 br9)
(bond br7 p3)
(bond br8 p3)
(bond br9 p3)
; PCC 
(bond n4 h9)
(bond h9 n4)
(aromaticbond c12 n4)
(aromaticbond c12 c13)
(aromaticbond c13 c14)
(aromaticbond c14 c15)
(aromaticbond c15 c16)
(aromaticbond c16 n4)
(aromaticbond n4 c12)
(aromaticbond c13 c12)
(aromaticbond c14 c13)
(aromaticbond c15 c14)
(aromaticbond c16 c15)
(aromaticbond n4 c16)
(bond h10 c12)
(bond h11 c13)
(bond h12 c14)
(bond h13 c15)
(bond h14 c16)
(bond c12 h10)
(bond c13 h11)
(bond c14 h12)
(bond c15 h13)
(bond c16 h14)
(bond o4 cr)
(doublebond cr o5)
(doublebond cr o6)
(bond cr cl1)
(bond cr o4)
(doublebond o5 cr)
(doublebond o6 cr)
(bond cl1 cr)
; second PCC 
(bond n5 h20)
(bond h20 n5)
(aromaticbond c17 n5)
(aromaticbond c17 c18)
(aromaticbond c18 c19)
(aromaticbond c19 c20)
(aromaticbond c20 c21)
(aromaticbond c21 n5)
(aromaticbond n5 c17)
(aromaticbond c18 c17)
(aromaticbond c19 c18)
(aromaticbond c20 c19)
(aromaticbond c21 c20)
(aromaticbond n5 c21)
(bond h21 c17)
(bond h22 c18)
(bond h23 c19)
(bond h24 c20)
(bond h25 c21)
(bond c17 h21)
(bond c18 h22)
(bond c19 h23)
(bond c20 h24)
(bond c21 h25)
(bond o7 cr2)
(doublebond cr2 o8)
(doublebond cr2 o9)
(bond cr2 cl2)
(bond cr2 o7)
(doublebond o8 cr2)
(doublebond o9 cr2)
(bond cl2 cr2)
; second MeOH 
(bond c22 o10)
(bond c22 h26)
(bond c22 h27)
(bond c22 h28)
(bond o10 c22)
(bond h26 c22)
(bond h27 c22)
(bond h28 c22)
(bond o10 h29)
(bond h29 o10)
; KCN 
(bond k1 c23)
(bond c23 k1)
(triplebond c23 n6)
(triplebond n6 c23)
; LiAlH4
(bond al1 h30)
(bond al1 h31)
(bond al1 h32)
(bond al1 h33)
(bond h30 al1)
(bond h31 al1)
(bond h32 al1)
(bond h33 al1)
; second free hydrogen
;  second LiAlH4 
(bond al2 h35)
(bond al2 h36)
(bond al2 h37)
(bond al2 h38)
(bond h35 al2)
(bond h36 al2)
(bond h37 al2)
(bond h38 al2)
)
(:goal
(and
(bond c22 c23)
(bond c23 n6)
(bond n6 h50)
(bond n6 c4)
(bond c1 c4)
(bond c4 c3)
(bond c3 c2)
(bond c22 h26)
(bond c22 h27)
(bond h28 c22)
(bond c23 h30)
(bond c23 h31)
(bond c4 h35)
(bond c1 h1)
(bond c1 h2)
(bond c1 h3)
(bond c3 h7)
(bond c3 h8)
(bond c2 h4)
(bond c2 h5)
(bond c2 h6)
)
)
)
