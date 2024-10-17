(define (problem p0_25-problem)
 (:domain p0_25-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 24) (= (capacity c2) 4) (= (capacity c3) 3) (= (capacity c4) 0) (= (capacity c5) 6) (base b5) (in b5 c1) (on b5 c1) (clear b5) (base b14) (in b14 c2) (on b14 c2) (on b16 b14) (in b16 c2) (on b4 b16) (in b4 c2) (clear b4) (base b23) (in b23 c3) (on b23 c3) (on b19 b23) (in b19 c3) (on b18 b19) (in b18 c3) (on b2 b18) (in b2 c3) (clear b2) (base b22) (in b22 c4) (on b22 c4) (on b11 b22) (in b11 c4) (on b20 b11) (in b20 c4) (on b21 b20) (in b21 c4) (on b12 b21) (in b12 c4) (on b9 b12) (in b9 c4) (on b8 b9) (in b8 c4) (on b13 b8) (in b13 c4) (on b7 b13) (in b7 c4) (on b15 b7) (in b15 c4) (on b6 b15) (in b6 c4) (on b1 b6) (in b1 c4) (on b17 b1) (in b17 c4) (on b3 b17) (in b3 c4) (on b10 b3) (in b10 c4) (on b25 b10) (in b25 c4) (on b24 b25) (in b24 c4) (clear b24) (clear c5))
 (:goal (and (on b9 c1) (on b16 b9) (on b12 b16) (on b18 b12) (on b8 b18) (on b11 b8) (on b10 b11) (on b4 b10) (on b7 b4) (on b17 b7) (on b25 b17) (on b21 b25) (on b23 b21) (on b19 b23) (on b5 b19) (on b15 b5) (on b14 b15) (on b13 b14) (on b6 b13) (on b2 b6) (on b20 b2) (on b24 b20) (on b1 b24) (on b3 b1) (on b22 b3) (clear b22)))
)
