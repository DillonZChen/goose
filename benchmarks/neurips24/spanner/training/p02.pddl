(define (problem spanner-problem)
 (:domain spanner-domain)
 (:objects
   shed location1 - location
   bob - man
 )
 (:init (link shed location1) (= (spanners_at location1) 2) (= (spanners_at shed) 0) (= (spanners_at gate) 0) (at bob shed) (link location1 gate)  (= (carrying bob) 0) (= (loose) 1))
 (:goal (and (= (loose) 0)))
)
