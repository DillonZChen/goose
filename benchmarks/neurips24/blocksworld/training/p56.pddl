(define (problem p56-problem)
 (:domain p56-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 7) (= (capacity c2) 7) (= (capacity c3) 3) (= (capacity c4) 0) (= (capacity c5) 8) (base b10) (in b10 c1) (on b10 c1) (clear b10) (base b4) (in b4 c2) (on b4 c2) (clear b4) (base b16) (in b16 c3) (on b16 c3) (on b15 b16) (in b15 c3) (on b2 b15) (in b2 c3) (on b3 b2) (in b3 c3) (on b7 b3) (in b7 c3) (on b1 b7) (in b1 c3) (on b12 b1) (in b12 c3) (clear b12) (base b14) (in b14 c4) (on b14 c4) (on b8 b14) (in b8 c4) (on b11 b8) (in b11 c4) (on b6 b11) (in b6 c4) (on b9 b6) (in b9 c4) (on b5 b9) (in b5 c4) (on b17 b5) (in b17 c4) (on b13 b17) (in b13 c4) (clear b13) (clear c5))
 (:goal (and (on b14 c1) (clear b14) (on b12 c2) (on b5 b12) (on b17 b5) (on b2 b17) (on b9 b2) (on b1 b9) (clear b1) (on b11 c3) (on b16 b11) (on b8 b16) (on b13 b8) (on b10 b13) (on b6 b10) (on b15 b6) (on b4 b15) (on b3 b4) (on b7 b3) (clear b7)))
)
