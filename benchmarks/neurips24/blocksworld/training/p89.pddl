(define (problem p89-problem)
 (:domain p89-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 8) (= (capacity c2) 5) (= (capacity c3) 0) (= (capacity c4) 17) (= (capacity c5) 9) (base b1) (in b1 c1) (on b1 c1) (on b21 b1) (in b21 c1) (clear b21) (base b24) (in b24 c2) (on b24 c2) (on b20 b24) (in b20 c2) (on b18 b20) (in b18 c2) (on b10 b18) (in b10 c2) (clear b10) (base b4) (in b4 c3) (on b4 c3) (on b5 b4) (in b5 c3) (on b16 b5) (in b16 c3) (on b26 b16) (in b26 c3) (on b8 b26) (in b8 c3) (on b19 b8) (in b19 c3) (on b7 b19) (in b7 c3) (on b23 b7) (in b23 c3) (on b14 b23) (in b14 c3) (on b13 b14) (in b13 c3) (on b2 b13) (in b2 c3) (on b17 b2) (in b17 c3) (on b11 b17) (in b11 c3) (on b3 b11) (in b3 c3) (on b15 b3) (in b15 c3) (on b12 b15) (in b12 c3) (on b25 b12) (in b25 c3) (on b22 b25) (in b22 c3) (on b6 b22) (in b6 c3) (on b9 b6) (in b9 c3) (clear b9) (clear c4) (clear c5))
 (:goal (and (on b2 c1) (on b20 b2) (clear b20) (on b6 c2) (on b23 b6) (clear b23) (on b15 c3) (on b9 b15) (on b25 b9) (on b18 b25) (on b26 b18) (clear b26) (on b24 c4) (on b19 b24) (on b21 b19) (on b4 b21) (on b1 b4) (on b14 b1) (on b11 b14) (on b10 b11) (on b8 b10) (on b12 b8) (on b16 b12) (on b7 b16) (on b3 b7) (on b13 b3) (on b17 b13) (on b5 b17) (on b22 b5) (clear b22)))
)
