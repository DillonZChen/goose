(define (problem p55-problem)
 (:domain p55-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 5) (= (capacity c2) 0) (= (capacity c3) 7) (= (capacity c4) 6) (= (capacity c5) 6) (base b5) (in b5 c1) (on b5 c1) (on b13 b5) (in b13 c1) (clear b13) (base b8) (in b8 c2) (on b8 c2) (on b9 b8) (in b9 c2) (on b16 b9) (in b16 c2) (on b3 b16) (in b3 c2) (on b11 b3) (in b11 c2) (on b12 b11) (in b12 c2) (on b10 b12) (in b10 c2) (on b7 b10) (in b7 c2) (on b1 b7) (in b1 c2) (on b4 b1) (in b4 c2) (on b15 b4) (in b15 c2) (on b14 b15) (in b14 c2) (on b6 b14) (in b6 c2) (on b2 b6) (in b2 c2) (clear b2) (clear c3) (clear c4) (clear c5))
 (:goal (and (on b4 c1) (on b3 b4) (clear b3) (on b12 c2) (on b16 b12) (on b5 b16) (clear b5) (on b10 c3) (on b11 b10) (on b13 b11) (on b2 b13) (on b14 b2) (clear b14) (on b1 c4) (on b9 b1) (on b6 b9) (on b15 b6) (on b8 b15) (on b7 b8) (clear b7)))
)
