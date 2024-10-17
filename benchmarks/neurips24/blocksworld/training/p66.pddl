(define (problem p66-problem)
 (:domain p66-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 - block
   c1 c2 c3 c4 c5 c6 c7 c8 c9 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 0) (= (capacity c2) 0) (= (capacity c3) 4) (= (capacity c4) 4) (= (capacity c5) 4) (= (capacity c6) 4) (= (capacity c7) 6) (= (capacity c8) 4) (= (capacity c9) 4) (base b16) (in b16 c1) (on b16 c1) (on b19 b16) (in b19 c1) (on b12 b19) (in b12 c1) (on b4 b12) (in b4 c1) (on b3 b4) (in b3 c1) (on b6 b3) (in b6 c1) (clear b6) (base b20) (in b20 c2) (on b20 c2) (on b14 b20) (in b14 c2) (on b7 b14) (in b7 c2) (on b2 b7) (in b2 c2) (on b13 b2) (in b13 c2) (on b1 b13) (in b1 c2) (on b11 b1) (in b11 c2) (on b15 b11) (in b15 c2) (on b5 b15) (in b5 c2) (on b10 b5) (in b10 c2) (on b17 b10) (in b17 c2) (on b9 b17) (in b9 c2) (on b18 b9) (in b18 c2) (on b8 b18) (in b8 c2) (clear b8) (clear c3) (clear c4) (clear c5) (clear c6) (clear c7) (clear c8) (clear c9))
 (:goal (and (on b8 c1) (clear b8) (on b18 c2) (clear b18) (on b17 c3) (on b15 b17) (on b14 b15) (clear b14) (on b12 c4) (on b2 b12) (on b16 b2) (clear b16) (on b13 c5) (on b3 b13) (on b6 b3) (clear b6) (on b10 c6) (on b1 b10) (on b7 b1) (clear b7) (on b9 c7) (on b5 b9) (on b11 b5) (on b20 b11) (on b19 b20) (on b4 b19) (clear b4)))
)
