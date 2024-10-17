(define (problem p0_24-problem)
 (:domain p0_24-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 7) (= (capacity c2) 19) (= (capacity c3) 2) (= (capacity c4) 0) (= (capacity c5) 8) (base b24) (in b24 c1) (on b24 c1) (on b22 b24) (in b22 c1) (clear b22) (base b19) (in b19 c2) (on b19 c2) (on b21 b19) (in b21 c2) (on b12 b21) (in b12 c2) (on b5 b12) (in b5 c2) (clear b5) (base b2) (in b2 c3) (on b2 c3) (on b14 b2) (in b14 c3) (on b20 b14) (in b20 c3) (on b6 b20) (in b6 c3) (on b7 b6) (in b7 c3) (on b15 b7) (in b15 c3) (on b17 b15) (in b17 c3) (clear b17) (base b13) (in b13 c4) (on b13 c4) (on b3 b13) (in b3 c4) (on b8 b3) (in b8 c4) (on b11 b8) (in b11 c4) (on b16 b11) (in b16 c4) (on b4 b16) (in b4 c4) (on b1 b4) (in b1 c4) (on b10 b1) (in b10 c4) (on b18 b10) (in b18 c4) (on b9 b18) (in b9 c4) (on b23 b9) (in b23 c4) (clear b23) (clear c5))
 (:goal (and (on b13 c1) (clear b13) (on b20 c2) (on b16 b20) (on b6 b16) (on b2 b6) (on b11 b2) (on b1 b11) (on b14 b1) (on b9 b14) (on b23 b9) (on b22 b23) (on b5 b22) (on b10 b5) (on b24 b10) (on b8 b24) (on b3 b8) (on b7 b3) (on b19 b7) (on b4 b19) (on b12 b4) (on b18 b12) (on b21 b18) (on b15 b21) (on b17 b15) (clear b17)))
)
