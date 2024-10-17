(define (problem p0_27-problem)
 (:domain p0_27-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 - block
   c1 c2 c3 c4 c5 c6 c7 c8 c9 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 5) (= (capacity c2) 6) (= (capacity c3) 7) (= (capacity c4) 7) (= (capacity c5) 4) (= (capacity c6) 0) (= (capacity c7) 0) (= (capacity c8) 5) (= (capacity c9) 5) (base b5) (in b5 c1) (on b5 c1) (clear b5) (base b13) (in b13 c2) (on b13 c2) (clear b13) (base b21) (in b21 c3) (on b21 c3) (clear b21) (base b16) (in b16 c4) (on b16 c4) (on b22 b16) (in b22 c4) (clear b22) (base b17) (in b17 c5) (on b17 c5) (on b20 b17) (in b20 c5) (clear b20) (base b7) (in b7 c6) (on b7 c6) (on b24 b7) (in b24 c6) (on b9 b24) (in b9 c6) (on b15 b9) (in b15 c6) (on b3 b15) (in b3 c6) (on b26 b3) (in b26 c6) (on b8 b26) (in b8 c6) (on b1 b8) (in b1 c6) (clear b1) (base b2) (in b2 c7) (on b2 c7) (on b14 b2) (in b14 c7) (on b25 b14) (in b25 c7) (on b18 b25) (in b18 c7) (on b11 b18) (in b11 c7) (on b4 b11) (in b4 c7) (on b10 b4) (in b10 c7) (on b23 b10) (in b23 c7) (on b6 b23) (in b6 c7) (on b19 b6) (in b19 c7) (on b12 b19) (in b12 c7) (clear b12) (clear c8) (clear c9))
 (:goal (and (on b21 c1) (on b12 b21) (clear b12) (on b6 c2) (on b22 b6) (on b16 b22) (on b17 b16) (on b7 b17) (on b8 b7) (on b1 b8) (clear b1) (on b2 c3) (on b18 b2) (on b26 b18) (on b3 b26) (on b25 b3) (on b19 b25) (on b9 b19) (on b10 b9) (clear b10) (on b13 c4) (on b5 b13) (on b15 b5) (on b24 b15) (on b11 b24) (on b20 b11) (on b14 b20) (on b23 b14) (on b4 b23) (clear b4)))
)
