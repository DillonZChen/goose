(define (problem p91-problem)
 (:domain p91-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 - block
   c1 c2 c3 c4 c5 c6 c7 c8 c9 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 4) (= (capacity c2) 2) (= (capacity c3) 0) (= (capacity c4) 0) (= (capacity c5) 6) (= (capacity c6) 5) (= (capacity c7) 13) (= (capacity c8) 5) (= (capacity c9) 5) (base b8) (in b8 c1) (on b8 c1) (on b22 b8) (in b22 c1) (clear b22) (base b20) (in b20 c2) (on b20 c2) (on b16 b20) (in b16 c2) (on b9 b16) (in b9 c2) (on b19 b9) (in b19 c2) (clear b19) (base b23) (in b23 c3) (on b23 c3) (on b21 b23) (in b21 c3) (on b6 b21) (in b6 c3) (on b10 b6) (in b10 c3) (on b4 b10) (in b4 c3) (on b12 b4) (in b12 c3) (on b24 b12) (in b24 c3) (on b1 b24) (in b1 c3) (on b26 b1) (in b26 c3) (on b11 b26) (in b11 c3) (clear b11) (base b25) (in b25 c4) (on b25 c4) (on b14 b25) (in b14 c4) (on b15 b14) (in b15 c4) (on b3 b15) (in b3 c4) (on b18 b3) (in b18 c4) (on b17 b18) (in b17 c4) (on b2 b17) (in b2 c4) (on b5 b2) (in b5 c4) (on b7 b5) (in b7 c4) (on b27 b7) (in b27 c4) (on b13 b27) (in b13 c4) (clear b13) (clear c5) (clear c6) (clear c7) (clear c8) (clear c9))
 (:goal (and (on b20 c1) (clear b20) (on b9 c2) (clear b9) (on b22 c3) (on b23 b22) (clear b23) (on b12 c4) (on b1 b12) (on b13 b1) (clear b13) (on b5 c5) (on b3 b5) (on b27 b3) (clear b27) (on b17 c6) (on b6 b17) (on b7 b6) (on b4 b7) (clear b4) (on b21 c7) (on b24 b21) (on b25 b24) (on b15 b25) (on b10 b15) (on b19 b10) (on b8 b19) (on b14 b8) (on b18 b14) (on b2 b18) (on b16 b2) (on b26 b16) (on b11 b26) (clear b11)))
)
