(define (problem p72-problem)
 (:domain p72-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 b21 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 6) (= (capacity c2) 6) (= (capacity c3) 0) (= (capacity c4) 10) (= (capacity c5) 9) (base b6) (in b6 c1) (on b6 c1) (on b8 b6) (in b8 c1) (on b17 b8) (in b17 c1) (clear b17) (base b13) (in b13 c2) (on b13 c2) (on b21 b13) (in b21 c2) (on b7 b21) (in b7 c2) (clear b7) (base b3) (in b3 c3) (on b3 c3) (on b20 b3) (in b20 c3) (on b2 b20) (in b2 c3) (on b5 b2) (in b5 c3) (on b19 b5) (in b19 c3) (on b9 b19) (in b9 c3) (on b10 b9) (in b10 c3) (on b14 b10) (in b14 c3) (on b12 b14) (in b12 c3) (on b4 b12) (in b4 c3) (on b16 b4) (in b16 c3) (on b18 b16) (in b18 c3) (on b15 b18) (in b15 c3) (on b1 b15) (in b1 c3) (on b11 b1) (in b11 c3) (clear b11) (clear c4) (clear c5))
 (:goal (and (on b20 c1) (clear b20) (on b12 c2) (on b18 b12) (on b3 b18) (clear b3) (on b15 c3) (on b17 b15) (on b7 b17) (on b10 b7) (on b8 b10) (on b1 b8) (on b6 b1) (clear b6) (on b19 c4) (on b4 b19) (on b11 b4) (on b14 b11) (on b5 b14) (on b2 b5) (on b16 b2) (on b21 b16) (on b13 b21) (on b9 b13) (clear b9)))
)
