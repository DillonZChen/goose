;; vehicles=3, packages=2, locations=5, max_capacity=2, out_folder=testing/easy, instance_id=3, seed=1009

(define (problem transport-03)
 (:domain transport)
 (:objects 
    v1 v2 v3 - vehicle
    p1 p2 - package
    l1 l2 l3 l4 l5 - location
    c0 c1 c2 - size
    )
 (:init (capacity v1 c1)
    (capacity v2 c1)
    (capacity v3 c2)
    (capacity-predecessor c0 c1)
    (capacity-predecessor c1 c2)
    (at p1 l1)
    (at p2 l5)
    (at v1 l4)
    (at v2 l2)
    (at v3 l5)
    (road l3 l4)
    (road l4 l3)
    (road l3 l1)
    (road l2 l3)
    (road l5 l3)
    (road l3 l2)
    (road l1 l3)
    (road l3 l5)
    (road l4 l5)
    (road l5 l4)
    (road l2 l5)
    (road l5 l2)
    (road l1 l5)
    (road l5 l1)
    (road l1 l2)
    (road l2 l1)
    (road l1 l4)
    (road l4 l1)
    )
 (:goal  (and 
    (at p1 l4)
    (at p2 l1))))
