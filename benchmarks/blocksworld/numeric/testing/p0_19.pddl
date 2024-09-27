(define (problem p0_19-problem)
 (:domain p0_19-domain)
 (:objects
   b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 b19 b20 - block
   c1 c2 c3 c4 c5 - cylinder
 )
 (:init (arm_empty) (= (capacity c1) 9) (= (capacity c2) 7) (= (capacity c3) 3) (= (capacity c4) 1) (= (capacity c5) 10) (base b14) (in b14 c1) (on b14 c1) (clear b14) (base b15) (in b15 c2) (on b15 c2) (on b17 b15) (in b17 c2) (on b6 b17) (in b6 c2) (clear b6) (base b7) (in b7 c3) (on b7 c3) (on b2 b7) (in b2 c3) (on b19 b2) (in b19 c3) (on b20 b19) (in b20 c3) (on b12 b20) (in b12 c3) (on b4 b12) (in b4 c3) (on b11 b4) (in b11 c3) (clear b11) (base b8) (in b8 c4) (on b8 c4) (on b10 b8) (in b10 c4) (on b18 b10) (in b18 c4) (on b9 b18) (in b9 c4) (on b3 b9) (in b3 c4) (on b1 b3) (in b1 c4) (on b5 b1) (in b5 c4) (on b16 b5) (in b16 c4) (on b13 b16) (in b13 c4) (clear b13) (clear c5))
 (:goal (and (on b10 c1) (on b8 b10) (clear b8) (on b11 c2) (on b2 b11) (on b6 b2) (on b13 b6) (clear b13) (on b9 c3) (on b3 b9) (on b5 b3) (on b15 b5) (on b16 b15) (on b20 b16) (clear b20) (on b19 c4) (on b7 b19) (on b14 b7) (on b18 b14) (on b1 b18) (on b12 b1) (on b4 b12) (on b17 b4) (clear b17)))
)
